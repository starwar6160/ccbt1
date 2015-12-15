#include "stdafx.h"
#include "CCBelock.h"
#include "zwCcbElockHdr.h"
#include "zwHidSplitMsg.h"
#include "zwHidComm.h"
#include "zwHidDevClass2015.h"
#include ".\\ATMCMsgConvert\\myConvIntHdr.h"
#include <stdio.h>
#include <deque>
using namespace boost::property_tree;
using boost::condition_variable;
using boost::condition_variable_any;
using jchidDevice2015::jcHidDevice;
using jcAtmcConvertDLL::jcLockMsg1512_t;

jcHidDevice *g_jhc=NULL;	//ʵ�ʵ�HID�豸�����

namespace zwccbthr {
	//���и��Ľӿڣ�û���������Ӳ����ĵط���Ҳ����˵����ȫ���Զ˿ڣ��ֻ��Ǵ������ļ���ȡ
	boost::mutex thrhid_mutex;
	void pushToCallBack( const char * recvBuf );
	deque<string> g_dqLockUpMsg;	//�����������͵Ĵ��������Ϣ����ʱ����
	bool myWaittingReturnMsg=false;	//�ȴ����ر����ڼ䲻Ҫ�·�����
	boost::condition_variable condJcLock;
	boost::timer g_LatTimer;	//�����Զ������ӳ�
	deque<jcLockMsg1512_t *> s_jcNotify;		//�·�����

	void wait(int milliseconds) {
		assert(milliseconds>0);
		boost::this_thread::sleep(boost::
					  posix_time::milliseconds
					  (milliseconds));
	} 

	time_t zwNormalTime521(time_t inTime,time_t gapSeconds)
	{
		assert(inTime>0);
		assert(gapSeconds>1);
		time_t tail=inTime % gapSeconds;
		return inTime-tail;

	}

	void pushToCallBack( const char * recvConvedXML )
	{
		assert(NULL!=recvConvedXML);
		assert(strlen(recvConvedXML)>0);
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
			//Sleep(2920);
			zwCfg::g_WarnCallback(recvConvedXML);
			ZWERROR(recvConvedXML)
			VLOG_IF(4,strlen(recvConvedXML)>0)<<"�ص������յ���������\n"<<recvConvedXML<<endl;
#ifdef _DEBUG401
			
			VLOG(4)<<"�ɹ��Ѵ����߽��յ������ݴ��ݸ��ص�����\n";
#endif // _DEBUG401
		}
	}
	

	void my515LockRecvThr(void)
	{
		
		ZWERROR("������֮������ݽ����߳�����.20151215.0925.v804")
		const int BLEN = 1024;
		char recvBuf[BLEN];			
		using zwccbthr::s_jcNotify;

		//Sleep(1300);	//�����������б��ĵ��߳�����ǰ���ʵ��ӳ�
		while (1)
		{	
			boost::this_thread::interruption_point();
			{
			boost::mutex::scoped_lock lock(thrhid_mutex);		
			JCHID_STATUS sts=JCHID_STATUS_FAIL;			
			{
				//boost::mutex::scoped_lock lock(thrhid_mutex);						
				VLOG_IF(4,s_jcNotify.size()>0)<<"s_jcNotify.size()="<<s_jcNotify.size()<<endl;
				if (s_jcNotify.size()>0)
				{
					zwccbthr::myWaittingReturnMsg=true;	
					string sCmd=s_jcNotify.front()->NotifyMsg;
					LOG(WARNING)<<"���͸����ߵ���Ϣ.������"<<sCmd<<endl;
					sts=static_cast<JCHID_STATUS>( g_jhc->SendJson(sCmd.c_str()));
					
					//��������̽�����
					if (JCHID_STATUS_OK!=static_cast<JCHID_STATUS>(sts))
					{
						g_jhc->OpenJc();
					}
					s_jcNotify.pop_front();
				}
			}
			do{
				//���ǵ��п������ߵ���������Ϣ����һ���·���Ϣ�ж���
				//������Ϣ�����Զ��ȡ����ֱ����������ϢΪֹ
				memset(recvBuf,0,BLEN);
				VLOG(4)<<"�������ݵ�RecvJson֮ǰ"<<endl;
				sts=static_cast<JCHID_STATUS>(g_jhc->RecvJson(recvBuf,BLEN));	
				int lRecvBytes=strlen(recvBuf);
				VLOG_IF(3,lRecvBytes>0)<<"RecvJson�յ�"<<lRecvBytes<<"�ֽڵ�����"<<endl;
				if (strlen(recvBuf)>0)
				{
					VLOG(4)<<"�յ����߷�����Ϣ= "<<recvBuf<<endl;
					assert(strlen(recvBuf)>0);
					VLOG(1)<<"�յ����߷�����Ϣ.������\n"<<recvBuf<<endl;
					string outXML;
					jcAtmcConvertDLL::zwJCjson2CCBxml(recvBuf,outXML);	

						//����һ��һ��ģ������ϴ�						
						if (outXML.size()>0)
						{
							//ZWWARN("�����ϴ�����")
							pushToCallBack(outXML.c_str());
							outXML="";
						}													
				}
			}while(strlen(recvBuf)>0);
			
		}	//�շ�thrhid_mutex����			
			//condJcLock.notify_all();	
			Sleep(100);	//�������һ�ֱ��ĺ���ͣ100������·������ڳ�ʱ�䣻
			VLOG(4)<<__FUNCTION__<<"\tSleep 100 ms"<<endl;
		}

	}

	void my515UpMsgThr(void)
	{
		ZWERROR("��ATMC֮��������ϴ��߳�����.20151215")
			while (1)
			{
				//LOG(ERROR)<<__FUNCTION__<<"RUNNING " <<time(NULL)<<endl;
				VLOG(4)<<__FUNCTION__;				
				VLOG(4)<<__FUNCTION__<<"START"<<endl;
				//�ȴ����ݽ����̲߳�����ϡ��յ������ݡ�����
				//��øö��е���������Ȩ����ʼ����
				VLOG(4)<<__FUNCTION__<<" scoped_lock lock(thrhid_mutex) START"<<endl;
				boost::mutex::scoped_lock lock(thrhid_mutex);				
				VLOG(4)<<__FUNCTION__<<"condJcLock.wait(lock);"<<endl;
				//condJcLock.wait(lock);		
				//ֻ�е������շ��̲߳��ڵȴ�һ��һ��һ��ķ��ر����ڼ�
				// ���ϴ��ñ��ӳ��ϴ��ı����������һ��һ��
					for (int i=0;i<g_dqLockUpMsg.size();i++)
					{
						LOG(WARNING)<<"�ӳ��ϴ�����"<<endl;
						pushToCallBack(g_dqLockUpMsg[i].c_str());
						g_dqLockUpMsg.pop_front();
					}
				VLOG(4)<<__FUNCTION__<<"END"<<endl;
				//������ϡ��յ������ݡ����У��ͷ���������Ȩ
				VLOG(4)<<__FUNCTION__<<" condJcLock.notify_all();"<<endl;
				//condJcLock.notify_all();	
				VLOG(4)<<__FUNCTION__<<" scoped_lock lock(thrhid_mutex) END"<<endl;
			}
	}

//////////////////////////////////////////////////////////////////////////
}				//namespace zwccbthr{

