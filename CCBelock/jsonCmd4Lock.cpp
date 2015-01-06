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
	long NotifyJson(const char *pszMsg)
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
			//if (inlen == 0 || inlen >= zwCfg::JC_MSG_MAXLEN) {
			//	ZWWARN("Notify���볬�������С����");
			//	return ELOCK_ERROR_PARAMINVALID;
			//}
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

	//��void ThreadLockComm() �޸Ķ���
	//������֮���ͨѶ�߳�
	void ThreadLockCommJson() {
		ZWFUNCTRACE boost::mutex::scoped_lock lock(zwccbthr::thr_mutex);

		try {			
			const int BLEN = 1024;
			char recvBuf[BLEN + 1];
			memset(recvBuf, 0, BLEN + 1);
			int outLen = 0;
			while (1) {
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
						recvBuf, BLEN, &outLen,JCHID_RECV_TIMEOUT);
					zwCfg::s_hidOpened=true;	//����ͨ���̵߳�һ��������־					
					//Ҫ��ʲôҲû�յ�����ֱ�ӽ�����һ��ѭ��
					if (JCHID_STATUS_OK!=sts)
					{
						printf("JCHID_STATUS 1225 %d\n",sts);
						Sleep(1000);
						continue;
					}
					printf("\n");
#else
					zwComPort->RecvData(recvBuf, BLEN,
						&outLen);
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

				string outXML;
				jcAtmcConvertDLL::zwJCjson2CCBxml(recvBuf,
					outXML);
				ZWINFO("�������߻ش���Json��ת��Ϊ����XML�ɹ�");
				//XML��ͷ�Ĺ̶�����38���ַ����������һ����ǩ�����Լ����źϼ�4���ַ�
				assert(outXML.length() > 42);
				ZWDBGMSG(outXML.c_str());
				{
					boost::
						mutex::scoped_lock lock(zwccbthr::recv_mutex);
					//�յ���XML�������
					zwccbthr::dqOutXML.push_back(outXML);
				}

				if (NULL != zwCfg::g_WarnCallback) {
					//���ûص�����������Ϣ��Ȼ��͹ر����ӣ�����ͨ���̣߳�
					zwCfg::g_WarnCallback(outXML.c_str());
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
			zwCfg::s_hidOpened=false;
			ZWFATAL
				("��ͨ�����ݽ����߳����������쳣�Ͽ����������ݽ����߳̽�����");
			return;
		}	
	}

}	//end of namespace jcLockJsonCmd_t2015a{