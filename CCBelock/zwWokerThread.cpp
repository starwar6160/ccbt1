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
	boost::mutex upDeque_mutex;
	void pushToCallBack( const char * recvConvedXML,RecvMsgRotine pCallBack );
	boost::timer g_LatTimer;	//�����Զ������ӳ�

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
//////////////////////////////////JcLockSendRecvData START////////////////////////////////////////
	//Ϊÿ���������߳�׼����ר���·����ϴ���Ϣ�Ķ���


	JcLockSendRecvData::JcLockSendRecvData(DWORD callerID,RecvMsgRotine pCallBack)
	{
		m_CallerThreadID=callerID;
		m_CallBack=pCallBack;
		VLOG(2)<<"������һ��JCATMC DLL�ϲ�Ӧ��ÿ���߳�ר�õ��ϴ��·����� �߳�ID= "<<m_CallerThreadID<<endl;
	}

	JcLockSendRecvData::~JcLockSendRecvData()
	{
		m_Notify.clear();
		m_UpMsg.clear();
		VLOG(2)<<"ɾ����һ��JCATMC DLL�ϲ�Ӧ��ÿ���߳�ר�õ��ϴ��·����� �߳�ID= "<<m_CallerThreadID<<endl;
	}

	DWORD JcLockSendRecvData::getCallerID()
	{
		VLOG_IF(4,m_CallerThreadID>0)<<__FUNCTION__<<"\t"<<m_CallerThreadID<<endl;
		return m_CallerThreadID;
	}

	void JcLockSendRecvData::PushNotifyMsg(const string &NotifyMsg)
	{
		boost::mutex::scoped_lock lock(notify_mutex);
		m_Notify.push_back(NotifyMsg);
		VLOG_IF(4,NotifyMsg.size()>0)<<__FUNCTION__<<"\t"<<NotifyMsg<<endl;
	}

	string JcLockSendRecvData::PullNotifyMsg()
	{
		boost::mutex::scoped_lock lock(notify_mutex);
		if (m_Notify.size()>0)
		{
			string retMsg=m_Notify.front();
			m_Notify.pop_front();
			VLOG_IF(4,retMsg.size()>0)<<__FUNCTION__<<"\t"<<retMsg<<endl;
			return retMsg;
		}
		VLOG(4)<<__FUNCTION__<<"\tNOMSG CAN PULL"<<endl;
		return "";
	}

	void JcLockSendRecvData::PushUpMsg(const string &UpMsg)
	{
		boost::mutex::scoped_lock lock(upmsg_mutex);
		m_UpMsg.push_back(UpMsg);
		VLOG_IF(4,UpMsg.size()>0)<<__FUNCTION__<<"\t"<<UpMsg<<endl;
	}

	string JcLockSendRecvData::UploadLockResult()
	{
		boost::mutex::scoped_lock lock(upmsg_mutex);
		if (m_UpMsg.size()>0)
		{
			string retMsg=m_UpMsg.front();
			pushToCallBack(retMsg.c_str(),m_CallBack);
			m_UpMsg.pop_front();
			VLOG_IF(4,retMsg.size()>0)<<__FUNCTION__<<"\t"<<retMsg<<endl;
			return retMsg;
		}
		VLOG(4)<<__FUNCTION__<<"\tNOMSG CAN PULL"<<endl;
		return "";
	}

//////////////////////////////////JcLockSendRecvData END////////////////////////////////////////
	void JcLockSendRecvData::pushToCallBack( const char * recvConvedXML,RecvMsgRotine pCallBack )
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

		if (NULL==pCallBack)
		{
			const char *err1="�ص�����ָ��Ϊ�գ��޷����ûص��������شӵ������յ��ı���";
			ZWERROR(err1);
			MessageBoxA(NULL,err1,"���ؾ���",MB_OK);
		}
		if (NULL != pCallBack && strlen(recvConvedXML)>0) {
			//���ûص�����������Ϣ��
			//20150415.1727.Ϊ��������Ҫ�󣬿����ϴ���Ϣ�������ÿ2��һ����ֹATM����
			//Sleep(2920);
			pCallBack(recvConvedXML);
			ZWERROR(recvConvedXML)
			VLOG_IF(4,strlen(recvConvedXML)>0)<<"�ص������յ���������\n"<<recvConvedXML<<endl;
#ifdef _DEBUG401
			
			VLOG(2)<<"�ɹ��Ѵ����߽��յ������ݴ��ݸ��ص�����\n";
#endif // _DEBUG401
		}
	}
	

	void mySendCmd2Lock(int i) 
	{
		JCHID_STATUS sts=JCHID_STATUS_FAIL;		

			string curCmd;
			curCmd=zwCfg::vecCallerCmdDq[i]->PullNotifyMsg();
			if (""!=curCmd)
			{
				LOG(WARNING)<<"���߳�"<<zwCfg::vecCallerCmdDq[i]->getCallerID()<<
					"���շ�����ȡ���·������ߵ���Ϣ.������"<<curCmd<<endl;

				sts=static_cast<JCHID_STATUS>( g_jhc->SendJson(curCmd.c_str()));

				//��������̽�����
				if (JCHID_STATUS_OK!=static_cast<JCHID_STATUS>(sts))
				{
					g_jhc->OpenJc();
				}					
			}		
	}

	void myRecvFromLock(int i) 
	{
		const int BLEN = 1024;
		char recvBuf[BLEN];	
		JCHID_STATUS sts=JCHID_STATUS_FAIL;							
		do{				
			//���ǵ��п������ߵ���������Ϣ����һ���·���Ϣ�ж���
			//������Ϣ�����Զ��ȡ����ֱ����������ϢΪֹ
			memset(recvBuf,0,BLEN);
			//VLOG(3)<<"�������ݵ�RecvJson֮ǰ"<<endl;
			sts=static_cast<JCHID_STATUS>(g_jhc->RecvJson(recvBuf,BLEN));				
			VLOG_IF(3,strlen(recvBuf)>0)<<"�������ݵ�RecvJson֮�� �յ� "<<
				strlen(recvBuf)<<" �ֽڵ�����"<<endl;
			if (strlen(recvBuf)>0)
			{
				VLOG(1)<<"�յ����߷�����Ϣ= "<<recvBuf<<endl;
				assert(strlen(recvBuf)>0);
				LOG(WARNING)<<"�յ����߷�����Ϣ.������\n"<<recvBuf<<endl;
				string outXML;
				jcAtmcConvertDLL::zwJCjson2CCBxml(recvBuf,outXML);					
				if (outXML.size()>0)
				{
					boost::mutex::scoped_lock lock(upDeque_mutex);	
					//�յ��ı��ķ����ϴ����У��ɵ������߳�ȥ�ϴ�
					zwCfg::vecCallerCmdDq[i]->PushUpMsg(outXML);
				}													
			}
		}while(strlen(recvBuf)>0);			
	}

	void my515LockRecvThr(void)
	{
		ZWERROR("������֮������ݽ����߳�����.20151210.v788")
		while (1)
		{	
			boost::this_thread::interruption_point();
			//���������շ�����
			int cmdDqSize=zwCfg::vecCallerCmdDq.size();
			for (int i=0;i<cmdDqSize;i++)
			{
				//�·�����ͽ������߻ظ����������̼���
				boost::mutex::scoped_lock lock(thrhid_mutex);		
				mySendCmd2Lock(i);
				myRecvFromLock(i);
			}	//end for
			Sleep(100);	//�������һ�ֱ��ĺ���ͣ100������·������ڳ�ʱ�䣻
		}
	}

	void my515UpMsgThr(void)
	{
		ZWERROR("��ATMC֮��������ϴ��߳�����.20151203.1720")
			while (1)
			{
					//�ȴ����ݽ����̲߳�����ϡ��յ������ݡ�����
					//��øö��е���������Ȩ����ʼ����
					boost::mutex::scoped_lock lock(upDeque_mutex);	
					for(int i=0;i<zwCfg::vecCallerCmdDq.size();i++)
					{
						DWORD tid=zwCfg::vecCallerCmdDq[i]->getCallerID();
						string tMsg=zwCfg::vecCallerCmdDq[i]->UploadLockResult();
						if (tMsg.size()>0)
						{
							//pushToCallBack(tMsg.c_str());
						}						
						//VLOG_IF(3,tMsg.size()>0)<<__FUNCTION__<<" �߳�ID "<<tid<<
						//	"���ϴ����б�����"<<tMsg<<endl;
					}
					Sleep(100);
			}
	}

//////////////////////////////////////////////////////////////////////////
}				//namespace zwccbthr{

