#include "stdafx.h"
#include "zwCcbElockHdr.h"
#include "zwHidComm.h"
#include "CCBelock.h"
#include "zwHidDevClass2015.h"
using namespace boost::property_tree;
using boost::mutex;

using jchidDevice2015::jcHidDevice;
extern jcHidDevice* g_jhc;	//ʵ�ʵ�HID�豸����󣬹���ʱ�Զ�����

namespace zwCfg {
	//����һ���ص�����ָ��
	extern RecvMsgRotine g_WarnCallback;
} //namespace zwCfg{  

namespace jcLockJsonCmd_t2015a27{
	//Ϊ�˱�������jsonʱ���صľ�̬�������ڶ��߳�״���µ�ʹ��
	boost::mutex recvstr_mutex;


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
			LOG(ERROR)<< 
				"Open Function incoming timeout Value must in -1 to " <<
				JC_CCBDLL_TIMEOUT << "seconds";
			return ELOCK_ERROR_PARAMINVALID;
		}
		//�������0��С��JC_CCBDLL_TIMEOUT��������һ������Χ��
		LOG(WARNING)<< "Open Return " << ELOCK_ERROR_SUCCESS << endl;
		string myLockIp;
		try {

		}
		catch(...) {
			string errMsg = "�򿪶˿�" + myLockIp + "ʧ��";
			ZWFATAL(errMsg.c_str())
		}

		ZWWARN("�ɹ��� �����ߵ�JSON����")
			return ELOCK_ERROR_SUCCESS;
	}

	CCBELOCK_API long JCAPISTD CloseJson()
	{
		ZWFUNCTRACE 
		zwCfg::g_WarnCallback = NULL;
		
			g_jhc->CloseJc();

		ZWWARN("�ر� �����ߵ�JSON����")
			return ELOCK_ERROR_SUCCESS;
	}


	//��long Notify(const char *pszMsg)�޸Ķ���
	CCBELOCK_API long SendToLockJson(const char *pszJson)
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
			ZWFATAL("Notify����Ϊ��")
				return ELOCK_ERROR_PARAMINVALID;
		}
		if (NULL != pszJson && strlen(pszJson) > 0) {
			LOG(INFO)<< "JinChu�·�JSON=" << endl << pszJson <<
				endl;
		}
		
		try {
			int inlen = strlen(pszJson);
			assert(inlen > 0 && inlen < JC_MSG_MAXLEN);
			if (inlen == 0 || inlen >= JC_MSG_MAXLEN) {
				ZWERROR("notify���볬�������С����");
				return ELOCK_ERROR_PARAMINVALID;
			}
			//////////////////////////////////////////////////////////////////////////
			ZWWARN(pszJson);
			Sleep(50);
			g_jhc->SendJson(pszJson);
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
	CCBELOCK_API const char * RecvFromLockJson( const int timeoutMs )
	{
		ZWFUNCTRACE 
		//��ʱֵĬ��ֵ
		int realTimeOut=JCHID_RECV_TIMEOUT;
		//�����ʱֵ��һ������Χ�ڣ��Ͳ���
		if (timeoutMs>0 && timeoutMs <(3600*1000))
		{
			realTimeOut=timeoutMs;
		}
		try {			
			const int BLEN = 1024;
			char recvBuf[BLEN + 1];
			memset(recvBuf, 0, BLEN + 1);
			int recvLen = 0;
			//while (1) {
#ifndef ZWUSE_HID_MSG_SPLIT
				if (NULL == zwComPort) {
					ZWERROR	("��·�Ѿ��رգ�ͨ���߳̽��˳�");
					return;
				}
#endif // ZWUSE_HID_MSG_SPLIT
				ZWWARN("��������JSON�ɹ� jsonCmd4Lock.cpp");			
				try {
#ifdef ZWUSE_HID_MSG_SPLIT
					JCHID_STATUS sts=static_cast<JCHID_STATUS>(g_jhc->RecvJson(recvBuf,BLEN));
					zwCfg::s_hidOpened=true;	//����ͨ���̵߳�һ��������־					
					//Ҫ��ʲôҲû�յ�����ֱ�ӽ�����һ��ѭ��
					if (JCHID_STATUS_OK!=sts)
					{
						printf("JCHID_STATUS No DATA RECVED %d\n",sts);
						return "JCHID_STATUS No DATA RECVED";						
					}
					printf("\n");
#else
					zwComPort->RecvData(recvBuf, BLEN,
						&recvLen);
#endif // ZWUSE_HID_MSG_SPLIT

					//////////////////////////////////////////////////////////////////////////

					ZWWARN("jsonCmd4Lock.cpp cmd4Lock�ɹ������߽���JSON�������£�");
				}
				catch(...) {
					ZWFATAL("RecvData����JSON����ʱ�����ߵ����������쳣�Ͽ������ݽ��ս���ֹ");
					return "RecvData����JSON����ʱ�����ߵ����������쳣�Ͽ������ݽ��ս���ֹ";
				}
				ZWWARN(recvBuf);
			//}	//while (1) {
			ZWINFO("��ͨ�����ݽ���JSON������������");
			{
				//��������
				//Ϊ�˱������������õ�static std::string retStr,������
				boost::mutex::scoped_lock lockRetStr(recvstr_mutex);
				static std::string retStr=recvBuf;
				return retStr.c_str();
			}
		}		//try
		catch(...) {			
			//�쳣�Ͽ����趨�ñ�־ΪFALSE,�Ա��´�Open��Ҫ����������ͨ���̵߳ĳ����
			zwCfg::s_hidOpened=false;
			ZWFATAL("��ͨ�����ݽ��չ��������������쳣�Ͽ����������ݽ���JSON�����쳣����");
			return "��ͨ�����ݽ��չ��������������쳣�Ͽ����������ݽ���JSON�����쳣����";
		}	
	}

}	//end of namespace jcLockJsonCmd_t2015a21{