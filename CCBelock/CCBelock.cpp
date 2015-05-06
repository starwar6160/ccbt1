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
#include "zwPocoLog.h"

using namespace std;
using boost::property_tree::ptree_error;
using boost::property_tree::ptree_bad_data;
using boost::property_tree::ptree_bad_path;
using jchidDevice2015::jcHidDevice;

extern jcHidDevice g_jhc;	//ʵ�ʵ�HID�豸����󣬹���ʱ�Զ�����


namespace zwccbthr {
	void ThreadLockComm();	//������֮���ͨѶ�߳�
	//boost::thread *opCommThr=NULL;	//Ϊ�˿���ͨѶ�߳���ֹ
	boost::thread *opCommThr=new boost::thread(zwccbthr::ThreadLockComm);
	time_t lastOpen=0;
	extern boost::mutex thrhid_mutex;
	extern string s_jsonCmd;
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
	LOG(INFO)<<m_start;
}

zw_trace::~zw_trace()
{

	OutputDebugStringA(m_end.c_str());	
	LOG(INFO)<<m_end;
}




extern int G_TESTCB_SUCC;	//�Ƿ�ɹ������˻ص�������һ����־λ������������


CCBELOCK_API long JCAPISTD Open(long lTimeOut)
{
	VLOG_IF(1,lTimeOut<=0 || lTimeOut>3)<<"ZIJIN423 Open Invalid Para 20150423.1559";
	int elockStatus=g_jhc.SendJson("{   \"command\": \"Lock_Firmware_Version\",    \"State\": \"get\"}");
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
	VLOG(2)<<"ZIJIN423 Close ELOCK_ERROR_SUCCESS";
	    return ELOCK_ERROR_SUCCESS;
}

CCBELOCK_API long JCAPISTD Notify(const char *pszMsg)
{
	boost::mutex::scoped_lock lock(zwccbthr::thrhid_mutex);
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
		VLOG_IF(1,strXMLSend.size()>0)<<"strXMLSend=\n"<<strXMLSend;
		assert(strXMLSend.length() > 42);	//XML��ͷ�Ĺ̶�����38���ַ����������һ����ǩ�����Լ����źϼ�4���ַ�
		jcAtmcConvertDLL::zwCCBxml2JCjson(strXMLSend, strJsonSend);
		assert(strJsonSend.length() > 9);	//json������ķ����������Ҫ9���ַ�����
		VLOG_IF(1,strJsonSend.size()>0)<<"strJsonSend="<<strJsonSend;
		Sleep(50);			

		//int sts=g_jhc.SendJson(strJsonSend.c_str());
		//VLOG_IF(1,JCHID_STATUS_OK!=sts)<<"423�·���Ϣ�������쳣\n";
		zwccbthr::s_jsonCmd=strJsonSend;
//////////////////////////////////////////////////////////////////////////
		//const int BLEN = 1024;
		//char recvBuf[BLEN + 1];			
		//memset(recvBuf, 0, BLEN + 1);
		//sts=g_jhc.RecvJson(recvBuf,BLEN);
		//if (strlen(recvBuf)>0)
		//{
		//	ZWWARN(recvBuf)
		//}
//////////////////////////////////////////////////////////////////////////
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


void cdecl myATMCRecvMsgRotine(const char *pszMsg)
{
	//ZWFUNCTRACE 
	//assert(pszMsg != NULL && strlen(pszMsg) > 42);
	//boost::mutex::scoped_lock lock(zwCfg::ComPort_mutex);
	//������������ݣ�������󲻵ó�����λ���ڴ��С������������
	assert(NULL != pszMsg);
	int inlen = strlen(pszMsg);
	if (0==inlen)
	{
		ZWERROR("Callback Function myATMCRecvMsgRotine input is NULL")
	}
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
		//boost::mutex::scoped_lock lock(zwccbthr::recv_mutex);
		G_TESTCB_SUCC=1;	//�ɹ������˻ص�����
		//printf("%s\n",pszMsg);
	}	
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
		if (JCHID_STATUS_OK != jcHidOpen(&m_jcElock)) {
			ZWERROR("myOpenElock1503 return ELOCK_ERROR_PARAMINVALID "
				"��������ʧ�� 20150504.0957 by Class jcHidDevice");
			m_hidOpened=false;
			return ELOCK_ERROR_PARAMINVALID;
		}
		ZWINFO("myOpenElock1503 �������򿪳ɹ�20150504.0957 by Class jcHidDevice")
			m_hidOpened=true;
		return ELOCK_ERROR_SUCCESS;
	}

	void jcHidDevice::CloseJc()
	{
		ZWFUNCTRACE
		boost::mutex::scoped_lock lock(m_jchid_mutex);
		if (NULL!=m_jcElock.hid_device)
		{
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

	JCHID_STATUS jcHidDevice::RecvJson( char *recvJson,int bufLen )
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
		JCHID_STATUS sts=jcHidRecvData(&m_jcElock,
			recvJson, bufLen, &outLen,4000);
		VLOG_IF(4,JCHID_STATUS_OK!=sts)<<"jcHidDevice::RecvJson FAIL\n";
		return sts;
	}


}	//namespace jchidDevice2015{



void zwtest504hidClass(void)
{
	const char *msg02="{\"Command\":\"Lock_Now_Info\"}";
	jcHidDevice *jc1=new jcHidDevice();	
	printf("%s\n",__FUNCTION__);
	jc1->SendJson(	msg02);
	jc1->SendJson(	msg02);
	char recvJson[256];

	memset(recvJson,0,256);
	jc1->RecvJson(recvJson,256);
	ZWWARN(recvJson)

	jc1->CloseJc();
	jc1->OpenJc();

	Sleep(3000);
	memset(recvJson,0,256);
	jc1->RecvJson(recvJson,256);
	ZWWARN(recvJson)
	jc1->CloseJc();

	jcHidDevice *jc2=new jcHidDevice();
	memset(recvJson,0,256);
	jc2->SendJson(	msg02);
	jc2->RecvJson(recvJson,256);
	ZWWARN(recvJson)
}