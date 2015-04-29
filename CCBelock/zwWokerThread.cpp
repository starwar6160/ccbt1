#include "stdafx.h"
#include "CCBelock.h"
#include "zwCcbElockHdr.h"
#include "zwPocoLog.h"
#include "zwHidSplitMsg.h"
#include "zwHidComm.h"
#include <stdio.h>
#include <deque>
using namespace boost::property_tree;

int myOpenElock1503(JCHID *jcElock);
void myCloseElock1503(void);

namespace zwccbthr {
	boost::mutex thr_mutex;
	//���и��Ľӿڣ�û���������Ӳ����ĵط���Ҳ����˵����ȫ���Զ˿ڣ��ֻ��Ǵ������ļ���ȡ
	boost::mutex recv_mutex;
	std::string s_ComPort;
	std::deque < string > dqOutXML;
	std::deque <string> dqPushJson;
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
		ZWWARN("��������IPʹ��");
		ZWWARN(myLockIp.c_str());
		return myLockIp;
	}

	//������֮���ͨѶ�߳�
	void ThreadLockComm() {
		//ZWFUNCTRACE 
		ZWWARN("������֮���ͨѶ�߳�����")
		boost::mutex::scoped_lock lock(thr_mutex);

		try {			
			const int BLEN = 1024;
			char recvBuf[BLEN + 1];			
			int outLen = 0;
			myOpenElock1503(&zwccbthr::hidHandle);
			//ÿ�����������´�һ��
			time_t lastOpenElock=time(NULL);
			//ÿ��һ��ʮ���ӣ���಻����Сʱ�Զ�ǿ�ƹر�һ��
			//��ΪHID�����ƺ���һ��Сʱ�ͻ���ʱ��ʧȥ��Ӧ��
			time_t lastCloseElock=time(NULL);
			//Open(1);
			while (1) {
				memset(recvBuf, 0, BLEN + 1);
				//printf("###############JCCOMMTHREAD 327 RUNNING\n");
				 
				//ÿ������������¼�Ⲣ�򿪵�����һ��
				if ((time(NULL)-lastOpenElock)>6)
				{					
					myOpenElock1503(&zwccbthr::hidHandle);
					lastOpenElock=time(NULL);
				}
				
				time_t thNow=time(NULL);
#ifdef _DEBUG417
				if (thNow % 6 ==0)
				{
					OutputDebugStringA(("���������ݽ����߳�����������"));
				}
				if (thNow % 60 ==0)
				{
					ZWINFO("���������ݽ����߳�����������");
				}
#endif // _DEBUG417
					
				try {
					boost::this_thread::interruption_point();
					if (NULL==zwccbthr::hidHandle.hid_device)
					{
						Sleep(300);
						continue;
					}
					JCHID_STATUS sts=JCHID_STATUS_FAIL;
					if (NULL!=zwccbthr::hidHandle.hid_device)
					{						
						{
							//boost::mutex::scoped_lock lock(recv_mutex);
							//OutputDebugStringA("415����һ�����߷�����Ϣ��ʼ\n");
							sts=jcHidRecvData(&zwccbthr::hidHandle,
								recvBuf, BLEN, &outLen,JCHID_RECV_TIMEOUT);
							//OutputDebugStringA("415����һ�����߷�����Ϣ����\n");
						}						
						//myCloseElock1503();
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
					//ZWINFO("�������߻ش���Json��ת��Ϊ����XML�ɹ�");
					//XML��ͷ�Ĺ̶�����38���ַ����������һ����ǩ�����Լ����źϼ�4���ַ�
					assert(outXML.length() > 42);
					//ZWDBGMSG(outXML.c_str());
					{
						boost::mutex::scoped_lock lock(recv_mutex);
						//�յ���XML�������
						dqOutXML.push_back(outXML);
					}
				}

				if (NULL==zwCfg::g_WarnCallback)
				{
					const char *err1="�ص�����ָ��Ϊ�գ��޷����ûص��������شӵ������յ��ı���";
					ZWERROR(err1);
					MessageBoxA(NULL,err1,"���ؾ���",MB_OK);
				}
				if (outXML.size()==0)
				{
					ZWERROR("�յ������߷�������Ϊ�գ��޷�����������Ϣ���ص�����");
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
				//15����ǿ�ƹر�һ��,��ֹһ��Сʱ����HID����ʧЧ
				if ((time(NULL)-lastCloseElock)>60*3)
				{			
					boost::mutex::scoped_lock lock(zwccbthr::recv_mutex);
					myCloseElock1503();
					lastCloseElock=time(NULL);
					//Sleep(3000);
					ZWWARN("20150427.ÿ��15�����Զ�ǿ�ƶϿ�HID���ӷ�ֹ����ʧЧ��3���Ժ�ָ�")
				}

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

CCBELOCK_API int zwPushString( const char *str )
{
	//ZWFUNCTRACE 
	assert(NULL != str && strlen(str) > 0);
	if (NULL == str || strlen(str) == 0) {
		ZWERROR("zwPushString input string is NULL")
		return JCHID_STATUS_FAIL;
	}

		JCHID_STATUS sts=JCHID_STATUS_FAIL;
		//Sleep(1000);		
		static time_t lastPrint=time(NULL);
		{			
			//20150415.1727.Ϊ��������Ҫ�󣬿����·���Ϣ�������ÿ��һ����ֹ��λ������
			boost::mutex::scoped_lock lock(zwccbthr::recv_mutex);
			//20150421.0935.Ӧ������Ҫ���·���Ϣ�ӳٷŵ���������ڲ�
			Sleep(1000);
			zwccbthr::dqPushJson.push_back(string(str));
			sts=jcHidSendData(&zwccbthr::hidHandle, str, strlen(str));
		}
		
		if (time(NULL)-lastPrint>6)
		{
			if (JCHID_STATUS_OK==sts)
			{
				//ZWINFO("��⵽��������")
			}
			else
			{
				ZWINFO("��⵽��������")
			}
			lastPrint=time(NULL);
		}
	
		return sts;
}

