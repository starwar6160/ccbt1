//2016/7/6 17:20:05 [������] ��ǰATMC DLL���վ��շ��̴߳����߼���
//	Notify�����·����ģ���һ��������thrhid_mutex�������շ��̻߳��⣬�������շ�������
//	�ѱ��ļ����·�����s_jcNotify��ͬʱ����һ����־λs_bPendingNotify������־��ǰ��
//	�Ѿ��·��������ڻ�����δ�����·�����s_jcNotify�ı���(���ڱ����·����ĺ�����
//	�����ϴ�����ײ������ϲ�ATMCϵͳ�����Ľ�������)
//	�����շ��߳�my706LockRecvThrѭ�������ģ����·�����s_jcNotifyȡ��һ�����ģ�
//	���͸����ߣ�Ȼ�󿴿��ñ��������ǲ��������������ͱ��ĵ�ȷ�ϱ��ģ�����ǣ�
//	?�Ͳ�ȥ��ȡ���ߵķ��ر����ˣ�
//	������ǣ����ȡ���߷��ر��ģ���ȡ���ر�������upType�������ر���ת��Ϊ
//	outXML����Ҫ���XML��ʽ��ΪoutXML����ȡ�·����ĵı�������downType��
//	���upType��downType���˵������һ�������·����ĵ�ƥ�䷵�ر���(�˷ɱ�֤
//	����˳����ͷ��ر��ģ����Ҳ��ᶪʧ����),����ͨ���ص��������ظ�
//	�ϲ�ATMCӦ�ã�����˵������һ�������������ͱ��ģ���Ҫ�ӳ��ϴ��������
//	һ��һ������ϲ�Ӧ�á����Ľ�������",���Խ���ŵ��������ϴ�����s_jcLockToC���棻
//	��ȡ��Ϸ��ر��ĺ��·�����s_jcNotify���Ե����ղŴ�����ϵı����ˣ�
//	Ȼ��ʼ�ϴ������ϴ����ģ��������·�����Ϊ�գ����һ���·���Ϣ�Ļ�Ӧ��Ϣ
//	�Ѿ��ϴ������500���룬����Notify��������û�������߳�����Ե�ʵȴ�����
//	�·����е��·����ģ���ô�Ϳ���ͨ���ص������ѵ����ϴ����Ĵ��ݸ��ϲ�Ӧ���ˣ�

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

#define MYFD1	std::setprecision(0)<<std::setiosflags(std::ios::fixed)

jcHidDevice *g_jhc=NULL;	//ʵ�ʵ�HID�豸�����

namespace zwccbthr {	
	//���и��Ľӿڣ�û���������Ӳ����ĵط���Ҳ����˵����ȫ���Զ˿ڣ��ֻ��Ǵ������ļ���ȡ
	boost::mutex thrhid_mutex;
	void pushToCallBack( const char * recvConvedXML,RecvMsgRotine pRecvMsgFun );	
	deque<jcLockMsg1512_t *> s_jcNotify;		//�·��������
	deque<jcLockMsg1512_t *> s_dbgMatchNotify;		//�·�������и���Ʒ�����ڵ���ƥ��
	RecvMsgRotine s_CallBack=NULL;
	double s_LastNormalMsgUpTimeMs=0.0;	//���һ������ѭ�������ϴ���ʱ�䣬�������
	////�������ϴ�����ר�õı������лص�����ָ�������,���õ����ķ��������߳�;
	vector <RecvMsgRotine> s_vecSingleUp;		


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
		static double lastUpMsg=0;	//���һ���ϴ����ĵ�ʱ��������ڼ������ļ��
		static int nUpCount=0;	//�ڶ������ϴ�����
		static int nExceedCount=0;	//����2�����ļ�����
		static double nExceedMsTotal=0;	//����2�����Ĺ��ж��ٺ���
		static double nExceedMaxMs=0;	//����2�����������ٺ���
		assert(NULL!=recvConvedXML);
		assert(strlen(recvConvedXML)>0);
		if (NULL==recvConvedXML || strlen(recvConvedXML)==0)
		{
			ZWERROR("�յ������߷�������Ϊ�գ��޷�����������Ϣ���ص�����");
			return;
		}		


		if (strlen(recvConvedXML)>0){		
			//XML��ͷ�Ĺ̶�����38���ַ����������һ����ǩ�����Լ����źϼ�4���ַ�
			assert(strlen(recvConvedXML) > 42);
		}

		if (NULL==pRecvMsgFun)
		{
			const char *err1="�ص�����ָ��Ϊ�գ��޷����ûص��������شӵ������յ��ı���20151221";
			ZWERROR(err1);
			MessageBoxA(NULL,err1,"���ؾ���",MB_OK);
		}
		if (NULL != pRecvMsgFun && strlen(recvConvedXML)>0) {
			//���ûص�����������Ϣ��
			//20150415.1727.Ϊ��������Ҫ�󣬿����ϴ���Ϣ�������ÿ2��һ����ֹATM����
			//Sleep(2920);
			pRecvMsgFun(recvConvedXML);
			LOG(INFO)<<"��λ���յ� "<<recvConvedXML<<endl;
			string upMsgType= jcAtmcConvertDLL::zwGetJcxmlMsgType(recvConvedXML);
			if (s_dbgMatchNotify.size()>0)
			{
				for(size_t i=0;i<s_dbgMatchNotify.size();i++)
				{
					
					string downType=s_dbgMatchNotify.front()->getNotifyNumType();
					//LOG_IF(INFO,downType==upMsgType)<<"���б�������"<<downType<<"�������б�������"<<upMsgType<<endl;
					LOG_IF(ERROR,downType!=upMsgType)<<"���б�������"<<downType<<"��ƥ�����б�������"<<upMsgType<<endl;
					s_dbgMatchNotify.pop_front();
					if (downType==upMsgType)
					{
						break;
					}
				}				
			}

			nUpCount++;
			double curMs=zwccbthr::zwGetMs();
			double diffMs=curMs-lastUpMsg;
			
			if (diffMs>2800 && lastUpMsg>0)
			{
				nExceedCount++;
				nExceedMsTotal+=diffMs;
				LOG_IF(WARNING,diffMs>nExceedMaxMs)<<"���ļ���쳣�ﵽ"<<diffMs<<"����"<<endl;
				if (diffMs>nExceedMaxMs)
				{
					nExceedMaxMs=diffMs;
				}
				
			}
			LOG_IF(WARNING,(nUpCount%20==0))<<"����֮����ʱ��"<<diffMs<<"���룬��ǰ��"<<nUpCount<<"������"<<endl;				
			LOG_IF(WARNING,(nUpCount%3==0) && nExceedMaxMs>100)<<"���"<<nExceedMaxMs<<"���� ƽ��"<<nExceedMsTotal/(nExceedCount+0.001)<<"����"<<endl;
			lastUpMsg=curMs;
		}
		for (size_t i=0;i<s_dbgMatchNotify.size();i++)
		{
			LOG(WARNING)<<"s_dbgMatchNotify."<<s_dbgMatchNotify[i]->getNotifyType()<<endl;
		}
	}
	
	//�Ƿ��������������ͱ��������ķ���ѭ������
	bool myIsJsonMsgFromLockFirstUp(const string &jcMsg)
	{
		if(						
			jcMsg=="Lock_Open_Ident"	  ||
			jcMsg=="Lock_Close_Code_Lock" ||
			jcMsg=="Lock_Time_Sync_Lock" ||
			jcMsg== "Lock_Alarm_Info")
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	double zwGetUs(void)
	{
		LARGE_INTEGER frq,cur;
		QueryPerformanceCounter(&cur);
		QueryPerformanceFrequency(&frq);
		return 1.0e6*cur.QuadPart/frq.QuadPart;
	}

	double zwGetMs(void)
	{
		return zwGetUs()/1000.0;
	}


	void my706LockRecvThr(void)
	{
		ZWERROR("������֮������ݽ����߳�����.20160713.v867")
		deque<jcLockMsg1512_t *> s_jcLockToC;		//���ߵ����ϴ�����
		double s_lastNotifyMs=0;
		const int BLEN = 1024;
		char recvBuf[BLEN];					
		using zwccbthr::s_jcNotify;
		while (1)
		{	
			boost::this_thread::interruption_point();
			{
				boost::mutex::scoped_lock lock(thrhid_mutex);		
//////////////////////////////�ȴ������ϴ�����////////////////////////////////////////////
				double curMs=zwccbthr::zwGetMs();
				//LOG_IF(WARNING,curMs-s_lastNotifyMs>2000)<<"curMs-s_lastNotifyMs="<<curMs-s_lastNotifyMs<<"ms"<<endl;
				LOG_IF(WARNING,s_jcLockToC.size()>0)<<"s_jcLockToC.size()="<<s_jcLockToC.size()
					<<" s_jcNotify.size()="<<s_jcNotify.size()
					<<" curMs-s_lastNotifyMs="<<(curMs-s_lastNotifyMs)<<endl;		
				//���·������Ѿ�Ϊ�գ����Ҵ�ʱ��Ϊ�߳�����Ե����ʱ���ܼ����µ���Ϣ���������һ�������·���Ϣ
				//�Ļ�Ӧ��Ϣ�Ѿ��ϴ���500���룬����û�еȴ������·����е���Ϣ������һ������ʱ�̿����ϴ���Ϣ�ˣ�
				LOG_IF(WARNING,s_jcLockToC.size()>0)<<"s_jcLockToC.size()="<<s_jcLockToC.size()<<endl;
				while (s_jcLockToC.size()>0)
				{
					string sUpMsg;
					jcAtmcConvertDLL::zwJCjson2CCBxml(s_jcLockToC.front()->getNotifyMsg(),sUpMsg);					
					double curMs=zwccbthr::zwGetMs();
					double diffMs= curMs- s_jcLockToC.front()->getNotifyMs();
					LOG(WARNING)<<"�����ϴ������ӳ���"<<diffMs<<"������ϴ�"<<sUpMsg<<endl;					
					RecvMsgRotine pRecvMsgFun=zwccbthr::s_CallBack;
					pushToCallBack(sUpMsg.c_str(),pRecvMsgFun);
					s_jcLockToC.pop_front();
				}			
//////////////////////////////////////////////////////////////////////////
				JCHID_STATUS sts=JCHID_STATUS_FAIL;		
				if (s_jcNotify.size()>0)
				{
					jcLockMsg1512_t *nItem=s_jcNotify.front();					
					//��¼�����·���ʱ��
					nItem->setInitNotifyMs();
					//�������ڵ���ƥ��Ķ���
					s_dbgMatchNotify.push_back(nItem);
					string sCmd=nItem->getNotifyMsg();
					
					LOG(INFO)<<"�·������ߵ���Ϣ������"<<sCmd<<endl;
					sts=static_cast<JCHID_STATUS>( g_jhc->SendJson(sCmd.c_str()));
					//��������̽�����
					if (JCHID_STATUS_OK!=static_cast<JCHID_STATUS>(sts))
					{
						g_jhc->OpenJc();
					}
					//����������������ͱ��ĵķ���ȷ�ϱ��ģ���ô�·���Ϻ��ö�ȡ���ߵķ��ر���
					if(true==myIsJsonMsgFromLockFirstUp(nItem->getNotifyType())) 
					{
						continue;
					}
				}	//if (s_jcNotify.size()>0)
				//��ȡ����ֵ
				double msgReadStart=zwccbthr::zwGetMs();
				do 
				{
					VLOG(4)<<"��ȡ����ֵѭ��������"<<endl;
					memset(recvBuf,0,BLEN);
					sts=static_cast<JCHID_STATUS>(g_jhc->RecvJson(recvBuf,BLEN));
					if (strlen(recvBuf)>0)
					{										
					jcLockMsg1512_t *nUpItem=new jcLockMsg1512_t(recvBuf);
					string upType=jcAtmcConvertDLL::zwGetJcJsonMsgType(recvBuf);
					if (myIsJsonMsgFromLockFirstUp(upType)==true)
					{
						LOG(WARNING)<<"�յ����ߵ����ϴ���Ϣ "<<recvBuf<<endl;
					}
					else{
						VLOG(3)<<"�յ����߷�����Ϣ1607= "<<recvBuf<<endl;
					}
					
					string outXML;
					jcAtmcConvertDLL::zwJCjson2CCBxml(recvBuf,outXML);
					VLOG(3)<<"outXML.size()="<<outXML.size()<<"s_jcNotify.size()="<<s_jcNotify.size()<<endl;
						if (outXML.size()>0)
						{																					
							if (s_jcNotify.front()>0 && s_jcNotify.front()->matchResponJsonMsg(recvBuf)==true)
							{
								VLOG(3)<<"�����б���ƥ����ȷ"<<endl;
								RecvMsgRotine pRecvMsgFun=zwccbthr::s_CallBack;
								pushToCallBack(outXML.c_str(),pRecvMsgFun);
								s_lastNotifyMs=zwccbthr::zwGetMs();
								VLOG(3)<<"��Ϣ"<<upType<<"����ʱ��"
									<<s_lastNotifyMs-s_jcNotify.front()->getNotifyMs()<<"����"<<endl;
								if (s_jcNotify.size()>0){
									s_jcNotify.pop_front();
								}
								break;
							}							
							if (myIsJsonMsgFromLockFirstUp(upType)==true)
							{	
								LOG(WARNING)<<" upType="<<upType<<"�ñ��Ľ��������һ�������ӳ��ϴ�"<<endl;
								//�����������ͱ��ģ����ҷŵ������Ķ��������д����ӳٴ���
								nUpItem->setInitNotifyMs();
								s_jcLockToC.push_back(nUpItem);
								continue;
							}							
																												
						}		
					}	//if (strlen(recvBuf)>0)					
					double curMs=zwccbthr::zwGetMs();
					if (curMs-msgReadStart>1800 || strlen(recvBuf)>0)
					{
						break;
					}
				} while (1);
					
			}//boost::mutex::scoped_lock lock(thrhid_mutex);		
			//�ڻ�������Χ��������һ��ʱ���Notify����µı��ĵ��·�����
			Sleep(100);
		}	//end of while(1)

	}



//////////////////////////////////////////////////////////////////////////
}				//namespace zwccbthr{

