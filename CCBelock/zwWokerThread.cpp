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
	bool myDownUpLoopIng=false;	//Ϊ��ά��һ���·�/����ѭ��������

	void wait(int milliseconds) {
		boost::this_thread::sleep(boost::
					  posix_time::milliseconds
					  (milliseconds));
	} 
	
	//������֮���ͨѶ�߳�
	void ThreadLockComm() {
		//ZWFUNCTRACE 
		ZWWARN("������֮���ͨѶ�߳�����v731")
		try {			
			const int BLEN = 1024;
			char recvBuf[BLEN + 1];			
			//ÿ�����������´�һ��
			time_t lastOpenElock=time(NULL);
			time_t lastPopUpMsg=time(NULL);
			
			//ÿ��һ��ʮ���ӣ���಻����Сʱ�Զ�ǿ�ƹر�һ��
			//��ΪHID�����ƺ���һ��Сʱ�ͻ���ʱ��ʧȥ��Ӧ��
			while (1) {		
								
				try {
					boost::this_thread::interruption_point();
					JCHID_STATUS sts=JCHID_STATUS_FAIL;
					{
						boost::mutex::scoped_lock lock(thrhid_mutex);
#ifdef _DEBUG
						VLOG(4)<<"thrhid_mutex START"<<endl;
#endif // _DEBUG
							//ÿ������������¼�Ⲣ�򿪵�����һ��
							if ((time(NULL)-lastOpenElock)>(60*1))
							{									
								//if (ELOCK_ERROR_SUCCESS!=g_jhc->getConnectStatus())
								{
									ZWINFO("0908ÿ��1���Ӷ��ڼ����������ӵ�������ֹ�쳣����\n");
									ZWWARN("�����ر����Ӻ��ٴδ�����512")
									g_jhc->CloseJc();
									g_jhc->OpenJc();							
								}												
								lastOpenElock=time(NULL);							
							}						

							//�����ϴ�������Ϣ������еĻ�.����Ҫ��ʱ����һ��һ��������
							if ((time(NULL)-lastPopUpMsg)>(20) && zwccbthr::myDownUpLoopIng==false)
							{
								VLOG_IF(3,g_dqLockUpMsg.size()>0)<<"g_dqLockUpMsg.size()="<<g_dqLockUpMsg.size()<<endl;
								for (auto iter=g_dqLockUpMsg.begin();iter!=g_dqLockUpMsg.end();iter++)
								{
									ZWWARN("ÿ��20�뵯��ǰ���ݴ����������������Ϣ���ص�����")
										ZWWARN((*iter).c_str())
										pushToCallBack((*iter).c_str());
								}
								g_dqLockUpMsg.clear();
								lastPopUpMsg=time(NULL);
							}
//////////////////////////////////////////////////////////////////////////
						//�ٶ������������ͱ��Ĳ�����3����һ��Ӧ��Ҳ��2��
						int upMsgCount=0;
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
								ZWINFO("�յ����߷�����Ϣ=")
								ZWINFO(recvBuf)
								string outXML;
								jcAtmcConvertDLL::zwJCjson2CCBxml(recvBuf,outXML);							
								//���������������ͱ��Ĳ��漰������������,�Լ��������ͱ��������֤������������
								if(zwccbthr::myDownUpLoopIng==false 
									&& (jcAtmcConvertDLL::s_pipeJcCmdUp=="Lock_Open_Ident" ||
									jcAtmcConvertDLL::s_pipeJcCmdUp=="Lock_Close_Code_Lock" )	)
								{
									//Lock_Open_Ident���������ǿ����Ĺؼ�������֤�뱨�ģ����ñ�ѹ��
									//������������ǰ����
									ZWWARN("����һ��һ���ڼ�ĵ��������������ͱ��Ĳ��漰�����������")
										pushToCallBack(outXML.c_str());	//���ݸ��ص�����
								}

								//��ȷ�Ķ�Ӧ�·����ĵĻ�Ӧ����
								if(jcAtmcConvertDLL::s_pipeJcCmdDown==jcAtmcConvertDLL::s_pipeJcCmdUp)
								{									
									DLOG(WARNING)<<"������ȷ�Կڱ��ĸ��ϲ�"<<endl;
									//���ݸ��ص�����
									pushToCallBack(outXML.c_str());	
									jcAtmcConvertDLL::s_pipeJcCmdDown="";
									//һ��һ���������
									zwccbthr::myDownUpLoopIng=false;	
									break;
								}								

								//�����������ͱ��Ĵ������,�Լ��ر�������Ϣ��ʱѹ��
								if((jcAtmcConvertDLL::s_pipeJcCmdDown!=jcAtmcConvertDLL::s_pipeJcCmdUp
									&& jcAtmcConvertDLL::s_pipeJcCmdDown!="") ||
									jcAtmcConvertDLL::s_pipeJcCmdUp=="Lock_Alarm_Info")
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
								VLOG(4)<<"HID��ȡû���κ����ݣ�����ѭ��"<<endl;
								break;
							}
						}while(strlen(recvBuf)>0);						

//////////////////////////////////////////////////////////////////////////
			
#ifdef _DEBUG
						VLOG(4)<<"thrhid_mutex END"<<endl;
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
			return;
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
			Sleep(2920);
			zwCfg::g_WarnCallback(recvConvedXML);
			VLOG_IF(3,strlen(recvConvedXML)>0)<<"�ص������յ���������\n"<<recvConvedXML<<endl;
#ifdef _DEBUG401
			ZWINFO("�ɹ��Ѵ����߽��յ������ݴ��ݸ��ص�����");
#endif // _DEBUG401
		}
	}



//////////////////////////////////////////////////////////////////////////
}				//namespace zwccbthr{

