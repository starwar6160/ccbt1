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
	void pushToCallBack( const char * recvConvedXML,RecvMsgRotine pRecvMsgFun );
	deque<string> g_dqLockUpMsg;	//�����������͵Ĵ��������Ϣ����ʱ����
	bool myWaittingReturnMsg=false;	//�ȴ����ر����ڼ䲻Ҫ�·�����
	boost::condition_variable condJcLock;
	boost::timer g_LatTimer;	//�����Զ������ӳ�
	deque<jcLockMsg1512_t *> s_jcNotify;		//�·�������У��·���Ϻ��ƶ����ϴ�����
	deque<jcLockMsg1512_t *> s_jcUpMsg;		//�ϴ��������
	deque<jcLockMsg1512_t *> s_LockFirstUpMsg;				//�����ϴ�����
	map<DWORD,RecvMsgRotine> s_thrIdToPointer;	//�߳�ID���ص�����ָ���map
	////�������ϴ�����ר�õı������лص�����ָ��ļ���,���õ����ķ��������߳�;
	set<RecvMsgRotine> s_setSingleUp;	


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

	void pushToCallBack( const char * recvConvedXML,RecvMsgRotine pRecvMsgFun )
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

		if (NULL==pRecvMsgFun)
		{
			const char *err1="�ص�����ָ��Ϊ�գ��޷����ûص��������شӵ������յ��ı���";
			ZWERROR(err1);
			MessageBoxA(NULL,err1,"���ؾ���",MB_OK);
		}
		if (NULL != pRecvMsgFun && strlen(recvConvedXML)>0) {
			//���ûص�����������Ϣ��
			//20150415.1727.Ϊ��������Ҫ�󣬿����ϴ���Ϣ�������ÿ2��һ����ֹATM����
			//Sleep(2920);
			pRecvMsgFun(recvConvedXML);
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
					jcLockMsg1512_t *nItem=s_jcNotify.front();
					zwccbthr::myWaittingReturnMsg=true;	
					string sCmd=nItem->NotifyMsg;
					DWORD tid=nItem->CallerThreadID;
					string sType=nItem->NotifyType;
					nItem->bSended=true;					
					LOG(WARNING)<<"�߳�["<<tid<<"]���͸����ߵ�"<<
						sType<<"������Ϣ.������"<<endl<<sCmd;
					sts=static_cast<JCHID_STATUS>( g_jhc->SendJson(sCmd.c_str()));
					if (s_LockFirstUpMsg.size()>0 && 
						s_LockFirstUpMsg.front()->NotifyType==sType)
					{
						s_LockFirstUpMsg.pop_front();
					}
					//��������̽�����
					if (JCHID_STATUS_OK!=static_cast<JCHID_STATUS>(sts))
					{
						g_jhc->OpenJc();
					}
					s_jcUpMsg.push_back(nItem);
					VLOG_IF(3,s_jcUpMsg.size()>0)<<"s_jcUpMsg.size()="<<s_jcUpMsg.size()<<endl;
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
					string sType=jcAtmcConvertDLL::zwGetJcJsonMsgType(recvBuf);
					VLOG(1)<<"�յ����߷�����Ϣ.������"<<sType<<"������\n"<<recvBuf<<endl;
					LOG(WARNING)<<"�����·��ͷ�����Ϣ���ͷֱ��� "
						<<"�·�����="<<s_jcUpMsg.front()->NotifyType
						<<"\t��������="<<sType<<endl;

					string outXML;
					jcAtmcConvertDLL::zwJCjson2CCBxml(recvBuf,outXML);	
					//���ȵ���������λ���������ͱ��������������ʱ��ͬ��
					//����������ѭ��������Ҫ��������
					if (outXML.size()>0 &&
						(sType=="Lock_Close_Code_Lock" ||
						sType=="Lock_Time_Sync_Lock"))
					{
						ZWERROR("�����������ϴ���Ϣ���ᱻ��һ���ϴ��߳��ڲ�����һ��һ���ǰ�����ӳ��ϴ�")
						jcLockMsg1512_t *nItem=new jcLockMsg1512_t;
						nItem->CallerThreadID=0;
						nItem->UpMsg=outXML;
						nItem->NotifyType=sType;
						s_LockFirstUpMsg.push_back(nItem);
						LOG(INFO)<<__FUNCTION__<<"�����ϴ�����s_LockFirstUpMsg��С="<<s_LockFirstUpMsg.size()<<endl;
					}
						//�������������������������ѭ��һ��һ��ģ������ϴ�						
						if (outXML.size()>0 && s_jcUpMsg.size()>0 &&
							s_jcUpMsg.front()->NotifyType==sType
							&& (sType!="Lock_Close_Code_Lock" &&
							sType!="Lock_Time_Sync_Lock"))
						{
							//ZWWARN("�����ϴ�����")
							DWORD tid=s_jcUpMsg.front()->CallerThreadID;
							VLOG(3)<<"��Ϣ���ظ��߳�ID="<<tid<<endl;
							RecvMsgRotine pRecvMsgFun=
								zwccbthr::s_thrIdToPointer[tid];
							pushToCallBack(outXML.c_str(),pRecvMsgFun);
							s_jcUpMsg.pop_front();
							VLOG(3)<<"��ͨ�ϴ����д�Сs_jcUpMsg.size()="<<s_jcUpMsg.size()<<endl;
						}
				}	//if (strlen(recvBuf)>0)
			}while(strlen(recvBuf)>0);
			
		}	//�շ�thrhid_mutex����			
			Sleep(100);	//�������һ�ֱ��ĺ���ͣ100������·������ڳ�ʱ�䣻
			VLOG(4)<<__FUNCTION__<<"\tSleep 100 ms"<<endl;
		}

	}

	void my515UpMsgThr(void)
	{
		ZWERROR("��ATMC֮��������ϴ��߳�����.20151215")
			while (1)
			{		
				Sleep(1000);
				//VLOG(3)<<__FUNCTION__<<"RUNNING"<<endl;
				//�ȴ����ݽ����̲߳�����ϡ��յ������ݡ�����
				//��øö��е���������Ȩ����ʼ����
				{
					boost::mutex::scoped_lock lock(thrhid_mutex);				
					//ֻ�е��ȴ�����ϴ�����Ϣ���Ѿ��ϴ���Ϻ�
					// ���ϴ��ñ��ӳ��ϴ��ı����������һ��һ��
					if (s_LockFirstUpMsg.size()>0 && s_jcUpMsg.size()==0)
					{
						string &strSingleUp=s_LockFirstUpMsg.front()->UpMsg;					
						set<RecvMsgRotine>::iterator it; //����ǰ������� 
						//���������ϴ����ķ���ÿһ���̵߳Ļص�����
						for (it=zwccbthr::s_setSingleUp.begin();
							it!=zwccbthr::s_setSingleUp.end();it++)
						{
							RecvMsgRotine pCallBack=(*it);
							LOG(WARNING)<<"�ӳ��ϴ����ĵ��ص�������ַ"<<std::hex<<pCallBack<<endl;
							pushToCallBack(strSingleUp.c_str(),pCallBack);
						}
						//s_LockFirstUpMsg.pop_front();
						//LOG(INFO)<<__FUNCTION__<<"�����ϴ�����s_SingleUpMsg��С="<<s_LockFirstUpMsg.size()<<endl;
					}
				}
			}
	}

//////////////////////////////////////////////////////////////////////////
}				//namespace zwccbthr{

