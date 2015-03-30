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
	boost::thread *opCommThr=NULL;	//Ϊ�˿���ͨѶ�߳���ֹ
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
		ZWNOTICE("�ر��ϴ�û�йرյ����ݽ����߳�")
		Close();//����ϴ�û��Close��ֱ���ٴ�Open��������Closeһ�Σ�
	}
#ifdef _DEBUG327
//��Ȼ��ʱ���ó�ʱ���������ݲ������
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
		ZWINFO("return ELOCK_ERROR_PARAMINVALID ��ڲ����Ƿ�")
		return ELOCK_ERROR_PARAMINVALID;
	}
#endif // _DEBUG327
	ZWFUNCTRACE boost::mutex::scoped_lock lock(zwCfg::ComPort_mutex);
	//�������0��С��JC_CCBDLL_TIMEOUT��������һ������Χ��
	//LOG(WARNING) << "Open Return " << ELOCK_ERROR_SUCCESS << endl;
	string myLockIp;
	try {

#ifdef ZWUSE_HID_MSG_SPLIT
		//if (true == zwCfg::s_hidOpened) {
		//	ZWNOTICE("return ELOCK_ERROR_SUCCESS �Ѿ��򿪵������������ظ���.")
		//	return ELOCK_ERROR_SUCCESS;
		//}
		memset(&zwccbthr::hidHandle, 0, sizeof(JCHID));
		zwccbthr::hidHandle.vid = JCHID_VID_2014;
		zwccbthr::hidHandle.pid = JCHID_PID_LOCK5151;
		if (JCHID_STATUS_OK != jcHidOpen(&zwccbthr::hidHandle)) {
			ZWERROR("return ELOCK_ERROR_PARAMINVALID ��������ʧ��");
			return ELOCK_ERROR_PARAMINVALID;
		}
		zwCfg::s_hidOpened = true;
#endif // ZWUSE_HID_MSG_SPLIT
		//����ͨ���߳�
		//boost::thread thr(zwccbthr::ThreadLockComm);
		zwccbthr::opCommThr=new boost::thread(zwccbthr::ThreadLockComm);
	}
	catch(...) {
		string errMsg = "�򿪶˿�" + myLockIp + "ʧ��";
		ZWFATAL(errMsg.c_str())
	}

	ZWNOTICE("return ELOCK_ERROR_SUCCESS �ɹ��򿪵�����")
	    return ELOCK_ERROR_SUCCESS;
}

CCBELOCK_API long JCAPISTD Close()
{
	ZWFUNCTRACE boost::mutex::scoped_lock lock(zwCfg::ComPort_mutex);
	//zwCfg::g_WarnCallback = NULL;
	//�رղ���Ҫ�㣺���ж����ݽ����̣߳�Ȼ��join�ȴ����ж���ɣ�
	// Ȼ���̶߳���ָ����λNULL,�´ξͿ��Գɹ�����
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
		LOG(INFO) << "CCB�·�XML=" << endl << pszMsg <<
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
	JC_CCBELOCK_ERROR_CODE m_status;	//����ֵ
	boost::thread *m_opCommThr;	
	boost::mutex m_hidMutex;	//���ڱ����������Ӷ���
	boost::mutex thr_mutex;	//����ͨ���߳�ֻ����һ��
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
		ZWERROR("return ELOCK_ERROR_PARAMINVALID ��������ʧ��");
		m_status=ELOCK_ERROR_PARAMINVALID;
		return ;
	}
	m_hidOpened = true;
	//����ͨ���߳�
	//zwccbthr::opCommThr=new boost::thread(zwccbthr::ThreadLockComm);
	m_opCommThr=new boost::thread(&ThreadLockComm330);
	StartRecvThread();
	ZWNOTICE("return ELOCK_ERROR_SUCCESS �ɹ��򿪵�����")
		m_status=ELOCK_ERROR_SUCCESS;
	return ;

}

zwJcElock1503::~zwJcElock1503()
{
	boost::mutex::scoped_lock lock(m_hidMutex);
	//�رղ���Ҫ�㣺���ж����ݽ����̣߳�Ȼ��join�ȴ����ж���ɣ�
	// Ȼ���̶߳���ָ����λNULL,�´ξͿ��Գɹ�����
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
	ZWNOTICE("�ر� �����ߵ�����")
	m_status=ELOCK_ERROR_SUCCESS;

}



void zwJcElock1503::StartRecvThread()
{
	assert(NULL!=m_hidHandle.hid_device);
	if (NULL==m_hidHandle.hid_device)
	{
		LOG(ERROR)<<"jcHidDevice Not Open,cant't Start RecvThread"<<endl;
	}
	//����һ���������󣬼������ڲ���ǰ���Ǻ�������ֵ�������ڲ��Ǻ�����һ�����߶������(�β�)�������Ƕ��ŷָ���
	//������boost::bind�������¸�ʽ�Ѻ���ָ��ͺ���_1��ʽ��һ�����߶������(�β�)�󶨳�Ϊһ����������
	//boost::function<int (JCHID *)> memberFunctionWrapper(boost::bind(&zwJcElock1503::ThreadLockComm330, this,_1));  	
	//�ٴ�ʹ��boost::bind�Ѻ���������ʵ�ΰ󶨵�һ�𣬾Ϳ��Դ��ݸ�boost::thread��Ϊ�߳��庯����
	//m_opCommThr=new boost::thread(boost::bind(memberFunctionWrapper,&m_hidHandle));	
	Sleep(5);	//�ȴ��߳�������ϣ���ʵҲ��2����һ��������ˣ�
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
		ZWFATAL("ע��ص��������ܴ����ָ��0952")
			return ELOCK_ERROR_PARAMINVALID;
	}
	m_WarnCallback = pRecvMsgFun;
	m_status=ELOCK_ERROR_SUCCESS;
	return m_status;
}

int zwJcElock1503::Notify(const char *pszMsg)
{
	boost::mutex::scoped_lock lock(m_hidMutex);
	//ͨ����Notify������ʼ����Ƿ�˿��Ѿ��򿪣�û�д򿪾�ֱ�ӷ��أ�����
	//2014��11�³��ڹ���������û����������ʱ��ATMCִ��0002���Ĳ�ѯ����״̬��
	//������ѯ������������־�����������
	if (false == m_hidOpened) {
		return ELOCK_ERROR_CONNECTLOST;
	}
	ZWFUNCTRACE assert(pszMsg != NULL);
	assert(strlen(pszMsg) >= 42);	//XML����42�ֽ�utf8
	if (pszMsg == NULL || strlen(pszMsg) < 42) {
		return ELOCK_ERROR_PARAMINVALID;
	}
	if (NULL != pszMsg && strlen(pszMsg) > 0) {
		LOG(INFO) << "CCB�·�XML=" << endl << pszMsg <<
			endl;
	}

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
			ZWINFO("�������߳ɹ�");
			try {
				boost::this_thread::interruption_point();
				JCHID_STATUS sts=
					jcHidRecvData(&g_jcElock.m_hidHandle,
					recvBuf, BLEN, &outLen,JCHID_RECV_TIMEOUT);
				//zwCfg::s_hidOpened=true;	//����ͨ���̵߳�һ��������־					
				//Ҫ��ʲôҲû�յ�����ֱ�ӽ�����һ��ѭ��
				if (JCHID_STATUS_OK!=sts)
				{
					printf("JCHID_STATUS 1225 %d\n",sts);
					Sleep(900);
					continue;
				}
				printf("\n");
				ZWNOTICE("wkThr�ɹ������߽����������£�");
			}
			catch(boost::thread_interrupted &e)
			{
				ZWERROR
					("RecvData�ӵ�������������ʱ�������߳��յ���ֹ�źţ����ݽ����߳̽���ֹ");
				return;
			}
			catch(...) {
				ZWERROR
					("RecvData�ӵ�������������ʱ��������·�������δ֪�������ݽ����߳̽���ֹ");
				return;
			}
			ZWNOTICE(recvBuf);

			boost::this_thread::interruption_point();
			string outXML;
			jcAtmcConvertDLL::zwJCjson2CCBxml(recvBuf,
				outXML);
			ZWINFO("�������߻ش���Json��ת��Ϊ����XML�ɹ�");
			//XML��ͷ�Ĺ̶�����38���ַ����������һ����ǩ�����Լ����źϼ�4���ַ�
			assert(outXML.length() > 42);
			ZWDBGMSG(outXML.c_str());
			{
				boost::
					mutex::scoped_lock lock(g_jcElock.recv_mutex);
				//�յ���XML�������
				g_jcElock.dqOutXML.push_back(outXML);
			}

			if (NULL != g_jcElock.m_WarnCallback) {
				//���ûص�����������Ϣ��Ȼ��͹ر����ӣ�����ͨ���̣߳�
				g_jcElock.m_WarnCallback(outXML.c_str());
				ZWINFO
					("�ɹ��Ѵ����߽��յ������ݴ��ݸ��ص�����");
			} else {
				ZWWARN
					("�ص�����ָ��Ϊ�գ��޷����ûص�����")
			}
		}
		ZWINFO("��ͨ�����ݽ����߳������˳�");

	}		//try
	catch(...) {			
		//�쳣�Ͽ����趨�ñ�־ΪFALSE,�Ա��´�Open��Ҫ����������ͨ���̵߳ĳ����
		g_jcElock.m_hidOpened=false;
		ZWFATAL
			("��ͨ�����ݽ����߳����������쳣�Ͽ����������ݽ����߳̽�����");
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
