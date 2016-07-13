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
#include "hidapi.h"
#include ".\\ATMCMsgConvert\\myConvIntHdr.h"

#include "zwHidDevClass2015.h"

using namespace std;
using boost::property_tree::ptree_error;
using boost::property_tree::ptree_bad_data;
using boost::property_tree::ptree_bad_path;
using jchidDevice2015::jcHidDevice;


extern jcHidDevice *g_jhc;	//实际的HID设备类对象
#define DBGTHRID	VLOG(3)<<"["<<__FUNCTION__<<"] ThreadID of Caller is "<<GetCurrentThreadId()<<endl;


namespace jcAtmcConvertDLL
{



	jcLockMsg1512_t::jcLockMsg1512_t(const std::string &notifyJsonMsg)
	{
		DWORD iCallerThrId=GetCurrentThreadId();
		m_CallerThreadID=iCallerThrId;
		m_NotifyMsg=notifyJsonMsg;
		m_UpMsg="";
		m_bSended=false;
		m_pRecvMsgFun=NULL;
		m_NotifyMs=zwccbthr::zwGetMs();
		m_NotifyType=jcAtmcConvertDLL::zwGetJcJsonMsgType(notifyJsonMsg.c_str());
	}

	bool jcLockMsg1512_t::matchResponJsonMsg(const std::string &responeJsonMsg)
	{
		string respMsgType=jcAtmcConvertDLL::zwGetJcJsonMsgType(responeJsonMsg.c_str());
		if (respMsgType==m_NotifyType)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	const std::string & jcLockMsg1512_t::getNotifyMsg(void)
	{
		return m_NotifyMsg;
	}

	const std::string & jcLockMsg1512_t::getNotifyType(void)
	{
		return m_NotifyType;
	}

	std::string jcLockMsg1512_t::getNotifyNumType(void)
	{
		if (m_NotifyType=="Lock_Secret_Key")
		{
			return "0000";
		}
		if (m_NotifyType=="Lock_System_Init")
		{
			return "0001";
		}
		if (m_NotifyType=="Lock_Now_Info")
		{
			return "0002";
		}
		if (m_NotifyType=="Lock_Time_Sync_ATM")
		{
			return "0003";
		}
		if (m_NotifyType=="Lock_Close_Code_ATM")
		{
			return "0004";
		}
		return "1043";
	}

	double jcLockMsg1512_t::getNotifyMs(void)
	{
		return m_NotifyMs;
	}

	void jcLockMsg1512_t::setInitNotifyMs(void)
	{
		m_NotifyMs=zwccbthr::zwGetMs();
	}
}

using jcAtmcConvertDLL::jcLockMsg1512_t;



namespace zwccbthr {
	double zwGetMs(void);	
	void my706LockRecvThr();	//与锁具之间的通讯线程20160706
	void my515UpMsgThr(void);
	boost::thread *opCommThr=NULL;	//为了控制通讯线程终止
	extern boost::mutex thrhid_mutex;
	extern deque<jcLockMsg1512_t *> s_jcNotify;		//下发命令
	extern RecvMsgRotine s_CallBack;
	////供单向上传报文专用的保存所有回调函数指针的向量,好让单向报文发给所有线程;
	extern vector <RecvMsgRotine> s_vecSingleUp;	
} //namespace zwccbthr{  

namespace zwCfg {
	//const long JC_CCBDLL_TIMEOUT = 86400;	//最长超时时间为30秒,用于测试目的尽快达到限制暴露问题
//	const int JC_MSG_MAXLEN = 4 * 1024;	//最长为128字节,用于测试目的尽快达到限制暴露问题
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
	//OutputDebugStringA(m_start.c_str());
	VLOG(4)<<m_start;
}

zw_trace::~zw_trace()
{

	//OutputDebugStringA(m_end.c_str());	
	VLOG(4)<<m_end;
}




extern int G_TESTCB_SUCC;	//是否成功调用了回调函数的一个标志位，仅仅测试用


CCBELOCK_API long JCAPISTD Open(long lTimeOut)
{
	VLOG_IF(3,lTimeOut<=-600 || lTimeOut>600)<<"ZIJIN423 Open Invalid Para 20150423.1559";
	if (NULL==g_jhc)
	{
		g_jhc=new jcHidDevice();
	}

	int elockStatus=JCHID_STATUS_OK;		
	//断线重连探测机制
	elockStatus=g_jhc->SendJson("{   \"Command\": \"Lock_Firmware_Version\",    \"State\": \"get\"}");
	char tmpRecv[256];
	g_jhc->RecvJson(tmpRecv,256);
	VLOG_IF(1,JCHID_STATUS_OK!=elockStatus)
		<<"ZIJIN522 Open ELOCK_ERROR_CONNECTLOST Send get_firmware_version to JinChu Elock Fail!";
	if (JCHID_STATUS_OK!=static_cast<JCHID_STATUS>(elockStatus))
	{
		g_jhc->CloseJc();
		elockStatus=g_jhc->OpenJc();
		VLOG_IF(1,JCHID_STATUS_OK!=elockStatus)
			<<"ZIJIN522 Open ELOCK_ERROR_CONNECTLOST Send get_firmware_version to JinChu Elock Fail on retry 2!";
	}

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
	if (NULL==g_jhc)
	{
		LOG(WARNING)<<"没有Open就执行Close 20151216"<<endl;
		return ELOCK_ERROR_HARDWAREERROR;
	}

	//结束的话，及时中断数据接收线程
	//注意，在这里做Sleep无效，要在主程序中Close之前做Sleep才能
	// 让回调函数收到结果，奇怪。20151210.1706.周伟
	VLOG_IF(3,zwccbthr::opCommThr!=NULL)<<" 现在关闭数据收发的主线程"<<endl;
	zwccbthr::opCommThr->interrupt();
	zwccbthr::opCommThr=NULL;
	Sleep(300);

	if (NULL!=g_jhc)
	{
		VLOG(3)<<__FUNCTION__<<" 现在关闭底层HID设备的连接对象"<<endl;
		delete g_jhc;
		g_jhc=NULL;
	}
		VLOG(3)<<"ZIJIN423 Close ELOCK_ERROR_SUCCESS";
	    return ELOCK_ERROR_SUCCESS;
}

CCBELOCK_API long JCAPISTD Notify(const char *pszMsg)
{
	VLOG(4)<<"Notify开始"<<pszMsg<<endl;
		if (NULL==g_jhc)
		{
			LOG(WARNING)<<"没有Open就执行Notify 20151216"<<endl;
			return ELOCK_ERROR_HARDWAREERROR;
		}

	if (NULL==zwccbthr::opCommThr)
	{
		VLOG(4)<<"Start my706LockRecvThr"<<endl;
		zwccbthr::opCommThr=new boost::thread(zwccbthr::my706LockRecvThr);
	}	

	assert(pszMsg != NULL);
	assert(strlen(pszMsg) >= 42);	//XML至少42字节utf8
	if (pszMsg == NULL || strlen(pszMsg) < 42) {
		ZWERROR("ZIJIN423 Notify ELOCK_ERROR_PARAMINVALID Input NULL or Not Valid XML !")
		return ELOCK_ERROR_PARAMINVALID;
	}

	string strJsonSend;
	if (NULL != pszMsg && strlen(pszMsg) > 0) {
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
		VLOG_IF(4,strXMLSend.size()>0)<<"strXMLSend=\n"<<strXMLSend;
		//XML开头的固定内容38个字符，外加起码一个标签的两对尖括号合计4个字符
		assert(strXMLSend.length() > 42);	
		jcAtmcConvertDLL::zwCCBxml2JCjson(strXMLSend, strJsonSend);
		assert(strJsonSend.length() > 9);	//json最基本的符号起码好像要9个字符左右
		
		//不要一口气下发完毕，导致可能的测试线程过早结束
		Sleep(100);
		boost::mutex::scoped_lock lock(zwccbthr::thrhid_mutex);


		//现在开始一问一答过程，在获得对口回复报文之前不得上传其他报文		
		DWORD iCallerThrId=GetCurrentThreadId();
		jcLockMsg1512_t *nItem=new jcLockMsg1512_t(strJsonSend);
		zwccbthr::s_jcNotify.push_back(nItem);
		VLOG_IF(3,strJsonSend.size()>0)<<"NotifyJson报文="<<strJsonSend;
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
	}	//if (NULL != pszMsg && strlen(pszMsg) > 0) {
	return ELOCK_ERROR_SUCCESS;
}

CCBELOCK_API int JCAPISTD SetRecvMsgRotine(RecvMsgRotine pRecvMsgFun)
{
	DBGTHRID
	G_TESTCB_SUCC=0;
	assert(NULL != pRecvMsgFun);
	if (NULL == pRecvMsgFun) {
		ZWFATAL("注册回调函数不能传入空指针0952")
		    return ELOCK_ERROR_PARAMINVALID;
	}
	DWORD iCallerThrId=GetCurrentThreadId();
	//把回调函数指针保存到线程ID为索引的MAP里面
	//zwccbthr::s_thrIdToPointer[iCallerThrId]=pRecvMsgFun;
	//把回调函数指针保存到一个集合里面供单向上传报文提供给所有线程的不重复回调函数
	zwccbthr::s_vecSingleUp.push_back(pRecvMsgFun);
	zwccbthr::s_CallBack=pRecvMsgFun;
	VLOG(3)<<"zwccbthr::s_vecSingleUp.size()="<<zwccbthr::s_vecSingleUp.size()<<endl;
	VLOG(3)<<"主程序的线程ID="<<iCallerThrId<<"\t回调函数地址="<<std::hex<<pRecvMsgFun<<endl;
	//VLOG(3)<<__FUNCTION__<<" s_thrIdToPointer.size()="<<zwccbthr::s_thrIdToPointer.size();
	return ELOCK_ERROR_SUCCESS;
}


//////////////////////////////////////////////////////////////////////////

namespace jchidDevice2015{
#ifndef _USE_FAKEHID_DEV20160705
	jcHidDevice::jcHidDevice()
	{
		memset(&m_jcElock, 0, sizeof(JCHID));
		m_jcElock.vid = JCHID_VID_2014;
		m_jcElock.pid = JCHID_PID_LOCK5151;
		m_hidOpened=false;
		OpenJc();
	}


	int jcHidDevice::OpenJc()
	{
		boost::mutex::scoped_lock lock(m_jchid_mutex);
		ZWWARN(__FUNCTION__)
		if (JCHID_STATUS_OK != jcHidOpen(&m_jcElock)) {
			ZWERROR("myOpenElock1503 return ELOCK_ERROR_PARAMINVALID "
				"电子锁打开失败 20160706.1656 by Class jcHidDevice");
			m_hidOpened=false;
			return ELOCK_ERROR_PARAMINVALID;
		}
		//hid_set_nonblocking(static_cast<hid_device *>(m_jcElock.hid_device),1);
		ZWWARN("myOpenElock1503 电子锁打开成功20160706.1656 by Class jcHidDevice")
		m_hidOpened=true;
		return ELOCK_ERROR_SUCCESS;
	}

	void jcHidDevice::CloseJc()
	{
		boost::mutex::scoped_lock lock(m_jchid_mutex);
		if (NULL!=m_jcElock.hid_device)
		{
			ZWWARN(__FUNCTION__)
			jcHidClose(&m_jcElock);
			//memset(&m_jcElock,0,sizeof(m_jcElock));
			//要允许反复Open/Close的话，就不能在此把数据结构置零
			m_hidOpened=false;
		}
	}

	jcHidDevice::~jcHidDevice()
	{
		CloseJc();
	}

	int jcHidDevice::getConnectStatus()
	{
		boost::mutex::scoped_lock lock(m_jchid_mutex);
		const char *m_cmdGetFirmware=
			"{   \"Command\": \"Lock_Firmware_Version\",    \"State\": \"get\"}";
		//
		int elockStatus=jcHidSendData(&m_jcElock, m_cmdGetFirmware, 
			strlen(m_cmdGetFirmware));
		VLOG_IF(1,JCHID_STATUS_OK!=elockStatus)
			<<"ZIJIN423 Open ELOCK_ERROR_CONNECTLOST Send "
			"get_firmware_version to JinChu Elock Fail! 20150504.1006";
		if (JCHID_STATUS_OK==elockStatus)
		{
			m_hidOpened=true;
			return ELOCK_ERROR_SUCCESS;
		}
		else
		{
			m_hidOpened=false;
			return ELOCK_ERROR_CONNECTLOST;
		}
	}

	int jcHidDevice::SendJson(const char *jcJson)
	{
		boost::mutex::scoped_lock lock(m_jchid_mutex);
		assert(NULL!=jcJson);
		assert(strlen(jcJson)>2);
		if (NULL==jcJson || strlen(jcJson)==0)
		{
			ZWWARN("jcHidDevice::SendJson can't send NULL json command 20150505.0938")
			return -938;
		}
		int sts=jcHidSendData(&m_jcElock, jcJson, strlen(jcJson));
		VLOG_IF(4,JCHID_STATUS_OK!=sts)<<"jcHidDevice::SendJson FAIL\n";
		return sts;
	}

	int jcHidDevice::RecvJson( char *recvJson,int bufLen )
	{
		boost::mutex::scoped_lock lock(m_jchid_mutex);		
		assert(NULL!=recvJson);
		assert(bufLen>=0);
		if (NULL==recvJson || bufLen<0)
		{			
			ZWWARN("jcHidDevice::RecvJson can't Using NULL buffer to Receive JinChu Lock Respone data")
			return JCHID_STATUS_INPUTNULL;
		}		

		//OutputDebugStringA("415接收一条锁具返回消息开始\n");
		int outLen=0;

		JCHID_STATUS sts=JCHID_STATUS_FAIL;
		sts=jcHidRecvData(&m_jcElock,recvJson, bufLen, &outLen,300*1);
		return sts;
	}

#else
jcHidDevice::jcHidDevice()
{
	memset(&m_jcElock, 0, sizeof(JCHID));
	m_jcElock.vid = 2016;
	m_jcElock.pid = 705;
	m_hidOpened=false;
	OpenJc();
}


int jcHidDevice::OpenJc()
{
	boost::mutex::scoped_lock lock(m_jchid_mutex);
	ZWWARN("myOpenElock1607 模拟电子锁打开成功20160705.1003")
			m_hidOpened=true;
		return ELOCK_ERROR_SUCCESS;
}

void jcHidDevice::CloseJc()
{
	ZWWARN("myOpenElock1607 模拟电子锁关闭成功20160705.1003")
}

jcHidDevice::~jcHidDevice()
{
	CloseJc();
}

int jcHidDevice::getConnectStatus()
{
		return ELOCK_ERROR_SUCCESS;
}

int jcHidDevice::SendJson(const char *jcJson)
{
	boost::mutex::scoped_lock lock(m_jchid_mutex);
	assert(NULL!=jcJson);
	assert(strlen(jcJson)>2);
	if (NULL==jcJson || strlen(jcJson)==0)
	{
		ZWWARN("jcHidDevice::SendJson can't send NULL json command 20150505.0938")
			return -938;
	}
	//int sts=jcHidSendData(&m_jcElock, jcJson, strlen(jcJson));
	//VLOG_IF(4,JCHID_STATUS_OK!=sts)<<"jcHidDevice::SendJson FAIL\n";
	m_dqMockLock.push_back(jcJson);
	JCHID_STATUS sts=JCHID_STATUS_OK;
	return sts;
}


int jcHidDevice::RecvJson( char *recvJson,int bufLen )
{
	const char * rspMsg0000="{\"Command\":\"Lock_Secret_Key\",\"Lock_Time\":1450419505,\"Atm_Serial\":\"440600300030\",\"Lock_Serial\":\"515066001005\",\"Lock_Public_Key\":\"BCE8v73suKOVk7RS/pTDMzdcB7okRziltU5QHk+svURAa6rz2G+1NdSieLW1I/BWdsDw9bexuBeuXOtwyf+9WkM=\"}";
	const char * rspMsg0001="{\"Command\":\"Lock_System_Init\",\"Lock_Time\":1450407913,\"Atm_Serial\":\"12345676789\",\"Lock_Serial\":\"22222222\",\"State\":\"0\",\"Lock_Init_Info\":\"BKg3trzacO+lBkCD7L/gvug//ads3Js821m6viVbqQGjGyFrIN4V7hYHy960AePaqXsq/xtHgUmgou00+2zFCj4=.YW8M2C0Of7z4VFj5a0vWsfIn+kFF6YZq.6zBiQXVe8iuOwRIGT0BpNfVPtJ25WYOT6K/obdkVB2ParXvKBugdVV9MIQwadzqtyK2E9NkTz6DF3tVD6vRpHmaiVHEUCqWHPGq8v8ttNko=\"}";
	const char * rspMsg0002="{\"Command\":\"Lock_Now_Info\",\"Lock_Time\":1450418648,\"Atm_Serial\":\"\",\"Lock_Serial\":\"515066001005\",\"Lock_Status\":\"1,0,0,0,0,0,0,0,28,0,0\"}";
	const char * rspMsg0003="{\"Command\":\"Lock_Time_Sync_ATM\",\"Lock_Serial\":\"515066001005\",\"Ex_Syn_Time\":1450419297,\"Lock_Time\":1450419324,\"Atm_Serial\":\"\"}";
	const char * rspMsg0004="{\"Command\":\"Lock_Close_Code_ATM\",\"Lock_Serial\":\"22222222\",\"Lock_Time\":1450407945,\"Atm_Serial\":\"12345676789\",\"Code\":75020268}";
	const char * rspMsg1000="{\"Command\":\"Lock_Close_Code_Lock\",\"Lock_Time\":1450246882,\"Atm_Serial\":\"123456\",\"Lock_Serial\":\"22222222\",\"Code\":59907695}";
	const char * rspMsg1002="{\"Command\":\"Lock_Open_Ident\",\"Lock_Time\":1450323811,\"Atm_Serial\":\"12345\",\"Lock_Serial\":\"515066001000\",\"Lock_Ident_Info\":52280743}";
	
	
	boost::mutex::scoped_lock lock(m_jchid_mutex);		
	assert(NULL!=recvJson);
	assert(bufLen>=0);
	if (NULL==recvJson || bufLen<0)
	{			
		ZWWARN("jcHidDevice::RecvJson can't Using NULL buffer to Receive JinChu Lock Respone data")
			return JCHID_STATUS_INPUTNULL;
	}		
	Sleep(300);
	//OutputDebugStringA("415接收一条锁具返回消息开始\n");
	int outLen=0;



	static bool mainMsgExeced=false;

	static bool msg1002Send=false;
	//以一定几率插入单向上行报文1002也就是发送验证码
	if (mainMsgExeced==true && msg1002Send==false)
	{
		msg1002Send=true;
		strcpy(recvJson,rspMsg1002);
		
		return JCHID_STATUS_OK;
	}

	static bool msg1000Send=false;
	if (mainMsgExeced==true && msg1000Send==false)
	{
		msg1000Send=true;
		strcpy(recvJson,rspMsg1000);
		
		return JCHID_STATUS_OK;
	}

	//JCHID_STATUS sts=JCHID_STATUS_OK;
	//sts=jcHidRecvData(&m_jcElock,recvJson, bufLen, &outLen,300*1);
	if (m_dqMockLock.size()==0)
	{
		return JCHID_STATUS_FAIL;
	}
	string rtMsg=m_dqMockLock.front();
	string jsonType=jcAtmcConvertDLL::zwGetJcJsonMsgType(rtMsg.c_str());
	string mockRetMsg="";
	if (jcAtmcConvertDLL::JCSTR_LOCK_ACTIVE_REQUEST==jsonType)
	{
		mockRetMsg=rspMsg0000;
	}
	if (jcAtmcConvertDLL::JCSTR_LOCK_INIT==jsonType)
	{
		mockRetMsg=rspMsg0001;		
	}
	if (jcAtmcConvertDLL::JCSTR_QUERY_LOCK_STATUS==jsonType)
	{
		mockRetMsg=rspMsg0002;
	}
	if (jcAtmcConvertDLL::JCSTR_TIME_SYNC==jsonType)
	{
		mockRetMsg=rspMsg0003;
	}
	if (jcAtmcConvertDLL::JCSTR_READ_CLOSECODE==jsonType)
	{
		mockRetMsg=rspMsg0004;
	}
	mainMsgExeced=true;

	if (mockRetMsg.length()<=(bufLen-1))
	{
		strcpy(recvJson,mockRetMsg.c_str());
		m_dqMockLock.pop_front();
		return JCHID_STATUS_OK;
	}
	else
	{
		printf("模拟锁具接收缓冲区太小");
		strcpy(recvJson,"bufTooSmall");
		return JCHID_STATUS_FAIL;
	}
}
#endif // _USE_JCHID_DEV20160705


}	//namespace jchidDevice2015{



