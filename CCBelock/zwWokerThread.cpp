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

jcHidDevice *g_jhc=NULL;	//ʵ�ʵ�HID�豸�����

namespace zwccbthr {
	//���и��Ľӿڣ�û���������Ӳ����ĵط���Ҳ����˵����ȫ���Զ˿ڣ��ֻ��Ǵ������ļ���ȡ
	boost::mutex thrhid_mutex;
	void pushToCallBack( const char * recvBuf );
	deque<string> g_dqLockUpMsg;	//�����������͵Ĵ��������Ϣ����ʱ����
	bool myWaittingReturnMsg=false;	//�ȴ����ر����ڼ䲻Ҫ�·�����
	boost::condition_variable condJcLock;
	boost::timer g_LatTimer;	//�����Զ������ӳ�
	deque<string> s_jcNotify;		//�·�����

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
		ZWERROR("������֮������ݽ����߳�����.20150615.v768")
		const int BLEN = 1024;
		char recvBuf[BLEN];			
		using zwccbthr::s_jcNotify;
		string upMsgType,downMsgType;
		string upMsg,downMsg;

		Sleep(1300);
		while (1)
		{		
			{
			//LOG(ERROR)<<__FUNCTION__<<"RUNNING " <<time(NULL)<<endl;
			VLOG(4)<<__FUNCTION__<<"START"<<endl;
			//VLOG(3)<<__FUNCTION__;				
			boost::mutex::scoped_lock lock(thrhid_mutex);		
			JCHID_STATUS sts=JCHID_STATUS_FAIL;			
			{
				//boost::mutex::scoped_lock lock(thrhid_mutex);						
				VLOG_IF(4,s_jcNotify.size()>0)<<"s_jcNotify.size()="<<s_jcNotify.size()<<endl;
				if (s_jcNotify.size()>0)
				{
					zwccbthr::myWaittingReturnMsg=true;					
					LOG(WARNING)<<"SendToLock JsonIS\n"<<s_jcNotify.front().c_str()<<endl;
					downMsg=s_jcNotify.front();
					assert(downMsg.size()>0);
					downMsgType=jcAtmcConvertDLL::zwGetJcJsonMsgType(downMsg.c_str());
					LOG(INFO)<<"���͸����ߵ���Ϣ.������"<<downMsgType<<endl;
					LOG(INFO)<<"���͸����ߵ���Ϣ.������"<<downMsg<<endl;
					sts=static_cast<JCHID_STATUS>( g_jhc->SendJson(s_jcNotify.front().c_str()));
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
				sts=static_cast<JCHID_STATUS>(g_jhc->RecvJson(recvBuf,BLEN));				
				if (strlen(recvBuf)>0)
				{
					//boost::mutex::scoped_lock lock(thrhid_mutex);
					assert(downMsg.size()>0);
					downMsgType=jcAtmcConvertDLL::zwGetJcJsonMsgType(downMsg.c_str());
					assert(downMsgType.size()>0);
					LOG(INFO)<<"�ٴη����������ߵ���Ϣ.������"<<downMsgType
						<<"������\n"<<downMsg<<endl;
					VLOG(4)<<"�յ����߷�����Ϣ= "<<recvBuf<<endl;
					upMsg=recvBuf;
					assert(strlen(recvBuf)>0);
					upMsgType=jcAtmcConvertDLL::zwGetJcJsonMsgType(recvBuf);
					LOG(INFO)<<"�յ����߷�����Ϣ.������"<<upMsgType<<endl;
					string outXML;
					jcAtmcConvertDLL::zwJCjson2CCBxml(recvBuf,outXML);	

					if(downMsgType==upMsgType)
					{
						//����һ��һ��ģ������ϴ�						
						if (outXML.size()>0)
						{
							ZWWARN("�����ϴ�����")
							pushToCallBack(outXML.c_str());
							outXML="";
						}													
						downMsgType="";
					}
#ifdef _DEBUG615
					//������֤�룬�Լ������������͵ı��������������ⱨ�ģ�
					//���������ӳ��ϴ�����ֱ���ϴ�
					if (	"Lock_Open_Ident"		==upMsgType
						||	"Lock_Close_Code_Lock"	==upMsgType	)
					{						
						if (outXML.size()>0)
						{
							ZWWARN("�����������������ϴ�����.��֤�룬�����������")
							pushToCallBack(outXML.c_str());
							outXML="";
						}						
					}
#endif // _DEBUG615
					//����������һ��һ��ģ��ӳ��ϴ���
					if (downMsgType!=upMsgType)
					{
						if (outXML.size()>0)
						{
							LOG(WARNING)<<"downMsgType!=upMsgType:\t"<<
								downMsgType<<"!="<<upMsgType<<endl;
							g_dqLockUpMsg.push_back(outXML);		
							ZWWARN("���ᱻ�ӳ��ϴ�����")
							outXML="";
						}
					}										
					//��������б��ģ���ô��ʱ�Ѿ��յ�����ˣ����Խ��������
					//���û�����б��ģ���û��ϵ
					zwccbthr::myWaittingReturnMsg=false;
				}
			}while(strlen(recvBuf)>0);
		}
			condJcLock.notify_all();	
			Sleep(500);
			VLOG(4)<<__FUNCTION__<<"END"<<endl;
		}

	}

	void my515UpMsgThr(void)
	{
		ZWERROR("��ATMC֮��������ϴ��߳�����.20150515.1655")
			while (1)
			{
				//LOG(ERROR)<<__FUNCTION__<<"RUNNING " <<time(NULL)<<endl;
				VLOG(4)<<__FUNCTION__;				
				VLOG(4)<<__FUNCTION__<<"START"<<endl;
				//�ȴ����ݽ����̲߳�����ϡ��յ������ݡ�����
				//��øö��е���������Ȩ����ʼ����
				boost::mutex::scoped_lock lock(thrhid_mutex);				
				condJcLock.wait(lock);					
				for (int i=0;i<g_dqLockUpMsg.size();i++)
				{
					LOG(WARNING)<<"�ӳ��ϴ�����"<<endl;
					//LOG(ERROR)<<(g_dqLockUpMsg[i]);
					pushToCallBack(g_dqLockUpMsg[i].c_str());
					g_dqLockUpMsg.pop_front();
				}
				//g_dqLockUpMsg.clear();				
				VLOG(4)<<__FUNCTION__<<"END"<<endl;
				//������ϡ��յ������ݡ����У��ͷ���������Ȩ
				condJcLock.notify_all();	
			}
	}

//////////////////////////////////////////////////////////////////////////
}				//namespace zwccbthr{

