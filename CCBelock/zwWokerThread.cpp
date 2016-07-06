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
	deque<jcLockMsg1512_t *> s_jcNotify;		//�·�������У��·���Ϻ��ƶ����ϴ�����
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
			const char *err1="�ص�����ָ��Ϊ�գ��޷����ûص��������شӵ������յ��ı���20151221";
			ZWERROR(err1);
			MessageBoxA(NULL,err1,"���ؾ���",MB_OK);
		}
		if (NULL != pRecvMsgFun && strlen(recvConvedXML)>0) {
			//���ûص�����������Ϣ��
			//20150415.1727.Ϊ��������Ҫ�󣬿����ϴ���Ϣ�������ÿ2��һ����ֹATM����
			//Sleep(2920);
			pRecvMsgFun(recvConvedXML);
			LOG(ERROR)<<"��λ���յ� "<<recvConvedXML<<endl;
			VLOG_IF(4,strlen(recvConvedXML)>0)<<"�ص������յ���������\n"<<recvConvedXML<<endl;
#ifdef _DEBUG401
			
			VLOG(4)<<"�ɹ��Ѵ����߽��յ������ݴ��ݸ��ص�����\n";
#endif // _DEBUG401
		}
	}
	
	//�Ƿ��������������ͱ��������ķ���ѭ������
	bool myIsMsgFromLockFirstUp(const string &jcMsg)
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

	boost::mutex tms_mutex;

	void myDbgPrintMs(const char *myFuncName)
	{
		boost::mutex::scoped_lock lock(tms_mutex);	
		double cur=zwGetMs();		
	}

	void my706LockRecvThr(void)
	{
		ZWERROR("������֮������ݽ����߳�����.20151231.v837Base834")
		const int BLEN = 1024;
		char recvBuf[BLEN];					
		using zwccbthr::s_jcNotify;
		while (1)
		{	
			boost::this_thread::interruption_point();
			{
				boost::mutex::scoped_lock lock(thrhid_mutex);		
				JCHID_STATUS sts=JCHID_STATUS_FAIL;		
				if (s_jcNotify.size()>0)
				{
					jcLockMsg1512_t *nItem=s_jcNotify.front();
					string sCmd=nItem->getNotifyMsg();
					VLOG(4)<<"�·������ߵ���Ϣ������"<<sCmd<<endl;
					sts=static_cast<JCHID_STATUS>( g_jhc->SendJson(sCmd.c_str()));
					//��������̽�����
					if (JCHID_STATUS_OK!=static_cast<JCHID_STATUS>(sts))
					{
						g_jhc->OpenJc();
					}
					s_jcNotify.pop_front();
				}	//if (s_jcNotify.size()>0)
				//��ȡ����ֵ
				do 
				{
					memset(recvBuf,0,BLEN);
					sts=static_cast<JCHID_STATUS>(g_jhc->RecvJson(recvBuf,BLEN));
					if (strlen(recvBuf)>0)
					{					
					VLOG(3)<<"�յ����߷�����Ϣ1607= "<<recvBuf<<endl;
					string upType=jcAtmcConvertDLL::zwGetJcJsonMsgType(recvBuf);
					string outXML;
					jcAtmcConvertDLL::zwJCjson2CCBxml(recvBuf,outXML);
						if (outXML.size()>0)
						{
							RecvMsgRotine pRecvMsgFun=zwccbthr::s_CallBack;
							pushToCallBack(outXML.c_str(),pRecvMsgFun);
							//if (s_jcNotify.size()>0)
							//{
							//jcLockMsg1512_t *nItem=s_jcNotify.front();
							//string downType=jcAtmcConvertDLL::zwGetJcJsonMsgType(nItem->getNotifyMsg().c_str());
							//VLOG_IF(3,downType!=upType)<<"downType="<<downType<<" upType="<<upType<<endl;
							////s_jcNotify.pop_front();
							//}
						}					
					}	//if (strlen(recvBuf)>0)					
				} while (strlen(recvBuf)>0);
			}
		}	//end of while(1)

	}



//////////////////////////////////////////////////////////////////////////
}				//namespace zwccbthr{

