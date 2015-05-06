#include "stdafx.h"
#include "CCBelock.h"
#include "zwCcbElockHdr.h"
#include "zwPocoLog.h"
#include "zwHidSplitMsg.h"
#include "zwHidComm.h"
#include "zwHidDevClass2015.h"
#include <stdio.h>
#include <deque>
using namespace boost::property_tree;
using jchidDevice2015::jcHidDevice;

jcHidDevice g_jhc;	//ʵ�ʵ�HID�豸����󣬹���ʱ�Զ�����




namespace jcAtmcConvertDLL {
	//Ϊ��ƥ�������б��ı������������ı������ͱ�־λ
	extern string s_pipeJcCmdDown;	
	extern string s_pipeJcCmdUp;
}

namespace zwccbthr {
	//���и��Ľӿڣ�û���������Ӳ����ĵط���Ҳ����˵����ȫ���Զ˿ڣ��ֻ��Ǵ������ļ���ȡ
	boost::mutex thrhid_mutex;
	string s_jsonCmd="";

	void wait(int milliseconds) {
		boost::this_thread::sleep(boost::
					  posix_time::milliseconds
					  (milliseconds));
	} 
	
	//������֮���ͨѶ�߳�
	void ThreadLockComm() {
		//ZWFUNCTRACE 
		ZWWARN("������֮���ͨѶ�߳�����")
		try {			
			const int BLEN = 1024;
			char recvBuf[BLEN + 1];			
			//ÿ�����������´�һ��
			time_t lastOpenElock=time(NULL);
			//ÿ��һ��ʮ���ӣ���಻����Сʱ�Զ�ǿ�ƹر�һ��
			//��ΪHID�����ƺ���һ��Сʱ�ͻ���ʱ��ʧȥ��Ӧ��
			//Open(1);
			while (1) {
				memset(recvBuf, 0, BLEN + 1);
				//printf("###############JCCOMMTHREAD 327 RUNNING\n");
				 
				{
#ifdef _DEBUG430
					//ǿ�ƹر����ӻᵼ�º����ղ������ݣ���ʱ����ô����
					//boost::mutex::scoped_lock lock(recv_mutex);
					//15����ǿ�ƹر�һ��,��ֹһ��Сʱ����HID����ʧЧ
					if ((time(NULL)-lastCloseElock)>19)
					{			
						lastCloseElock=time(NULL);						
						//ZWWARN("20150430.ÿ��4���������Զ�ǿ�ƶϿ�HID���ӷ�ֹ����ʧЧ��3���Ժ�ָ�")
						//Sleep(3000);
						continue;
					}
#endif // _DEBUG430


					//ÿ������������¼�Ⲣ�򿪵�����һ��
					if ((time(NULL)-lastOpenElock)>10)
					{					
						//g_jhc.OpenJc();
						lastOpenElock=time(NULL);
					}
					
				}
									
				try {
					boost::this_thread::interruption_point();
					if (ELOCK_ERROR_SUCCESS!=g_jhc.getConnectStatus())
					{
						Sleep(900);
						continue;
					}

					JCHID_STATUS sts=JCHID_STATUS_FAIL;
					if (s_jsonCmd.length()>0)
					{
						boost::mutex::scoped_lock lock(thrhid_mutex);
						g_jhc.SendJson(s_jsonCmd.c_str());
						sts=g_jhc.RecvJson(recvBuf,BLEN);
						s_jsonCmd.clear();
					}
					//zwCfg::s_hidOpened=true;	//����ͨ���̵߳�һ��������־					
					//Ҫ��ʲôҲû�յ�����ֱ�ӽ�����һ��ѭ��
					if (JCHID_STATUS_OK!=sts)
					{
						Sleep(900);
						continue;
					}
					printf("\n");
					//////////////////////////////////////////////////////////////////////////
				}
				catch(boost::thread_interrupted &)
				{
					ZWERROR	("RecvData�ӵ�������������ʱ�������߳��յ���ֹ�źţ����ݽ����߳̽���ֹ");
					return;
				}
				catch(...) {
					ZWFATAL ("RecvData�ӵ�������������ʱ��������·�������δ֪�������ݽ����߳̽���ֹ");
					return;
				}
				

				boost::this_thread::interruption_point();
				string outXML;
				if (strlen(recvBuf)>0){
					jcAtmcConvertDLL::zwJCjson2CCBxml(recvBuf,
						outXML);
					if (jcAtmcConvertDLL::s_pipeJcCmdDown!=jcAtmcConvertDLL::s_pipeJcCmdUp)
					//if(jcAtmcConvertDLL::s_pipeJcCmdDown.size()>0)
					{
						ZWERROR("%%%%430jcAtmcConvertDLL::s_pipeJcCmdDown!=jcAtmcConvertDLL::s_pipeJcCmdUp\n");
						ZWWARN(jcAtmcConvertDLL::s_pipeJcCmdDown)
						ZWWARN(jcAtmcConvertDLL::s_pipeJcCmdUp)
					}
					//ZWINFO("�������߻ش���Json��ת��Ϊ����XML�ɹ�");
					//XML��ͷ�Ĺ̶�����38���ַ����������һ����ǩ�����Լ����źϼ�4���ַ�
					assert(outXML.length() > 42);
				}

				if (NULL==zwCfg::g_WarnCallback)
				{
					const char *err1="�ص�����ָ��Ϊ�գ��޷����ûص��������شӵ������յ��ı���";
					ZWERROR(err1);
					MessageBoxA(NULL,err1,"���ؾ���",MB_OK);
				}
				if (strlen(recvBuf)==0)
				{
					ZWERROR("�յ������߷�������Ϊ�գ��޷�����������Ϣ���ص�����");
					ZWWARN(recvBuf);
				}
				if (NULL != zwCfg::g_WarnCallback && outXML.size()>0) {
					//���ûص�����������Ϣ��
					//20150415.1727.Ϊ��������Ҫ�󣬿����ϴ���Ϣ�������ÿ2��һ����ֹATM����
					Sleep(920);
					zwCfg::g_WarnCallback(outXML.c_str());
#ifdef _DEBUG401
					ZWINFO("�ɹ��Ѵ����߽��յ������ݴ��ݸ��ص�����");
#endif // _DEBUG401
				} 					
				boost::this_thread::interruption_point();

			}
			ZWINFO("��ͨ�����ݽ����߳������˳�");

		}		//try
		catch(...) {			
			//�쳣�Ͽ����趨�ñ�־ΪFALSE,�Ա��´�Open��Ҫ����������ͨ���̵߳ĳ����
			zwCfg::s_hidOpened=false;
			ZWFATAL("��ͨ�����ݽ����߳����������쳣�Ͽ����������ݽ����߳̽�����");
			return;
		}	
	}


//////////////////////////////////////////////////////////////////////////
}				//namespace zwccbthr{

