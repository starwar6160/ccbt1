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
#include "zwHidDevClass2015.h"
#include "hidapi.h"

using namespace std;
using boost::property_tree::ptree_error;
using boost::property_tree::ptree_bad_data;
using boost::property_tree::ptree_bad_path;
using jchidDevice2015::jcHidDevice;

extern jcHidDevice *g_jhc;	//ʵ�ʵ�HID�豸�����


namespace zwccbthr {
	void ThreadLockRecv();	//������֮���ͨѶ�߳�
	void my515LockRecvThr();	//������֮���ͨѶ�߳�20150515
	void my515UpMsgThr(void);
	boost::thread *opCommThr=NULL;	//Ϊ�˿���ͨѶ�߳���ֹ
	boost::thread *opUpMsgThr=NULL;	
	//boost::thread *opCommThr=new boost::thread(zwccbthr::ThreadLockRecv);
	time_t lastOpen=0;
	extern boost::mutex thrhid_mutex;
	extern bool myWaittingReturnMsg;	//�ȴ����ر����ڼ䲻Ҫ�·�����
	extern boost::timer g_LatTimer;	//�����Զ������ӳ�
	extern boost::condition_variable condJcLock;
	extern deque<string> s_jcNotify;		//�·�����
	//extern string s_jsonCmd;
} //namespace zwccbthr{  

namespace zwCfg {
	//const long JC_CCBDLL_TIMEOUT = 86400;	//���ʱʱ��Ϊ30��,���ڲ���Ŀ�ľ���ﵽ���Ʊ�¶����
//	const int JC_MSG_MAXLEN = 4 * 1024;	//�Ϊ128�ֽ�,���ڲ���Ŀ�ľ���ﵽ���Ʊ�¶����
	//����һ���ص�����ָ��
	RecvMsgRotine g_WarnCallback = NULL;
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
	OutputDebugStringA(m_start.c_str());
	VLOG(4)<<m_start;
}

zw_trace::~zw_trace()
{

	OutputDebugStringA(m_end.c_str());	
	VLOG(4)<<m_end;
}




extern int G_TESTCB_SUCC;	//�Ƿ�ɹ������˻ص�������һ����־λ������������


CCBELOCK_API long JCAPISTD Open(long lTimeOut)
{
	VLOG_IF(3,lTimeOut<=0 || lTimeOut>60)<<"ZIJIN423 Open Invalid Para 20150423.1559";
	if (NULL==g_jhc)
	{
		g_jhc=new jcHidDevice();
	}

	int elockStatus=JCHID_STATUS_OK;		
	//��������̽�����
	elockStatus=g_jhc->SendJson("{   \"command\": \"Lock_Firmware_Version\",    \"State\": \"get\"}");
	VLOG_IF(1,JCHID_STATUS_OK!=elockStatus)<<"ZIJIN522 Open ELOCK_ERROR_CONNECTLOST Send get_firmware_version to JinChu Elock Fail!";
	if (JCHID_STATUS_OK!=static_cast<JCHID_STATUS>(elockStatus))
	{
		g_jhc->CloseJc();
		elockStatus=g_jhc->OpenJc();
		VLOG_IF(1,JCHID_STATUS_OK!=elockStatus)<<"ZIJIN522 Open ELOCK_ERROR_CONNECTLOST Send get_firmware_version to JinChu Elock Fail on retry 2!";
	}

	if (NULL==zwccbthr::opUpMsgThr)
	{
		zwccbthr::opUpMsgThr=new boost::thread(zwccbthr::my515UpMsgThr);
	}	

	if (NULL==zwccbthr::opCommThr)
	{
		zwccbthr::opCommThr=new boost::thread(zwccbthr::my515LockRecvThr);
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
	VLOG(2)<<"ZIJIN423 Close ELOCK_ERROR_SUCCESS";
	zwccbthr::opCommThr=NULL;
	if (NULL!=g_jhc)
	{
		delete g_jhc;
		g_jhc=NULL;
	}
	    return ELOCK_ERROR_SUCCESS;
}

CCBELOCK_API long JCAPISTD Notify(const char *pszMsg)
{
	VLOG(4)<<__FUNCTION__<<" scoped_lock lock(thrhid_mutex) START"<<endl;
	boost::mutex::scoped_lock lock(zwccbthr::thrhid_mutex);
	VLOG(4)<<__FUNCTION__<<"condJcLock.wait(lock);"<<endl;
	//zwccbthr::condJcLock.wait(lock);							
	//LOG(ERROR)<<__FUNCTION__<<"RUNNING " <<time(NULL)<<endl;
	//ͨ����Notify������ʼ����Ƿ�˿��Ѿ��򿪣�û�д򿪾͵ȴ�һ��ʱ�䣬����
	//2014��11�³��ڹ���������û����������ʱ��ATMCִ��0002���Ĳ�ѯ����״̬��
	//������ѯ������������־�����������	
	//if (time(NULL)-zwccbthr::lastOpen>20)
	{
		//Open(1);
	}
	//LOG(INFO)<<"Notify��ʼ####################\n";

	//ZWFUNCTRACE 
	assert(pszMsg != NULL);
	assert(strlen(pszMsg) >= 42);	//XML����42�ֽ�utf8
	if (pszMsg == NULL || strlen(pszMsg) < 42) {
		ZWERROR("ZIJIN423 Notify ELOCK_ERROR_PARAMINVALID Input NULL or Not Valid XML !")
		return ELOCK_ERROR_PARAMINVALID;
	}
	if (NULL != pszMsg && strlen(pszMsg) > 0) {
	}

	string strJsonSend;
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
		assert(strXMLSend.length() > 42);	//XML��ͷ�Ĺ̶�����38���ַ����������һ����ǩ�����Լ����źϼ�4���ַ�
		jcAtmcConvertDLL::zwCCBxml2JCjson(strXMLSend, strJsonSend);
		assert(strJsonSend.length() > 9);	//json������ķ����������Ҫ9���ַ�����
		VLOG_IF(4,strJsonSend.size()>0)<<"strJsonSend="<<strJsonSend;
		Sleep(50);			
		VLOG(3)<<__FUNCTION__<<"\tSleep 50 ms"<<endl;

		//���ڿ�ʼһ��һ����̣��ڻ�öԿڻظ�����֮ǰ�����ϴ���������
		
		zwccbthr::s_jcNotify.push_back(strJsonSend);
		//int sts=g_jhc->SendJson(strJsonSend.c_str());
		//VLOG_IF(1,JCHID_STATUS_OK!=sts)<<"423�·���Ϣ�������쳣\n";
		//zwccbthr::s_jsonCmd=strJsonSend;
		VLOG(4)<<"condJcLock.notify_all();"<<endl;
		 //zwccbthr::condJcLock.notify_all();	
//////////////////////////////////////////////////////////////////////////
		//const int BLEN = 1024;
		//char recvBuf[BLEN + 1];			
		//memset(recvBuf, 0, BLEN + 1);
		//sts=g_jhc->RecvJson(recvBuf,BLEN);
		//if (strlen(recvBuf)>0)
		//{
		//	ZWWARN(recvBuf)
		//}
//////////////////////////////////////////////////////////////////////////
		VLOG(3)<<__FUNCTION__<<" scoped_lock lock(thrhid_mutex) END"<<endl;
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
	
}

CCBELOCK_API int JCAPISTD SetRecvMsgRotine(RecvMsgRotine pRecvMsgFun)
{
	G_TESTCB_SUCC=0;
	//ZWFUNCTRACE 
	//boost::mutex::scoped_lock lock(zwCfg::ComPort_mutex);
	assert(NULL != pRecvMsgFun);
	if (NULL == pRecvMsgFun) {
		ZWFATAL("ע��ص��������ܴ����ָ��0952")
		    return ELOCK_ERROR_PARAMINVALID;
	}
	zwCfg::g_WarnCallback = pRecvMsgFun;
	return ELOCK_ERROR_SUCCESS;
}


//////////////////////////////////////////////////////////////////////////
namespace jchidDevice2015{
	jcHidDevice::jcHidDevice()
	{
		ZWFUNCTRACE
		memset(&m_jcElock, 0, sizeof(JCHID));
		m_jcElock.vid = JCHID_VID_2014;
		m_jcElock.pid = JCHID_PID_LOCK5151;
		m_hidOpened=false;
		OpenJc();
	}


	int jcHidDevice::OpenJc()
	{
		ZWFUNCTRACE		
		boost::mutex::scoped_lock lock(m_jchid_mutex);
		ZWWARN(__FUNCTION__)
		if (JCHID_STATUS_OK != jcHidOpen(&m_jcElock)) {
			ZWERROR("myOpenElock1503 return ELOCK_ERROR_PARAMINVALID "
				"��������ʧ�� 20150504.0957 by Class jcHidDevice");
			m_hidOpened=false;
			return ELOCK_ERROR_PARAMINVALID;
		}
		//hid_set_nonblocking(static_cast<hid_device *>(m_jcElock.hid_device),1);
		ZWWARN("myOpenElock1503 �������򿪳ɹ�20150504.0957 by Class jcHidDevice")
		m_hidOpened=true;
		return ELOCK_ERROR_SUCCESS;
	}

	void jcHidDevice::CloseJc()
	{
		ZWFUNCTRACE
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
		ZWFUNCTRACE
		CloseJc();
	}

	int jcHidDevice::getConnectStatus()
	{
		boost::mutex::scoped_lock lock(m_jchid_mutex);
		const char *m_cmdGetFirmware=
			"{   \"command\": \"Lock_Firmware_Version\",    \"State\": \"get\"}";
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
		ZWFUNCTRACE
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
		sts=jcHidRecvData(&m_jcElock,recvJson, bufLen, &outLen,0);
			
		return sts;
	}


}	//namespace jchidDevice2015{



