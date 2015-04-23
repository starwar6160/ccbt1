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
	//boost::thread *opCommThr=NULL;	//为了控制通讯线程终止
	boost::thread *opCommThr=new boost::thread(zwccbthr::ThreadLockComm);
	string zwGetLockIP(void);
	extern std::deque < string > dqOutXML;;
	extern boost::mutex recv_mutex;
	extern JCHID hidHandle;
	time_t lastOpen=0;
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
	//OutputDebugStringA(x);
	LOG(INFO)<<x;
}

void ZWDBGWARN(const char *x)
{
	//OutputDebugStringA(x);
	LOG(WARNING)<<x;
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




extern int G_TESTCB_SUCC;	//是否成功调用了回调函数的一个标志位，仅仅测试用
void myCloseElock1503(void);
int myOpenElock1503(JCHID *jcElock)
{
	assert(NULL!=jcElock);
	if (NULL==jcElock)
	{
		return ELOCK_ERROR_PARAMINVALID;
	}		
	if (ELOCK_ERROR_SUCCESS==Open(1))
	{
		return ELOCK_ERROR_SUCCESS;
	}
	memset(jcElock, 0, sizeof(JCHID));
	jcElock->vid = JCHID_VID_2014;
	jcElock->pid = JCHID_PID_LOCK5151;
	if (JCHID_STATUS_OK != jcHidOpen(jcElock)) {
		ZWERROR("return ELOCK_ERROR_PARAMINVALID 电子锁打开失败");
		zwCfg::s_hidOpened=false;
		return ELOCK_ERROR_PARAMINVALID;
	}
	ZWWARN("电子锁打开成功20150401.1059")
	zwCfg::s_hidOpened=true;
	return ELOCK_ERROR_SUCCESS;
}

void myCloseElock1503(void)
{
	//if (NULL!=zwccbthr::opCommThr)
	//{
	//	zwccbthr::opCommThr->interrupt();
	//	zwccbthr::opCommThr->join();
	//	zwccbthr::opCommThr=NULL;
	//}
	if (NULL!=zwccbthr::hidHandle.hid_device
		&& NULL != zwccbthr::hidHandle.vid 
		&& NULL != zwccbthr::hidHandle.pid) {
	jcHidClose(&zwccbthr::hidHandle);
	memset(&zwccbthr::hidHandle,0,sizeof(JCHID));
	zwCfg::s_hidOpened=false;
	}
}

CCBELOCK_API long JCAPISTD Open(long lTimeOut)
{
	VLOG_IF(1,lTimeOut<=0 || lTimeOut>3)<<"ZIJIN423 Open Invalid Para 20150423.1559";
	int elockStatus=zwPushString("{   \"command\": \"Lock_Firmware_Version\",    \"State\": \"get\"}");
	VLOG_IF(1,JCHID_STATUS_OK!=elockStatus)<<"ZIJIN423 Open ELOCK_ERROR_CONNECTLOST Send get_firmware_version to JinChu Elock Fail!";
	if (JCHID_STATUS_OK==elockStatus)
	{
		return ELOCK_ERROR_SUCCESS;
	}
	else
	{
		return ELOCK_ERROR_CONNECTLOST;
	}
	
}

CCBELOCK_API long JCAPISTD Close()
{
	//myCloseElock1503();
	VLOG(2)<<"ZIJIN423 Close ELOCK_ERROR_SUCCESS";
	    return ELOCK_ERROR_SUCCESS;
}

CCBELOCK_API long JCAPISTD Notify(const char *pszMsg)
{
	//通过在Notify函数开始检测是否端口已经打开，没有打开就等待一段时间，避免
	//2014年11月初在广州遇到的没有连接锁具时，ATMC执行0002报文查询锁具状态，
	//反复查询，大量无用日志产生的情况。	
	//if (time(NULL)-zwccbthr::lastOpen>20)
	{
		//myCloseElock1503();
		//Open(1);
	}
	//LOG(INFO)<<"Notify开始####################\n";
	if (false == zwCfg::s_hidOpened) {
		ZWERROR("ZIJIN423 JinChu ELock Not Open")
		return ELOCK_ERROR_CONNECTLOST;
	}
	//ZWFUNCTRACE 
	assert(pszMsg != NULL);
	assert(strlen(pszMsg) >= 42);	//XML至少42字节utf8
	if (pszMsg == NULL || strlen(pszMsg) < 42) {
		ZWERROR("ZIJIN423 Notify ELOCK_ERROR_PARAMINVALID Input NULL or Not Valid XML !")
		return ELOCK_ERROR_PARAMINVALID;
	}
	if (NULL != pszMsg && strlen(pszMsg) > 0) {
	}
	boost::mutex::scoped_lock lock(zwCfg::ComPort_mutex);
	string strJsonSend;
	try {
		//输入必须有内容，但是最大不得长于下位机内存大小，做合理限制
		assert(NULL != pszMsg);
		if (NULL == pszMsg) {
			ZWFATAL("Notify输入为空")
			    return ELOCK_ERROR_PARAMINVALID;
		}
		int inlen = strlen(pszMsg);
		assert(inlen > 0 && inlen < JC_MSG_MAXLEN);
		if (inlen == 0 || inlen >= JC_MSG_MAXLEN) {
			ZWERROR("Notify输入超过最大最小限制");
			return ELOCK_ERROR_PARAMINVALID;
		}
		//////////////////////////////////////////////////////////////////////////
		string strXMLSend = pszMsg;
		VLOG_IF(1,strXMLSend.size()>0)<<"strXMLSend="<<strXMLSend;
		assert(strXMLSend.length() > 42);	//XML开头的固定内容38个字符，外加起码一个标签的两对尖括号合计4个字符
		jcAtmcConvertDLL::zwCCBxml2JCjson(strXMLSend, strJsonSend);
		assert(strJsonSend.length() > 9);	//json最基本的符号起码好像要9个字符左右
		VLOG_IF(1,strJsonSend.size()>0)<<"strJsonSend="<<strJsonSend;
		Sleep(50);			
			//OutputDebugStringA("415下发消息给锁具开始\n");
		int sts=zwPushString(strJsonSend.c_str());
			//OutputDebugStringA("415下发消息给锁具结束\n");
		VLOG_IF(1,JCHID_STATUS_OK!=sts)<<"423下发消息给锁具异常\n";
		return ELOCK_ERROR_SUCCESS;
	}
	catch(ptree_bad_path & e) {
		ZWFATAL(e.what());
		ZWFATAL("CCB下发XML有错误节点路径")
		    return ELOCK_ERROR_NOTSUPPORT;
	}
	catch(ptree_bad_data & e) {
		ZWFATAL(e.what());
		ZWFATAL("CCB下发XML有错误数据内容")
		    return ELOCK_ERROR_PARAMINVALID;
	}
	catch(ptree_error & e) {
		ZWFATAL(e.what());
		ZWFATAL("CCB下发XML有其他未知错误")
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
	G_TESTCB_SUCC=0;
	//ZWFUNCTRACE 
	//boost::mutex::scoped_lock lock(zwCfg::ComPort_mutex);
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
	//ZWFUNCTRACE 
	//assert(pszMsg != NULL && strlen(pszMsg) > 42);
	//boost::mutex::scoped_lock lock(zwCfg::ComPort_mutex);
	//输入必须有内容，但是最大不得长于下位机内存大小，做合理限制
	assert(NULL != pszMsg);
	int inlen = strlen(pszMsg);
	VLOG_IF(1,inlen==0)<<"Callback Function myATMCRecvMsgRotine input is NULL";
	assert(
		//inlen > 0 && 
		inlen < JC_MSG_MAXLEN);
	if (NULL == pszMsg 
		//|| inlen == 0 
		|| inlen >= JC_MSG_MAXLEN) {
		VLOG(1)<<"Callback Function myATMCRecvMsgRotine input TOO LONG";
		return;
	}
	if (inlen>0)	
	{
		G_TESTCB_SUCC=1;	//成功调用了回调函数
		//printf("%s\n",pszMsg);
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




