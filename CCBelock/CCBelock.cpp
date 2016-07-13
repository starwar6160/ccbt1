// CCBelock.cpp : ���� DLL Ӧ�ó���ĵ���������
//
//20140804.1346.Ŀǰ���ڵ����⣺
//1.�������߼���������������û��"State","get"���ֶ��ˣ�û�����õ�"Public_Key","123456"�ֶ��ˣ�
//2.�������߼�����Ϣ��JC��Ӧ������LOCKNO�ֶΣ���Ҫ�����
//JC��Ӧ���ĵ�"������Ϣ"�����Ƿ����Ѿ����ܵ�PSK?
//3.����ʱ���ֶΣ�����Ӧ����������ԭ����������ʱ�仹�Ǹ���ʵ��ʱ��������ʱ���ֶΣ�
//4.��ʼ����ʱ�򣬽����·��������������������Կ�����ǵı�������û�У������ڲ��Դ˽��д�������
//5.�°��ĵ�����"���߳�ʼ��"�������ǡ�Atm_Serial����Ӧ���ǡ�Lock_Serial�������ĸ�Ϊ׼��

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


extern jcHidDevice *g_jhc;	//ʵ�ʵ�HID�豸�����
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
	void my706LockRecvThr();	//������֮���ͨѶ�߳�20160706
	void my515UpMsgThr(void);
	boost::thread *opCommThr=NULL;	//Ϊ�˿���ͨѶ�߳���ֹ
	extern boost::mutex thrhid_mutex;
	extern deque<jcLockMsg1512_t *> s_jcNotify;		//�·�����
	extern RecvMsgRotine s_CallBack;
	////�������ϴ�����ר�õı������лص�����ָ�������,���õ����ķ��������߳�;
	extern vector <RecvMsgRotine> s_vecSingleUp;	
} //namespace zwccbthr{  

namespace zwCfg {
	//const long JC_CCBDLL_TIMEOUT = 86400;	//���ʱʱ��Ϊ30��,���ڲ���Ŀ�ľ���ﵽ���Ʊ�¶����
//	const int JC_MSG_MAXLEN = 4 * 1024;	//�Ϊ128�ֽ�,���ڲ���Ŀ�ľ���ﵽ���Ʊ�¶����
	boost::mutex ComPort_mutex;	//���ڱ����������Ӷ���
	//�̶߳�����Ϊһ��ȫ�־�̬����������Ҫ��ʾ������������һ���߳�
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




extern int G_TESTCB_SUCC;	//�Ƿ�ɹ������˻ص�������һ����־λ������������


CCBELOCK_API long JCAPISTD Open(long lTimeOut)
{
	VLOG_IF(3,lTimeOut<=-600 || lTimeOut>600)<<"ZIJIN423 Open Invalid Para 20150423.1559";
	if (NULL==g_jhc)
	{
		g_jhc=new jcHidDevice();
	}

	int elockStatus=JCHID_STATUS_OK;		
	//��������̽�����
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
		LOG(WARNING)<<"û��Open��ִ��Close 20151216"<<endl;
		return ELOCK_ERROR_HARDWAREERROR;
	}

	//�����Ļ�����ʱ�ж����ݽ����߳�
	//ע�⣬��������Sleep��Ч��Ҫ����������Close֮ǰ��Sleep����
	// �ûص������յ��������֡�20151210.1706.��ΰ
	VLOG_IF(3,zwccbthr::opCommThr!=NULL)<<" ���ڹر������շ������߳�"<<endl;
	zwccbthr::opCommThr->interrupt();
	zwccbthr::opCommThr=NULL;
	Sleep(300);

	if (NULL!=g_jhc)
	{
		VLOG(3)<<__FUNCTION__<<" ���ڹرյײ�HID�豸�����Ӷ���"<<endl;
		delete g_jhc;
		g_jhc=NULL;
	}
		VLOG(3)<<"ZIJIN423 Close ELOCK_ERROR_SUCCESS";
	    return ELOCK_ERROR_SUCCESS;
}

CCBELOCK_API long JCAPISTD Notify(const char *pszMsg)
{
	VLOG(4)<<"Notify��ʼ"<<pszMsg<<endl;
		if (NULL==g_jhc)
		{
			LOG(WARNING)<<"û��Open��ִ��Notify 20151216"<<endl;
			return ELOCK_ERROR_HARDWAREERROR;
		}

	if (NULL==zwccbthr::opCommThr)
	{
		VLOG(4)<<"Start my706LockRecvThr"<<endl;
		zwccbthr::opCommThr=new boost::thread(zwccbthr::my706LockRecvThr);
	}	

	assert(pszMsg != NULL);
	assert(strlen(pszMsg) >= 42);	//XML����42�ֽ�utf8
	if (pszMsg == NULL || strlen(pszMsg) < 42) {
		ZWERROR("ZIJIN423 Notify ELOCK_ERROR_PARAMINVALID Input NULL or Not Valid XML !")
		return ELOCK_ERROR_PARAMINVALID;
	}

	string strJsonSend;
	if (NULL != pszMsg && strlen(pszMsg) > 0) {
	try {
		//������������ݣ�������󲻵ó�����λ���ڴ��С������������
		assert(NULL != pszMsg);
		if (NULL == pszMsg) {
			ZWFATAL("Notify����Ϊ��")
			    return ELOCK_ERROR_PARAMINVALID;
		}
		int inlen = strlen(pszMsg);
		assert(inlen > 0 && inlen < JC_MSG_MAXLEN);
		if (inlen == 0 || inlen >= JC_MSG_MAXLEN) {
			ZWERROR("Notify���볬�������С����");
			return ELOCK_ERROR_PARAMINVALID;
		}
		//////////////////////////////////////////////////////////////////////////
		string strXMLSend = pszMsg;
		VLOG_IF(4,strXMLSend.size()>0)<<"strXMLSend=\n"<<strXMLSend;
		//XML��ͷ�Ĺ̶�����38���ַ����������һ����ǩ�����Լ����źϼ�4���ַ�
		assert(strXMLSend.length() > 42);	
		jcAtmcConvertDLL::zwCCBxml2JCjson(strXMLSend, strJsonSend);
		assert(strJsonSend.length() > 9);	//json������ķ����������Ҫ9���ַ�����
		
		//��Ҫһ�����·���ϣ����¿��ܵĲ����̹߳������
		Sleep(100);
		boost::mutex::scoped_lock lock(zwccbthr::thrhid_mutex);


		//���ڿ�ʼһ��һ����̣��ڻ�öԿڻظ�����֮ǰ�����ϴ���������		
		DWORD iCallerThrId=GetCurrentThreadId();
		jcLockMsg1512_t *nItem=new jcLockMsg1512_t(strJsonSend);
		zwccbthr::s_jcNotify.push_back(nItem);
		VLOG_IF(3,strJsonSend.size()>0)<<"NotifyJson����="<<strJsonSend;
		return ELOCK_ERROR_SUCCESS;
	}
	catch(ptree_bad_path & e) {
		ZWFATAL(e.what());
		ZWFATAL("CCB�·�XML�д���ڵ�·��")
		    return ELOCK_ERROR_NOTSUPPORT;
	}
	catch(ptree_bad_data & e) {
		ZWFATAL(e.what());
		ZWFATAL("CCB�·�XML�д�����������")
		    return ELOCK_ERROR_PARAMINVALID;
	}
	catch(ptree_error & e) {
		ZWFATAL(e.what());
		ZWFATAL("CCB�·�XML������δ֪����")
		    return ELOCK_ERROR_CONNECTLOST;
	}
	catch(...) {		//һ�������쳣��ֱ�ӷ��ش�����Ҫ��Ϊ�˲�׽δ����ʱ
		//���ڶ���Ϊ����ɷ���NULLָ��ĵ�SEH�쳣  
		//Ϊ��ʹ�õײ�Poco����cceblock�������ҵ�WS��
		//����WS������Ϊδ���Ӷ���NULLʱֱ��throwһ��ö��
		//Ȼ���ڴˣ�Ҳ�����ϲ㲶����ʱ��֪������ȷ����
		//����catch�����쳣��
		ZWFATAL(__FUNCTION__);
		ZWFATAL("Notifyͨ����·���������쳣�������������")
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
		ZWFATAL("ע��ص��������ܴ����ָ��0952")
		    return ELOCK_ERROR_PARAMINVALID;
	}
	DWORD iCallerThrId=GetCurrentThreadId();
	//�ѻص�����ָ�뱣�浽�߳�IDΪ������MAP����
	//zwccbthr::s_thrIdToPointer[iCallerThrId]=pRecvMsgFun;
	//�ѻص�����ָ�뱣�浽һ���������湩�����ϴ������ṩ�������̵߳Ĳ��ظ��ص�����
	zwccbthr::s_vecSingleUp.push_back(pRecvMsgFun);
	zwccbthr::s_CallBack=pRecvMsgFun;
	VLOG(3)<<"zwccbthr::s_vecSingleUp.size()="<<zwccbthr::s_vecSingleUp.size()<<endl;
	VLOG(3)<<"��������߳�ID="<<iCallerThrId<<"\t�ص�������ַ="<<std::hex<<pRecvMsgFun<<endl;
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
				"��������ʧ�� 20160706.1656 by Class jcHidDevice");
			m_hidOpened=false;
			return ELOCK_ERROR_PARAMINVALID;
		}
		//hid_set_nonblocking(static_cast<hid_device *>(m_jcElock.hid_device),1);
		ZWWARN("myOpenElock1503 �������򿪳ɹ�20160706.1656 by Class jcHidDevice")
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
			//Ҫ������Open/Close�Ļ����Ͳ����ڴ˰����ݽṹ����
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

		//OutputDebugStringA("415����һ�����߷�����Ϣ��ʼ\n");
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
	ZWWARN("myOpenElock1607 ģ��������򿪳ɹ�20160705.1003")
			m_hidOpened=true;
		return ELOCK_ERROR_SUCCESS;
}

void jcHidDevice::CloseJc()
{
	ZWWARN("myOpenElock1607 ģ��������رճɹ�20160705.1003")
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
	//OutputDebugStringA("415����һ�����߷�����Ϣ��ʼ\n");
	int outLen=0;



	static bool mainMsgExeced=false;

	static bool msg1002Send=false;
	//��һ�����ʲ��뵥�����б���1002Ҳ���Ƿ�����֤��
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
		printf("ģ�����߽��ջ�����̫С");
		strcpy(recvJson,"bufTooSmall");
		return JCHID_STATUS_FAIL;
	}
}
#endif // _USE_JCHID_DEV20160705


}	//namespace jchidDevice2015{



