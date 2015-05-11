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

jcHidDevice *g_jhc=NULL;	//ʵ�ʵ�HID�豸�����

namespace jcAtmcConvertDLL {
	//Ϊ��ƥ�������б��ı������������ı������ͱ�־λ
	extern string s_pipeJcCmdDown;	
	extern string s_pipeJcCmdUp;
}

namespace zwccbthr {
	//���и��Ľӿڣ�û���������Ӳ����ĵط���Ҳ����˵����ȫ���Զ˿ڣ��ֻ��Ǵ������ļ���ȡ
	boost::mutex thrhid_mutex;
	void pushToCallBack( const char * recvBuf );
	deque<string> g_dqLockUpMsg;	//�����������͵Ĵ��������Ϣ����ʱ����

	void wait(int milliseconds) {
		boost::this_thread::sleep(boost::
					  posix_time::milliseconds
					  (milliseconds));
	} 
	
	//������֮���ͨѶ�߳�
	void ThreadLockComm() {
		//ZWFUNCTRACE 
		ZWWARN("������֮���ͨѶ�߳�����v724")
		try {			
			const int BLEN = 1024;
			char recvBuf[BLEN + 1];			
			//ÿ�����������´�һ��
			time_t lastOpenElock=time(NULL);
			//ÿ��һ��ʮ���ӣ���಻����Сʱ�Զ�ǿ�ƹر�һ��
			//��ΪHID�����ƺ���һ��Сʱ�ͻ���ʱ��ʧȥ��Ӧ��
			while (1) {		
								
				try {
					boost::this_thread::interruption_point();
					JCHID_STATUS sts=JCHID_STATUS_FAIL;
					{
						boost::mutex::scoped_lock lock(thrhid_mutex);
#ifdef _DEBUG
						ZWINFO("thrhid_mutex START")
#endif // _DEBUG
							//ÿ������������¼�Ⲣ�򿪵�����һ��
							if ((time(NULL)-lastOpenElock)>(60))
							{									
								if (ELOCK_ERROR_SUCCESS!=g_jhc->getConnectStatus())
								{
									ZWWARN("1551ÿ��1���Ӷ��ڼ����������ӵ�������ֹ�쳣����\n");
									ZWWARN("�����ر����Ӻ��ٴδ�����508")
									g_jhc->CloseJc();
									g_jhc->OpenJc();							
								}					
								lastOpenElock=time(NULL);							
							}						
//////////////////////////////////////////////////////////////////////////
						//�ٶ������������ͱ��Ĳ�����3����һ��Ӧ��Ҳ��2��
						int upMsgCount=0;
						//for (int i=0;i<3;i++)
						do
						{
							upMsgCount++;
							if (upMsgCount>19)
							{
								break;
							}
							memset(recvBuf, 0, BLEN + 1);
							sts=g_jhc->RecvJson(recvBuf,BLEN);							
							if (strlen(recvBuf)>0)
							{
								ZWWARN("�յ����߷�����Ϣ=")
								ZWWARN(recvBuf)
								string outXML;
								jcAtmcConvertDLL::zwJCjson2CCBxml(recvBuf,outXML);							
								//��ȷ�Ķ�Ӧ�·����ĵĻ�Ӧ����
								if(jcAtmcConvertDLL::s_pipeJcCmdDown==jcAtmcConvertDLL::s_pipeJcCmdUp)
								{									
									DLOG(WARNING)<<"������ȷ�Կڱ��ĸ��ϲ�"<<endl;
									pushToCallBack(outXML.c_str());	//���ݸ��ص�����
									jcAtmcConvertDLL::s_pipeJcCmdDown="";
									break;
								}								
								//���������������ͱ��Ĳ��漰������������
								if(jcAtmcConvertDLL::s_pipeJcCmdDown=="")
								{
									ZWINFO("���������������ͱ��Ĳ��漰�����������")
									pushToCallBack(outXML.c_str());	//���ݸ��ص�����
								}
								//�����������ͱ��Ĵ������
								if(jcAtmcConvertDLL::s_pipeJcCmdDown!=jcAtmcConvertDLL::s_pipeJcCmdUp
									&& jcAtmcConvertDLL::s_pipeJcCmdDown!="")
								{									
									ZWWARN("�������,�ݴ�������Щʱ���ٴ����ص�����")	
										ZWWARN(jcAtmcConvertDLL::s_pipeJcCmdDown)
										ZWWARN(jcAtmcConvertDLL::s_pipeJcCmdUp)
										g_dqLockUpMsg.push_back(outXML);
								}
							}
							else
							{
								//�ղ���ʲô��������������ѭ��
								DLOG(INFO)<<"HID��ȡû���κ����ݣ�����ѭ��"<<endl;
								break;
							}
						}while(jcAtmcConvertDLL::s_pipeJcCmdDown!=jcAtmcConvertDLL::s_pipeJcCmdUp);
						//end for (int i=0;i<3;i++)

						for (auto iter=g_dqLockUpMsg.begin();iter!=g_dqLockUpMsg.end();iter++)
						{
							ZWWARN("����ǰ���ݴ����������������Ϣ���ص�����")
							ZWWARN((*iter).c_str())
							pushToCallBack((*iter).c_str());
						}
						g_dqLockUpMsg.clear();
//////////////////////////////////////////////////////////////////////////
			
#ifdef _DEBUG
						ZWINFO("thrhid_mutex END")
#endif // _DEBUG
					}	//end if (s_jsonCmd.length()>0)
					//Ҫ��ʲôҲû�յ�����ֱ�ӽ�����һ��ѭ��
					if (JCHID_STATUS_OK!=sts)
					{
						Sleep(900);
						continue;
					}
					printf("\n");
					//////////////////////////////////////////////////////////////////////////
				}	//end of try {
				catch(boost::thread_interrupted &)
				{
					g_jhc->CloseJc();
					ZWERROR	("RecvData�ӵ�������������ʱ�������߳��յ���ֹ�źţ����ݽ����߳̽���ֹ");
					return;
				}
				catch(...) {
					g_jhc->CloseJc();
					ZWFATAL ("RecvData�ӵ�������������ʱ��������·�������δ֪�������ݽ����߳̽���ֹ");
					return;
				}
			}	//end of while (1) {	
			ZWINFO("��ͨ�����ݽ����߳������˳�");

		}		//try
		catch(...) {			
			//�쳣�Ͽ����趨�ñ�־ΪFALSE,�Ա��´�Open��Ҫ����������ͨ���̵߳ĳ����
			g_jhc->CloseJc();
			ZWFATAL("��ͨ�����ݽ����߳����������쳣�Ͽ����������ݽ����߳̽�����");
			return;
		}	
	}

	void pushToCallBack( const char * recvConvedXML )
	{
		if (NULL==recvConvedXML || strlen(recvConvedXML)==0)
		{
			ZWERROR("�յ������߷�������Ϊ�գ��޷�����������Ϣ���ص�����");
			ZWWARN(recvConvedXML);
		}

		
		if (strlen(recvConvedXML)>0){		
			//ZWINFO("�������߻ش���Json��ת��Ϊ����XML�ɹ�");
			//XML��ͷ�Ĺ̶�����38���ַ����������һ����ǩ�����Լ����źϼ�4���ַ�
			assert(strlen(recvConvedXML) > 42);
		}

		if (NULL==zwCfg::g_WarnCallback)
		{
			const char *err1="�ص�����ָ��Ϊ�գ��޷����ûص��������شӵ������յ��ı���";
			ZWERROR(err1);
			MessageBoxA(NULL,err1,"���ؾ���",MB_OK);
		}
		if (NULL != zwCfg::g_WarnCallback && strlen(recvConvedXML)>0) {
			//���ûص�����������Ϣ��
			//20150415.1727.Ϊ��������Ҫ�󣬿����ϴ���Ϣ�������ÿ2��һ����ֹATM����
			Sleep(920);
			zwCfg::g_WarnCallback(recvConvedXML);
#ifdef _DEBUG401
			ZWINFO("�ɹ��Ѵ����߽��յ������ݴ��ݸ��ص�����");
#endif // _DEBUG401
		}
	}



//////////////////////////////////////////////////////////////////////////
}				//namespace zwccbthr{

