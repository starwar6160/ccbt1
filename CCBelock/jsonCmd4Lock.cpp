#include "stdafx.h"
#include "zwCcbElockHdr.h"
#include "zwPocoLog.h"
#include "zwHidComm.h"
#include "CCBelock.h"
using namespace boost::property_tree;
using Poco::AutoPtr;
using Poco::Util::IniFileConfiguration;
using boost::mutex;
namespace zwccbthr{
	extern boost::mutex thr_mutex;
	extern JCHID hidHandle;
	extern boost::mutex recv_mutex;
	extern std::deque < string > dqOutXML;
}


namespace zwCfg {
	//extern const long JC_CCBDLL_TIMEOUT;	//���ʱʱ��Ϊ30��,���ڲ���Ŀ�ľ���ﵽ���Ʊ�¶����
	//����һ���ص�����ָ��
	extern RecvMsgRotine g_WarnCallback;
	extern boost::mutex ComPort_mutex;	//���ڱ����������Ӷ���
	//�̶߳�����Ϊһ��ȫ�־�̬����������Ҫ��ʾ������������һ���߳�
	extern boost::thread * thr;
	extern bool s_hidOpened;
} //namespace zwCfg{  

namespace jcLockJsonCmd_t2015a{

	CCBELOCK_API long OpenJson(long lTimeOut)
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
#else
			//�򿪴���
			myLockIp = zwccbthr::zwGetLockIP();
			zwccbthr::zwComPort = new jcSerialPort(myLockIp.c_str());
#endif // ZWUSE_HID_MSG_SPLIT			
		}
		catch(...) {
			string errMsg = "�򿪶˿�" + myLockIp + "ʧ��";
			ZWFATAL(errMsg.c_str())
		}

		ZWNOTICE("�ɹ��� �����ߵ�����")
			return ELOCK_ERROR_SUCCESS;
	}


	//��long Notify(const char *pszMsg)�޸Ķ���
	CCBELOCK_API long jcSendJson2Lock(const char *pszJson)
	{
		//ͨ����Notify������ʼ����Ƿ�˿��Ѿ��򿪣�û�д򿪾�ֱ�ӷ��أ�����
		//2014��11�³��ڹ���������û����������ʱ��ATMCִ��0002���Ĳ�ѯ����״̬��
		//������ѯ������������־�����������
		if (false == zwCfg::s_hidOpened) {
			return ELOCK_ERROR_CONNECTLOST;
		}
		ZWFUNCTRACE 
		//������������ݣ�������󲻵ó�����λ���ڴ��С������������
		assert(NULL != pszJson);
		if (NULL == pszJson) {
			ZWERROR("Notify����Ϊ��")
				return ELOCK_ERROR_PARAMINVALID;
		}
		if (NULL != pszJson && strlen(pszJson) > 0) {
			pocoLog->information() << "JinChu�·�JSON=" << endl << pszJson <<
				endl;
		}
		boost::mutex::scoped_lock lock(zwCfg::ComPort_mutex);
		
		try {
			int inlen = strlen(pszJson);
			assert(inlen > 0 && inlen < JC_MSG_MAXLEN);
			if (inlen == 0 || inlen >= JC_MSG_MAXLEN) {
				ZWWARN("notify���볬�������С����");
				return ELOCK_ERROR_PARAMINVALID;
			}
			//////////////////////////////////////////////////////////////////////////
			ZWNOTICE(pszJson);
			Sleep(50);
			zwPushString(pszJson);
			return ELOCK_ERROR_SUCCESS;
		}
		catch(...) {		//һ�������쳣��ֱ�ӷ��ش�����Ҫ��Ϊ�˲�׽δ����ʱ
			//���ڶ���Ϊ����ɷ���NULLָ��ĵ�SEH�쳣  
			//Ϊ��ʹ�õײ�Poco����cceblock�������ҵ�WS��
			//����WS������Ϊδ���Ӷ���NULLʱֱ��throwһ��ö��
			//Ȼ���ڴˣ�Ҳ�����ϲ㲶����ʱ��֪������ȷ����
			//����catch�����쳣��
			ZWFATAL(__FUNCTION__);
			ZWFATAL("NotifyJsonͨ����·���������쳣�������������")
				return ELOCK_ERROR_CONNECTLOST;
		}
	}

	//��void ThreadLockComm() �޸Ķ���
	//�����������߷���ֵ��3�볬ʱ���أ�ֱ�ӷ����յ���JSON����
	CCBELOCK_API void jcRecvJsonFromLock(char *outJson,const int outMaxLen) {
		ZWFUNCTRACE boost::mutex::scoped_lock lock(zwccbthr::thr_mutex);

		try {			
			const int BLEN = 1024;
			char recvBuf[BLEN + 1];
			memset(recvBuf, 0, BLEN + 1);
			int recvLen = 0;
			//while (1) {
#ifndef ZWUSE_HID_MSG_SPLIT
				if (NULL == zwComPort) {
					ZWNOTICE
						("��·�Ѿ��رգ�ͨ���߳̽��˳�");
					return;
				}
#endif // ZWUSE_HID_MSG_SPLIT
				ZWNOTICE("�������߳ɹ�");
				ZWINFO("ͨ���̵߳���һ�ֵȴ���������ѭ����ʼ");
				try {
#ifdef ZWUSE_HID_MSG_SPLIT
					JCHID_STATUS sts=
						jcHidRecvData(&zwccbthr::hidHandle,
						recvBuf, BLEN, &recvLen,JCHID_RECV_TIMEOUT);
					zwCfg::s_hidOpened=true;	//����ͨ���̵߳�һ��������־					
					//Ҫ��ʲôҲû�յ�����ֱ�ӽ�����һ��ѭ��
					if (JCHID_STATUS_OK!=sts)
					{
						printf("JCHID_STATUS No DATA RECVED %d\n",sts);
						return;						
					}
					printf("\n");
#else
					zwComPort->RecvData(recvBuf, BLEN,
						&recvLen);
#endif // ZWUSE_HID_MSG_SPLIT

					//////////////////////////////////////////////////////////////////////////

					ZWNOTICE("�ɹ������߽����������£�");
				}
				catch(...) {
					ZWFATAL
						("RecvData��������ʱ�����ߵ����������쳣�Ͽ������ݽ����߳̽���ֹ");
					return;
				}
				ZWNOTICE(recvBuf);
			//��������
				//�յ������ݳ��ȴ��������������С�ͽض����
				if (recvLen>outMaxLen)
				{
					strncpy(outJson,recvBuf,outMaxLen);
				}
				else
				{
					strncpy(outJson,recvBuf,recvLen);
				}
				
			//}	//while (1) {
			ZWINFO("��ͨ�����ݽ��չ�����������");

		}		//try
		catch(...) {			
			//�쳣�Ͽ����趨�ñ�־ΪFALSE,�Ա��´�Open��Ҫ����������ͨ���̵߳ĳ����
			zwCfg::s_hidOpened=false;
			ZWFATAL
				("��ͨ�����ݽ��չ��������������쳣�Ͽ����������ݽ��չ����쳣����");
			return;
		}	
	}

}	//end of namespace jcLockJsonCmd_t2015a{