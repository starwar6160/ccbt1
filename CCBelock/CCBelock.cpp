// CCBelock.cpp : 定义 DLL 应用程序的导出函数。
//
//20140804.1346.目前存在的问题：
//1.发送锁具激活请求：请求报文中没有"State","get"的字段了？没有无用的"Public_Key","123456"字段了？
//2.发送锁具激活信息：JC的应答报文无LOCKNO字段，需要解决；
//JC的应答报文的"激活信息"内容是否是已经解密的PSK?
//3.日期时间字段，我们应答报文里面是原样复制日期时间还是根据实际时间再生成时间字段？
//4.初始化的时候，建行下发报文中有密码服务器公钥，我们的报文里面没有，锁具内部对此进行处理了吗？
//5.新版文档里面"锁具初始化"，请求是“Atm_Serial”，应答是“Lock_Serial”，以哪个为准？

#include "stdafx.h"
#include "CCBelock.h"
#include "zwCcbElockHdr.h"
#include "zwHidComm.h"
#include "zwPocoLog.h"
using namespace std;
using boost::property_tree::ptree_error;
using boost::property_tree::ptree_bad_data;
using boost::property_tree::ptree_bad_path;

namespace zwccbthr {
	void ThreadLockComm();	//与锁具之间的通讯线程
	boost::thread *opCommThr=NULL;	//为了控制通讯线程终止
	string zwGetLockIP(void);
	extern std::deque < string > dqOutXML;;
	extern boost::mutex recv_mutex;
	extern JCHID hidHandle;
} //namespace zwccbthr{  

namespace zwCfg {
	//const long JC_CCBDLL_TIMEOUT = 86400;	//最长超时时间为30秒,用于测试目的尽快达到限制暴露问题
//	const int JC_MSG_MAXLEN = 4 * 1024;	//最长为128字节,用于测试目的尽快达到限制暴露问题
	//定义一个回调函数指针
	RecvMsgRotine g_WarnCallback = NULL;
	boost::mutex ComPort_mutex;	//用于保护串口连接对象
	//线程对象作为一个全局静态变量，则不需要显示启动就能启动一个线程
	boost::thread * thr = NULL;
	bool s_hidOpened = false;
} //namespace zwCfg{  


void ZWDBGMSG(const char *x)
{
	OutputDebugStringA(x);
	LOG(INFO)<<x;
}

zw_trace::zw_trace(const char *funcName)
{
	m_str = funcName;
	m_start = m_str + "\tSTART";
	m_end = m_str + "\tEND";
	OutputDebugStringA(m_start.c_str());
	LOG(INFO)<<m_start;
}

zw_trace::~zw_trace()
{

	OutputDebugStringA(m_end.c_str());	
	LOG(INFO)<<m_end;
}





CCBELOCK_API long JCAPISTD Open(long lTimeOut)
{
	char buf[256];
	memset(buf, 0, 256);
	sprintf(buf, "Open incoming timeout value seconds is %d", lTimeOut);
	OutputDebugStringA(buf);
	if (NULL!=zwccbthr::opCommThr)
	{
		ZWNOTICE("关闭上次没有关闭的数据接收线程")
		Close();//如果上次没有Close就直接再次Open，则首先Close一次；
	}
#ifdef _DEBUG327
//既然暂时不用超时参数，就暂不检测了
	assert(lTimeOut >= -1 && lTimeOut <= JC_CCBDLL_TIMEOUT);
	if (lTimeOut < -1 || lTimeOut > JC_CCBDLL_TIMEOUT) {
		memset(buf, 0, 256);
		sprintf(buf,
			"Open Function incoming timeout Value must in -1 to %d seconds",
			JC_CCBDLL_TIMEOUT);
		OutputDebugStringA(buf);
		LOG(ERROR)<<
		    "Open Function incoming timeout Value must in -1 to " <<
		    JC_CCBDLL_TIMEOUT << "seconds";
		ZWINFO("return ELOCK_ERROR_PARAMINVALID 入口参数非法")
		return ELOCK_ERROR_PARAMINVALID;
	}
#endif // _DEBUG327
	ZWFUNCTRACE boost::mutex::scoped_lock lock(zwCfg::ComPort_mutex);
	//必须大于0，小于JC_CCBDLL_TIMEOUT，限制在一个合理范围内
	//LOG(WARNING) << "Open Return " << ELOCK_ERROR_SUCCESS << endl;
	string myLockIp;
	try {

#ifdef ZWUSE_HID_MSG_SPLIT
		//if (true == zwCfg::s_hidOpened) {
		//	ZWNOTICE("return ELOCK_ERROR_SUCCESS 已经打开电子锁，不再重复打开.")
		//	return ELOCK_ERROR_SUCCESS;
		//}
		memset(&zwccbthr::hidHandle, 0, sizeof(JCHID));
		zwccbthr::hidHandle.vid = JCHID_VID_2014;
		zwccbthr::hidHandle.pid = JCHID_PID_LOCK5151;
		if (JCHID_STATUS_OK != jcHidOpen(&zwccbthr::hidHandle)) {
			ZWERROR("return ELOCK_ERROR_PARAMINVALID 电子锁打开失败");
			return ELOCK_ERROR_PARAMINVALID;
		}
		zwCfg::s_hidOpened = true;
#endif // ZWUSE_HID_MSG_SPLIT
		//启动通信线程
		//boost::thread thr(zwccbthr::ThreadLockComm);
		zwccbthr::opCommThr=new boost::thread(zwccbthr::ThreadLockComm);
	}
	catch(...) {
		string errMsg = "打开端口" + myLockIp + "失败";
		ZWFATAL(errMsg.c_str())
	}

	ZWNOTICE("return ELOCK_ERROR_SUCCESS 成功打开电子锁")
	    return ELOCK_ERROR_SUCCESS;
}

CCBELOCK_API long JCAPISTD Close()
{
	ZWFUNCTRACE boost::mutex::scoped_lock lock(zwCfg::ComPort_mutex);
	//zwCfg::g_WarnCallback = NULL;
	//关闭操作要点：先中断数据接收线程，然后join等待其中断完成，
	// 然后将线程对象指针置位NULL,下次就可以成功打开了
	if (NULL!=zwccbthr::opCommThr)
	{
		zwccbthr::opCommThr->interrupt();
		zwccbthr::opCommThr->join();
		zwccbthr::opCommThr=NULL;
	}
#ifdef ZWUSE_HID_MSG_SPLIT
	//goto CloseHidEnd;
	if (NULL != zwccbthr::hidHandle.vid && NULL != zwccbthr::hidHandle.pid) {
		//if (true==s_hidOpened)
		//{
		jcHidClose(&zwccbthr::hidHandle);
		//}             
	}
#else
	if (NULL != zwccbthr::zwComPort) {
		delete zwccbthr::zwComPort;
		zwccbthr::zwComPort = NULL;
	}
#endif // ZWUSE_HID_MSG_SPLIT
	zwCfg::s_hidOpened = false;
	ZWNOTICE("关闭 到锁具的连接")
	    return ELOCK_ERROR_SUCCESS;
}

CCBELOCK_API long JCAPISTD Notify(const char *pszMsg)
{
	//通过在Notify函数开始检测是否端口已经打开，没有打开就直接返回，避免
	//2014年11月初在广州遇到的没有连接锁具时，ATMC执行0002报文查询锁具状态，
	//反复查询，大量无用日志产生的情况。
	if (false == zwCfg::s_hidOpened) {
		return ELOCK_ERROR_CONNECTLOST;
	}
	ZWFUNCTRACE assert(pszMsg != NULL);
	assert(strlen(pszMsg) >= 42);	//XML至少42字节utf8
	if (pszMsg == NULL || strlen(pszMsg) < 42) {
		return ELOCK_ERROR_PARAMINVALID;
	}
	if (NULL != pszMsg && strlen(pszMsg) > 0) {
		LOG(INFO) << "CCB下发XML=" << endl << pszMsg <<
		    endl;
	}
	boost::mutex::scoped_lock lock(zwCfg::ComPort_mutex);
	string strJsonSend;
	try {
		//输入必须有内容，但是最大不得长于下位机内存大小，做合理限制
		assert(NULL != pszMsg);
		if (NULL == pszMsg) {
			ZWERROR("Notify输入为空")
			    return ELOCK_ERROR_PARAMINVALID;
		}
		int inlen = strlen(pszMsg);
		assert(inlen > 0 && inlen < JC_MSG_MAXLEN);
		if (inlen == 0 || inlen >= JC_MSG_MAXLEN) {
			ZWWARN("Notify输入超过最大最小限制");
			return ELOCK_ERROR_PARAMINVALID;
		}
		//////////////////////////////////////////////////////////////////////////
		string strXMLSend = pszMsg;
		assert(strXMLSend.length() > 42);	//XML开头的固定内容38个字符，外加起码一个标签的两对尖括号合计4个字符
		jcAtmcConvertDLL::zwCCBxml2JCjson(strXMLSend, strJsonSend);
		assert(strJsonSend.length() > 9);	//json最基本的符号起码好像要9个字符左右
		ZWNOTICE(strJsonSend.c_str());
		Sleep(50);
		zwPushString(strJsonSend.c_str());
		return ELOCK_ERROR_SUCCESS;
	}
	catch(ptree_bad_path & e) {
		ZWERROR(e.what());
		ZWERROR("CCB下发XML有错误节点路径")
		    return ELOCK_ERROR_NOTSUPPORT;
	}
	catch(ptree_bad_data & e) {
		ZWERROR(e.what());
		ZWERROR("CCB下发XML有错误数据内容")
		    return ELOCK_ERROR_PARAMINVALID;
	}
	catch(ptree_error & e) {
		ZWERROR(e.what());
		ZWERROR("CCB下发XML有其他未知错误")
		    return ELOCK_ERROR_CONNECTLOST;
	}
	catch(...) {		//一切网络异常都直接返回错误。主要是为了捕捉未连接时
		//串口对象为空造成访问NULL指针的的SEH异常  
		//为了使得底层Poco库与cceblock类解耦，从我的WS类
		//发现WS对象因为未连接而是NULL时直接throw一个枚举
		//然后在此，也就是上层捕获。暂时不知道捕获精确类型
		//所以catch所有异常了
		ZWFATAL(__FUNCTION__);
		ZWFATAL("Notify通过线路发送数据异常，可能网络故障")
		    return ELOCK_ERROR_CONNECTLOST;
	}
}

CCBELOCK_API int JCAPISTD SetRecvMsgRotine(RecvMsgRotine pRecvMsgFun)
{
	ZWFUNCTRACE boost::mutex::scoped_lock lock(zwCfg::ComPort_mutex);
	assert(NULL != pRecvMsgFun);
	if (NULL == pRecvMsgFun) {
		ZWFATAL("注册回调函数不能传入空指针0952")
		    return ELOCK_ERROR_PARAMINVALID;
	}
	zwCfg::g_WarnCallback = pRecvMsgFun;
	return ELOCK_ERROR_SUCCESS;
}


void cdecl myATMCRecvMsgRotine(const char *pszMsg)
{
	ZWFUNCTRACE assert(pszMsg != NULL && strlen(pszMsg) > 42);
	boost::mutex::scoped_lock lock(zwCfg::ComPort_mutex);
	//输入必须有内容，但是最大不得长于下位机内存大小，做合理限制
	assert(NULL != pszMsg);
	int inlen = strlen(pszMsg);
	assert(inlen > 0 && inlen < JC_MSG_MAXLEN);
	if (NULL == pszMsg || inlen == 0 || inlen >= JC_MSG_MAXLEN) {
		return;
	}
}

CCBELOCK_API const char *dbgGetLockReturnXML(void)
{
	boost::mutex::scoped_lock lock(zwccbthr::recv_mutex);
	static string sg_outxml;
	if (!zwccbthr::dqOutXML.empty()) {
		sg_outxml = zwccbthr::dqOutXML.front();
		zwccbthr::dqOutXML.pop_front();
		return sg_outxml.c_str();
	} else {
		return "NODATA916";
	}

}



#ifdef _DEBUG330B
class zwJcElock1503
{
public:
	zwJcElock1503(int msTimeout);
	~zwJcElock1503();
	int SetCallBack(RecvMsgRotine pRecvMsgFun);
	int Notify(const char *pszMsg);
	int getStatus(){return m_status;}	
private:
	JC_CCBELOCK_ERROR_CODE m_status;	//返回值
	boost::thread *m_opCommThr;	
	boost::mutex m_hidMutex;	//用于保护串口连接对象
	boost::mutex thr_mutex;	//保护通信线程只启动一个
	RecvMsgRotine m_WarnCallback;
	bool m_hidOpened;	
	JCHID m_hidHandle;
	std::deque < string > dqOutXML;;
	boost::mutex recv_mutex;	
	void StartRecvThread();
	void StopRecvThread();
	friend void ThreadLockComm330();
};

zwJcElock1503 g_jcElock(1);

zwJcElock1503::zwJcElock1503(int msTimeout)
{
	boost::mutex::scoped_lock lock(m_hidMutex);
	memset(&m_hidHandle, 0, sizeof(JCHID));
	m_hidHandle.vid = JCHID_VID_2014;
	m_hidHandle.pid = JCHID_PID_LOCK5151;
	if (JCHID_STATUS_OK != jcHidOpen(&m_hidHandle)) {
		ZWERROR("return ELOCK_ERROR_PARAMINVALID 电子锁打开失败");
		m_status=ELOCK_ERROR_PARAMINVALID;
		return ;
	}
	m_hidOpened = true;
	//启动通信线程
	//zwccbthr::opCommThr=new boost::thread(zwccbthr::ThreadLockComm);
	m_opCommThr=new boost::thread(&ThreadLockComm330);
	StartRecvThread();
	ZWNOTICE("return ELOCK_ERROR_SUCCESS 成功打开电子锁")
		m_status=ELOCK_ERROR_SUCCESS;
	return ;

}

zwJcElock1503::~zwJcElock1503()
{
	boost::mutex::scoped_lock lock(m_hidMutex);
	//关闭操作要点：先中断数据接收线程，然后join等待其中断完成，
	// 然后将线程对象指针置位NULL,下次就可以成功打开了
	//if (NULL!=m_opCommThr)
	//{
	//	m_opCommThr->interrupt();
	//	m_opCommThr->join();
	//	m_opCommThr=NULL;
	//	delete m_opCommThr;
	//}
	StopRecvThread();
	m_WarnCallback = NULL;
	if (NULL != m_hidHandle.vid && NULL != m_hidHandle.pid) {
		jcHidClose(&m_hidHandle);
		memset(&m_hidHandle, 0, sizeof(JCHID));
	}
	m_hidOpened = false;
	ZWNOTICE("关闭 到锁具的连接")
	m_status=ELOCK_ERROR_SUCCESS;

}



void zwJcElock1503::StartRecvThread()
{
	assert(NULL!=m_hidHandle.hid_device);
	if (NULL==m_hidHandle.hid_device)
	{
		LOG(ERROR)<<"jcHidDevice Not Open,cant't Start RecvThread"<<endl;
	}
	//声明一个函数对象，尖括号内部，前面是函数返回值，括号内部是函数的一个或者多个参数(形参)，估计是逗号分隔，
	//后面用boost::bind按照以下格式把函数指针和后面_1形式的一个或者多个参数(形参)绑定成为一个函数对象
	//boost::function<int (JCHID *)> memberFunctionWrapper(boost::bind(&zwJcElock1503::ThreadLockComm330, this,_1));  	
	//再次使用boost::bind把函数对象与实参绑定到一起，就可以传递给boost::thread作为线程体函数了
	//m_opCommThr=new boost::thread(boost::bind(memberFunctionWrapper,&m_hidHandle));	
	Sleep(5);	//等待线程启动完毕，其实也就2毫秒一般就启动了；
}

void zwJcElock1503::StopRecvThread()
{
	assert(NULL!=m_opCommThr);
	assert(NULL!=m_hidHandle.hid_device);
	if (NULL!=m_opCommThr && NULL!=m_hidHandle.hid_device)
	{
		m_opCommThr->interrupt();
		m_opCommThr->join();
	}
}

int zwJcElock1503::SetCallBack(RecvMsgRotine pRecvMsgFun)
{
	boost::mutex::scoped_lock lock(m_hidMutex);
	assert(NULL != pRecvMsgFun);
	if (NULL == pRecvMsgFun) {
		ZWFATAL("注册回调函数不能传入空指针0952")
			return ELOCK_ERROR_PARAMINVALID;
	}
	m_WarnCallback = pRecvMsgFun;
	m_status=ELOCK_ERROR_SUCCESS;
	return m_status;
}

int zwJcElock1503::Notify(const char *pszMsg)
{
	boost::mutex::scoped_lock lock(m_hidMutex);
	//通过在Notify函数开始检测是否端口已经打开，没有打开就直接返回，避免
	//2014年11月初在广州遇到的没有连接锁具时，ATMC执行0002报文查询锁具状态，
	//反复查询，大量无用日志产生的情况。
	if (false == m_hidOpened) {
		return ELOCK_ERROR_CONNECTLOST;
	}
	ZWFUNCTRACE assert(pszMsg != NULL);
	assert(strlen(pszMsg) >= 42);	//XML至少42字节utf8
	if (pszMsg == NULL || strlen(pszMsg) < 42) {
		return ELOCK_ERROR_PARAMINVALID;
	}
	if (NULL != pszMsg && strlen(pszMsg) > 0) {
		LOG(INFO) << "CCB下发XML=" << endl << pszMsg <<
			endl;
	}

	string strJsonSend;
	try {
		//输入必须有内容，但是最大不得长于下位机内存大小，做合理限制
		assert(NULL != pszMsg);
		if (NULL == pszMsg) {
			ZWERROR("Notify输入为空")
				return ELOCK_ERROR_PARAMINVALID;
		}
		int inlen = strlen(pszMsg);
		assert(inlen > 0 && inlen < JC_MSG_MAXLEN);
		if (inlen == 0 || inlen >= JC_MSG_MAXLEN) {
			ZWWARN("Notify输入超过最大最小限制");
			return ELOCK_ERROR_PARAMINVALID;
		}
		//////////////////////////////////////////////////////////////////////////
		string strXMLSend = pszMsg;
		assert(strXMLSend.length() > 42);	//XML开头的固定内容38个字符，外加起码一个标签的两对尖括号合计4个字符
		jcAtmcConvertDLL::zwCCBxml2JCjson(strXMLSend, strJsonSend);
		assert(strJsonSend.length() > 9);	//json最基本的符号起码好像要9个字符左右
		ZWNOTICE(strJsonSend.c_str());
		Sleep(50);
		zwPushString(strJsonSend.c_str());
		return ELOCK_ERROR_SUCCESS;
	}
	catch(ptree_bad_path & e) {
		ZWERROR(e.what());
		ZWERROR("CCB下发XML有错误节点路径")
			return ELOCK_ERROR_NOTSUPPORT;
	}
	catch(ptree_bad_data & e) {
		ZWERROR(e.what());
		ZWERROR("CCB下发XML有错误数据内容")
			return ELOCK_ERROR_PARAMINVALID;
	}
	catch(ptree_error & e) {
		ZWERROR(e.what());
		ZWERROR("CCB下发XML有其他未知错误")
			return ELOCK_ERROR_CONNECTLOST;
	}
	catch(...) {		//一切网络异常都直接返回错误。主要是为了捕捉未连接时
		//串口对象为空造成访问NULL指针的的SEH异常  
		//为了使得底层Poco库与cceblock类解耦，从我的WS类
		//发现WS对象因为未连接而是NULL时直接throw一个枚举
		//然后在此，也就是上层捕获。暂时不知道捕获精确类型
		//所以catch所有异常了
		ZWFATAL(__FUNCTION__);
		ZWFATAL("Notify通过线路发送数据异常，可能网络故障")
			return ELOCK_ERROR_CONNECTLOST;
	}

	return ELOCK_ERROR_SUCCESS;
}


void ThreadLockComm330(){
	ZWFUNCTRACE boost::mutex::scoped_lock lock(g_jcElock.thr_mutex);

	try {			
		const int BLEN = 1024;
		char recvBuf[BLEN + 1];
		memset(recvBuf, 0, BLEN + 1);
		int outLen = 0;
		while (1) {
			printf("\n###############JCCOMMTHREAD 327 RUNNING\n");
			ZWINFO("连接锁具成功");
			try {
				boost::this_thread::interruption_point();
				JCHID_STATUS sts=
					jcHidRecvData(&g_jcElock.m_hidHandle,
					recvBuf, BLEN, &outLen,JCHID_RECV_TIMEOUT);
				//zwCfg::s_hidOpened=true;	//算是通信线程的一个心跳标志					
				//要是什么也没收到，就直接进入下一个循环
				if (JCHID_STATUS_OK!=sts)
				{
					printf("JCHID_STATUS 1225 %d\n",sts);
					Sleep(900);
					continue;
				}
				printf("\n");
				ZWNOTICE("wkThr成功从锁具接收数据如下：");
			}
			catch(boost::thread_interrupted &e)
			{
				ZWERROR
					("RecvData从电子锁接收数据时到遇到线程收到终止信号，数据接收线程将终止");
				return;
			}
			catch(...) {
				ZWERROR
					("RecvData从电子锁接收数据时到遇到线路错误或者未知错误，数据接收线程将终止");
				return;
			}
			ZWNOTICE(recvBuf);

			boost::this_thread::interruption_point();
			string outXML;
			jcAtmcConvertDLL::zwJCjson2CCBxml(recvBuf,
				outXML);
			ZWINFO("分析锁具回传的Json并转换为建行XML成功");
			//XML开头的固定内容38个字符，外加起码一个标签的两对尖括号合计4个字符
			assert(outXML.length() > 42);
			ZWDBGMSG(outXML.c_str());
			{
				boost::
					mutex::scoped_lock lock(g_jcElock.recv_mutex);
				//收到的XML存入队列
				g_jcElock.dqOutXML.push_back(outXML);
			}

			if (NULL != g_jcElock.m_WarnCallback) {
				//调用回调函数传回信息，然后就关闭连接，结束通信线程；
				g_jcElock.m_WarnCallback(outXML.c_str());
				ZWINFO
					("成功把从锁具接收到的数据传递给回调函数");
			} else {
				ZWWARN
					("回调函数指针为空，无法调用回调函数")
			}
		}
		ZWINFO("金储通信数据接收线程正常退出");

	}		//try
	catch(...) {			
		//异常断开就设定该标志为FALSE,以便下次Open不要再跳过启动通信线程的程序段
		g_jcElock.m_hidOpened=false;
		ZWFATAL
			("金储通信数据接收线程数据连接异常断开，现在数据接收线程将结束");
		return;
	}	
}



//CCBELOCK_API long	JCAPISTD Open(long lTimeOut)
//{
//	return g_jcElock.getStatus();
//}
//CCBELOCK_API long	JCAPISTD Close()
//{
//	return ELOCK_ERROR_SUCCESS;
//}
//
//CCBELOCK_API long	JCAPISTD Notify(const char *pszMsg)
//{
//	return g_jcElock.Notify(pszMsg);
//
//}
//CCBELOCK_API int	JCAPISTD SetRecvMsgRotine(RecvMsgRotine pRecvMsgFun)
//{
//	return g_jcElock.SetCallBack(pRecvMsgFun);
//}
//
#endif // _DEBUG330B
