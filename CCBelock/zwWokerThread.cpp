#include "stdafx.h"
#include "CCBelock.h"
#include "zwCcbElockHdr.h"
#include "zwPocoLog.h"
#include "zwHidSplitMsg.h"
#include "zwHidComm.h"
#include <stdio.h>
using namespace boost::property_tree;

int myOpenElock1503(JCHID *jcElock);
void myCloseElock1503(void);

namespace zwccbthr {
	boost::mutex thr_mutex;
	//���и��Ľӿڣ�û���������Ӳ����ĵط���Ҳ����˵����ȫ���Զ˿ڣ��ֻ��Ǵ������ļ���ȡ
	boost::mutex recv_mutex;
	std::string s_ComPort;
	std::deque < string > dqOutXML;
	JCHID hidHandle;

	void wait(int milliseconds) {
		boost::this_thread::sleep(boost::
					  posix_time::milliseconds
					  (milliseconds));
	} string zwGetLockIP(void) {
		string myLockIp = "COM2";	//Ĭ��ֵ��COM2
		if (s_ComPort.length() > 0) {
			myLockIp = s_ComPort;	//����������ļ��Ĵ���ֵ����ʹ��֮��
		}
		ZWNOTICE("��������IPʹ��");
		ZWNOTICE(myLockIp.c_str());
		return myLockIp;
	}

	//������֮���ͨѶ�߳�
	void ThreadLockComm() {
		//ZWFUNCTRACE 
		ZWNOTICE("������֮���ͨѶ�߳�����")
		boost::mutex::scoped_lock lock(thr_mutex);

		try {			
			const int BLEN = 1024;
			char recvBuf[BLEN + 1];
			memset(recvBuf, 0, BLEN + 1);
			int outLen = 0;
			//Open(1);
			while (1) {
				printf("###############JCCOMMTHREAD 327 RUNNING\n");
				myOpenElock1503(&zwccbthr::hidHandle);
				time_t thNow=time(NULL);
				if (thNow % 3 ==0)
				{
					OutputDebugStringA(("���������ݽ����߳�����������"));
				}
				if (thNow % 60 ==0)
				{
					ZWINFO("���������ݽ����߳�����������");
				}
					
				try {
					boost::this_thread::interruption_point();
					if (NULL==zwccbthr::hidHandle.hid_device)
					{
						Sleep(700);
						continue;
					}
					JCHID_STATUS sts=JCHID_STATUS_FAIL;
					if (NULL!=zwccbthr::hidHandle.hid_device)
					{
						sts=jcHidRecvData(&zwccbthr::hidHandle,
							recvBuf, BLEN, &outLen,JCHID_RECV_TIMEOUT);
						//myCloseElock1503();
					}
					//zwCfg::s_hidOpened=true;	//����ͨ���̵߳�һ��������־					
					//Ҫ��ʲôҲû�յ�����ֱ�ӽ�����һ��ѭ��
					if (JCHID_STATUS_OK!=sts)
					{
						if (NULL != zwCfg::g_WarnCallback) {
							//���ûص��������ؿ�
							zwCfg::g_WarnCallback("");
						}
						//printf("JCHID_STATUS 1225 %d\n",sts);
						Sleep(900);
						continue;
					}
					printf("\n");
					//////////////////////////////////////////////////////////////////////////

					if (strlen(recvBuf)>0)
					{
						ZWNOTICE("wkThr�ɹ������߽����������£�");
						ZWNOTICE(recvBuf);
					}
				}
				catch(boost::thread_interrupted &)
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
				

				boost::this_thread::interruption_point();
				string outXML;
				if (strlen(recvBuf)>0){
					jcAtmcConvertDLL::zwJCjson2CCBxml(recvBuf,
						outXML);
					ZWINFO("�������߻ش���Json��ת��Ϊ����XML�ɹ�");
					//XML��ͷ�Ĺ̶�����38���ַ����������һ����ǩ�����Լ����źϼ�4���ַ�
					assert(outXML.length() > 42);
					ZWDBGMSG(outXML.c_str());
					{
						boost::
							mutex::scoped_lock lock(recv_mutex);
						//�յ���XML�������
						dqOutXML.push_back(outXML);
					}
				}

				if (NULL==zwCfg::g_WarnCallback)
				{
					const char *err1="�ص�����ָ��Ϊ�գ��޷����ûص��������شӵ������յ��ı���";
					ZWWARN(err1);
					MessageBoxA(NULL,err1,"���ؾ���",MB_OK);
				}
				if (outXML.size()==0)
				{
					ZWWARN("�յ������߷�������Ϊ�գ��޷�����������Ϣ���ص�����");
				}
				if (NULL != zwCfg::g_WarnCallback && outXML.size()>0) {
					//���ûص�����������Ϣ��
					zwCfg::g_WarnCallback(outXML.c_str());
					ZWINFO
					    ("�ɹ��Ѵ����߽��յ������ݴ��ݸ��ص�����");
				} 					
				boost::this_thread::interruption_point();
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


//////////////////////////////////////////////////////////////////////////
}				//namespace zwccbthr{

CCBELOCK_API int zwPushString( const char *str )
{
	//ZWFUNCTRACE 
	assert(NULL != str && strlen(str) > 0);
	if (NULL == str || strlen(str) == 0) {
		return JCHID_STATUS_FAIL;
	}

		JCHID_STATUS sts=JCHID_STATUS_FAIL;

		sts=jcHidSendData(&zwccbthr::hidHandle, str, strlen(str));
			if (JCHID_STATUS_OK==sts)
			{
				ZWINFO("������Ϣ�����߳ɹ�,֤����������")
			}
			else
			{
				ZWINFO("������Ϣ������ʧ��,֤����������")
			}
	
		return sts;
}

