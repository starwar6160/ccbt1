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
	extern const long JC_CCBDLL_TIMEOUT;	//���ʱʱ��Ϊ30��,���ڲ���Ŀ�ľ���ﵽ���Ʊ�¶����
	//����һ���ص�����ָ��
	extern RecvMsgRotine g_WarnCallback;
	extern boost::mutex ComPort_mutex;	//���ڱ����������Ӷ���
	//�̶߳�����Ϊһ��ȫ�־�̬����������Ҫ��ʾ������������һ���߳�
	extern boost::thread * thr;
	extern bool s_hidOpened;
} //namespace zwCfg{  

namespace jcLockJsonCmd_t2015a{
	//��long Notify(const char *pszMsg)�޸Ķ���
	long jcSendJson2Lock(const char *pszJson)
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
	void jcRecvJsonFromLock(char *outJson,const int outMaxLen) {
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