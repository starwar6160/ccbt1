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
#include "zwPocoLog.h"
using namespace std;
using boost::property_tree::ptree_error;
using boost::property_tree::ptree_bad_data;
using boost::property_tree::ptree_bad_path;

namespace zwccbthr {
	void ThreadLockComm();	//������֮���ͨѶ�߳�
	string zwGetLockIP(void);
	extern std::deque < string > dqOutXML;;
	extern boost::mutex recv_mutex;
	extern JCHID hidHandle;
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
	OutputDebugStringA(x);
	pocoLog->trace(x);
}

zw_trace::zw_trace(const char *funcName)
{
	m_str = funcName;
	m_start = m_str + "\tSTART";
	m_end = m_str + "\tEND";
	OutputDebugStringA(m_start.c_str());
	pocoLog->trace(m_start);
}

zw_trace::~zw_trace()
{

	OutputDebugStringA(m_end.c_str());
	pocoLog->trace(m_end);
}




CCBELOCK_API long JCAPISTD Open(long lTimeOut)
{
	char buf[256];
	memset(buf, 0, 256);
	sprintf(buf, "Open incoming timeout value seconds is %d", lTimeOut);
	OutputDebugStringA(buf);
	assert(lTimeOut >= -1 && lTimeOut <= JC_CCBDLL_TIMEOUT);
	if (lTimeOut < -1 || lTimeOut > JC_CCBDLL_TIMEOUT) {
		memset(buf, 0, 256);
		sprintf(buf,
			"Open Function incoming timeout Value must in -1 to %d seconds",
			JC_CCBDLL_TIMEOUT);
		OutputDebugStringA(buf);
		pocoLog->
		    error() <<
		    "Open Function incoming timeout Value must in -1 to " <<
		    JC_CCBDLL_TIMEOUT << "seconds";
		return ELOCK_ERROR_PARAMINVALID;
	}
	ZWFUNCTRACE boost::mutex::scoped_lock lock(zwCfg::ComPort_mutex);
	//�������0��С��JC_CCBDLL_TIMEOUT��������һ������Χ��
	pocoLog->notice() << "Open Return " << ELOCK_ERROR_SUCCESS << endl;
	string myLockIp;
	try {

#ifdef ZWUSE_HID_MSG_SPLIT
		if (true == zwCfg::s_hidOpened) {
			ZWNOTICE("s_hidOpened already Opened,so return directly.")
			return ELOCK_ERROR_SUCCESS;
		}
		memset(&zwccbthr::hidHandle, 0, sizeof(JCHID));
		zwccbthr::hidHandle.vid = JCHID_VID_2014;
		zwccbthr::hidHandle.pid = JCHID_PID_LOCK5151;
		if (JCHID_STATUS_OK != jcHidOpen(&zwccbthr::hidHandle)) {
			ZWERROR("HID Device Open ERROR 1225 !");
			return ELOCK_ERROR_PARAMINVALID;
		}
		zwCfg::s_hidOpened = true;
#endif // ZWUSE_HID_MSG_SPLIT
		//����ͨ���߳�
		boost::thread thr(zwccbthr::ThreadLockComm);
	}
	catch(...) {
		string errMsg = "�򿪶˿�" + myLockIp + "ʧ��";
		ZWFATAL(errMsg.c_str())
	}

	ZWNOTICE("�ɹ��� �����ߵ�����")
	    return ELOCK_ERROR_SUCCESS;
}

CCBELOCK_API long JCAPISTD Close()
{
	ZWFUNCTRACE boost::mutex::scoped_lock lock(zwCfg::ComPort_mutex);
	zwCfg::g_WarnCallback = NULL;
#ifdef ZWUSE_HID_MSG_SPLIT
	goto CloseHidEnd;
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
      CloseHidEnd:
	zwCfg::s_hidOpened = false;
	ZWNOTICE("�ر� �����ߵ�����")
	    return ELOCK_ERROR_SUCCESS;
}

CCBELOCK_API long JCAPISTD Notify(const char *pszMsg)
{
	//ͨ����Notify������ʼ����Ƿ�˿��Ѿ��򿪣�û�д򿪾�ֱ�ӷ��أ�����
	//2014��11�³��ڹ���������û����������ʱ��ATMCִ��0002���Ĳ�ѯ����״̬��
	//������ѯ������������־�����������
	if (false == zwCfg::s_hidOpened) {
		return ELOCK_ERROR_CONNECTLOST;
	}
	ZWFUNCTRACE assert(pszMsg != NULL);
	assert(strlen(pszMsg) >= 42);	//XML����42�ֽ�utf8
	if (pszMsg == NULL || strlen(pszMsg) < 42) {
		return ELOCK_ERROR_PARAMINVALID;
	}
	if (NULL != pszMsg && strlen(pszMsg) > 0) {
		pocoLog->information() << "CCB�·�XML=" << endl << pszMsg <<
		    endl;
	}
	boost::mutex::scoped_lock lock(zwCfg::ComPort_mutex);
	string strJsonSend;
	try {
		//������������ݣ�������󲻵ó�����λ���ڴ��С������������
		assert(NULL != pszMsg);
		if (NULL == pszMsg) {
			ZWERROR("Notify����Ϊ��")
			    return ELOCK_ERROR_PARAMINVALID;
		}
		int inlen = strlen(pszMsg);
		assert(inlen > 0 && inlen < JC_MSG_MAXLEN);
		if (inlen == 0 || inlen >= JC_MSG_MAXLEN) {
			ZWWARN("Notify���볬�������С����");
			return ELOCK_ERROR_PARAMINVALID;
		}
		//////////////////////////////////////////////////////////////////////////
		string strXMLSend = pszMsg;
		assert(strXMLSend.length() > 42);	//XML��ͷ�Ĺ̶�����38���ַ����������һ����ǩ�����Լ����źϼ�4���ַ�
		jcAtmcConvertDLL::zwCCBxml2JCjson(strXMLSend, strJsonSend);
		assert(strJsonSend.length() > 9);	//json������ķ����������Ҫ9���ַ�����
		ZWNOTICE(strJsonSend.c_str());
		Sleep(50);
		zwPushString(strJsonSend.c_str());
		return ELOCK_ERROR_SUCCESS;
	}
	catch(ptree_bad_path & e) {
		ZWERROR(e.what());
		ZWERROR("CCB�·�XML�д���ڵ�·��")
		    return ELOCK_ERROR_NOTSUPPORT;
	}
	catch(ptree_bad_data & e) {
		ZWERROR(e.what());
		ZWERROR("CCB�·�XML�д�����������")
		    return ELOCK_ERROR_PARAMINVALID;
	}
	catch(ptree_error & e) {
		ZWERROR(e.what());
		ZWERROR("CCB�·�XML������δ֪����")
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

void cdecl myATMCRecvMsgRotine(const char *pszMsg)
{
	ZWFUNCTRACE assert(pszMsg != NULL && strlen(pszMsg) > 42);
	boost::mutex::scoped_lock lock(zwCfg::ComPort_mutex);
	//������������ݣ�������󲻵ó�����λ���ڴ��С������������
	assert(NULL != pszMsg);
	int inlen = strlen(pszMsg);
	assert(inlen > 0 && inlen < JC_MSG_MAXLEN);
	if (NULL == pszMsg || inlen == 0 || inlen >= JC_MSG_MAXLEN) {
		return;
	}
}

CCBELOCK_API int JCAPISTD SetRecvMsgRotine(RecvMsgRotine pRecvMsgFun)
{
	ZWFUNCTRACE boost::mutex::scoped_lock lock(zwCfg::ComPort_mutex);
	assert(NULL != pRecvMsgFun);
	if (NULL == pRecvMsgFun) {
		ZWFATAL("ע��ص��������ܴ����ָ��0952")
		    return ELOCK_ERROR_PARAMINVALID;
	}
	zwCfg::g_WarnCallback = pRecvMsgFun;
	return ELOCK_ERROR_SUCCESS;
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
