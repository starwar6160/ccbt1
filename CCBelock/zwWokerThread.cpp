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
	deque<string> g_dqLockUpMsg;	//�����������͵Ĵ��������Ϣ����ʱ����
	bool myWaittingReturnMsg=false;	//�ȴ����ر����ڼ䲻Ҫ�·�����
	boost::condition_variable condJcLock;
	boost::timer g_LatTimer;	//�����Զ������ӳ�
	deque<jcLockMsg1512_t *> s_jcNotify;		//�·�������У��·���Ϻ��ƶ����ϴ�����
	deque<jcLockMsg1512_t *> s_jcUpMsg;		//�ϴ��������
	deque<jcLockMsg1512_t *> s_LockFirstUpMsg;				//�����ϴ�����
	//map<DWORD,RecvMsgRotine> s_thrIdToPointer;	//�߳�ID���ص�����ָ���map
	RecvMsgRotine s_CallBack=NULL;
	double s_zwProcStartMs=0.0;			//����������ʱ������������
	double s_LastNormalMsgUpTimeMs=0.0;	//���һ������ѭ�������ϴ���ʱ�䣬�������
	bool s_bLockInitDur=false;	//�Ƿ������߳�ʼ���ڼ�

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
			ZWERROR(recvConvedXML)
			VLOG_IF(4,strlen(recvConvedXML)>0)<<"�ص������յ���������\n"<<recvConvedXML<<endl;
#ifdef _DEBUG401
			
			VLOG(4)<<"�ɹ��Ѵ����߽��յ������ݴ��ݸ��ص�����\n";
#endif // _DEBUG401
		}
	}
	
	//�Ƿ��������������ͱ��������ķ���ѭ������
	bool myIsMsgFromLockFirstUp(const string &jcMsg)
	{
		if(jcMsg=="Lock_Close_Code_Lock" ||
			jcMsg=="Lock_Time_Sync_Lock" ||
			jcMsg=="Lock_Open_Ident"	  ||
			jcMsg== "Lock_Alarm_Info")
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool myIsLockInitMsg(const string &jcMsg)
	{
		if ("Lock_Secret_Key"==jcMsg 
	||	"Lock_System_Init"==jcMsg)
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
#ifdef _DEBUG
VLOG(3)<<myFuncName<<" "<<MYFD1<<"ZWHTms="<<(cur-s_zwProcStartMs)<<endl;
#else
VLOG(4)<<myFuncName<<" "<<MYFD1<<"ZWHTms="<<(cur-s_zwProcStartMs)<<endl;
#endif // _DEBUG
		
	}


	void my515LockRecvThr(void)
	{
		
		ZWERROR("������֮������ݽ����߳�����.20151231.v835")
		const int BLEN = 1024;
		char recvBuf[BLEN];			
		using zwccbthr::s_jcNotify;
		zwccbthr::s_zwProcStartMs=zwGetMs();

		//Sleep(1300);	//�����������б��ĵ��߳�����ǰ���ʵ��ӳ�
		while (1)
		{	
			boost::this_thread::interruption_point();
			{
			boost::mutex::scoped_lock lock(thrhid_mutex);		
			myDbgPrintMs("RECV");
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
					if ("Lock_Secret_Key"==sType)
					{
						s_bLockInitDur=true;
					}
					sts=static_cast<JCHID_STATUS>( g_jhc->SendJson(sCmd.c_str()));
					//��������̽�����
					if (JCHID_STATUS_OK!=static_cast<JCHID_STATUS>(sts))
					{
						g_jhc->OpenJc();
					}
					s_jcUpMsg.push_back(nItem);
					VLOG_IF(3,s_jcUpMsg.size()>0)<<"s_jcUpMsg.size()="<<s_jcUpMsg.size()<<endl;
					s_jcNotify.pop_front();
					s_LastNormalMsgUpTimeMs=zwGetMs();
					//VLOG(3)<<"s_LastNormalMsgUpTimeMs="<<MYFD1<<s_LastNormalMsgUpTimeMs<<endl;
					myDbgPrintMs("s_LastNormalMsgUpTimeMsSend");
				}
			}
			do{
				//���ǵ��п������ߵ���������Ϣ����һ���·���Ϣ�ж���
				//������Ϣ�����Զ��ȡ����ֱ����������ϢΪֹ
				memset(recvBuf,0,BLEN);
				VLOG(4)<<"�������ݵ�RecvJson֮ǰ"<<endl;
				sts=static_cast<JCHID_STATUS>(g_jhc->RecvJson(recvBuf,BLEN));	
				int lRecvBytes=strlen(recvBuf);
				VLOG_IF(4,lRecvBytes>0)<<"RecvJson�յ�"<<lRecvBytes<<"�ֽڵ�����"<<endl;
				if (strlen(recvBuf)>0)
				{
					VLOG(4)<<"�յ����߷�����Ϣ= "<<recvBuf<<endl;
					assert(strlen(recvBuf)>0);
					string sType=jcAtmcConvertDLL::zwGetJcJsonMsgType(recvBuf);
					VLOG(1)<<"�յ����߷�����Ϣ.������"<<sType<<"������\n"<<recvBuf<<endl;
					if (s_jcUpMsg.size()>0)
					{
						LOG_IF(WARNING,s_jcUpMsg.front()->NotifyType!=sType)
							<<"�����·��ͷ�����Ϣ���Ͳ�ͬ�����߷ֱ��� "
							<<"�·�����="<<s_jcUpMsg.front()->NotifyType
							<<"\t��������="<<sType<<endl;
					}

					string outXML;
					jcAtmcConvertDLL::zwJCjson2CCBxml(recvBuf,outXML);	
					assert(outXML.size()>0);
					//���ȵ���������λ���������ͱ��������������ʱ��ͬ��
					// �������߷�����֤��,���߷��ͱ�����Ϣ
					//�⼸������ѭ��������Ҫ��������
					if (outXML.size()>0 && !myIsLockInitMsg(sType))
					{
						ZWERROR("�����������ϴ���Ϣ���ᱻ��һ���ϴ��߳��ڲ�����һ��һ���ǰ�����ӳ��ϴ�")
						jcLockMsg1512_t *nItem=new jcLockMsg1512_t;
						nItem->CallerThreadID=0;
						nItem->UpMsg=outXML;
						nItem->NotifyType=sType;
						s_LockFirstUpMsg.push_back(nItem);
						LOG(INFO)<<__FUNCTION__<<"�����ϴ�����s_LockFirstUpMsg��С="<<s_LockFirstUpMsg.size()<<endl;
						if (s_jcUpMsg.size()>0)
						{
							s_jcUpMsg.pop_front();
						}						
					}
						//�����⼸���������ⶼ�Ƿ�������ѭ��һ��һ��ģ������ϴ�		
						// ��������ͷ����������Ժ��ݲ���������б������Ͷ�Ӧ��
						// ��һ������Ŀ��Ӧ��������ͷ�������ѭ�����׷ֿ������߳�������Ӧ�þ������
						if (outXML.size()>0 && s_jcUpMsg.size()>0 
							&& myIsLockInitMsg(sType))
						{
							//ZWWARN("�����ϴ�����")
							DWORD tid=s_jcUpMsg.front()->CallerThreadID;
							assert(tid>0);
							VLOG(3)<<"��Ϣ���ظ��߳�ID="<<tid<<endl;
							RecvMsgRotine pRecvMsgFun=zwccbthr::s_CallBack;
							assert(pRecvMsgFun!=NULL);
								//zwccbthr::s_thrIdToPointer[tid];
							pushToCallBack(outXML.c_str(),pRecvMsgFun);
							assert(s_jcUpMsg.size()>0);
							s_jcUpMsg.pop_front();
							VLOG(3)<<"��ͨ�ϴ����д�Сs_jcUpMsg.size()="<<s_jcUpMsg.size()<<endl;
							s_LastNormalMsgUpTimeMs=zwGetMs();
							//VLOG(3)<<"s_LastNormalMsgUpTimeMs="<<MYFD1<<s_LastNormalMsgUpTimeMs<<endl;
							myDbgPrintMs("s_LastNormalMsgUpTimeMs_Recv");
							if ("Lock_System_Init"==sType)
							{
								s_bLockInitDur=false;
							}
						}
				}	//if (strlen(recvBuf)>0)
			}while(strlen(recvBuf)>0);
			
		}	//�շ�thrhid_mutex����			
			Sleep(100);	//�������һ�ֱ��ĺ���ͣ100������·������ڳ�ʱ�䣻
			//VLOG(3)<<__FUNCTION__<<" ��Ҫ�������շ��߳������У�ѭ��ĩβ"<<endl;
		}	//end while

	}

	void my515UpMsgThr(void)
	{
		//����ѭ������û��Ҫ��ô�����ͣ������ӳ�300��������1-2���������̻߳������ǿ��Ե�
		Sleep(300);
		ZWERROR("��ATMC֮��������ϴ��߳�����.20151227")
			while (1)
			{
				{
					boost::this_thread::interruption_point();
					boost::mutex::scoped_lock lock(thrhid_mutex);	
					//ֻ�е��ȴ�����ϴ�����Ϣ���Ѿ��ϴ���Ϻ�
					// ���ϴ��ñ��ӳ��ϴ��ı����������һ��һ��
					//�������ԣ����÷��ͺͽ��ղ�����ɺ�����һ������׷�ʱ����ķ������ϴ��߳��ڸ�ʱ��֮�����
					// �ܲ�����һ��һ���أ�300�Ļ�ͦ���صĴ�����ţ�600�Ļ������������ţ�900�Ļ�����һ��û������
					// ���Ҿ�����˫�����ң�2000��Ҳ����2�룬2����ӳٶ����˵ĸо������������ںܿ�ģ�Ҳ������
					// Τ�����˵���ӳ�5���ȥһ���ƽ�ָ�0.6֮���ֵ���Ҿ��趨Ϊ2���ӳٰɡ�
#ifdef _DEBUG
					if(zwGetMs()-s_LastNormalMsgUpTimeMs<900)
					{
						myDbgPrintMs("my515UpMsgThrTest1228NOTEXEC");
					}
					else
					{
						static int lUpTimes=1;
						//myDbgPrintMs("my515UpMsgThrTest1228UPLOADED");
						LOG_IF(WARNING,lUpTimes<=3)<<MYFD1<<"my515UpMsgThrTest1228UPLOADED\tms="<<zwGetMs()-s_zwProcStartMs
							<<"\tlUpTimes="<<(lUpTimes++)<<endl;
					}
#endif // _DEBUG

					if (s_LockFirstUpMsg.size()>0)
					{				
						string &strSingleUp=s_LockFirstUpMsg.front()->UpMsg;	
						assert(strSingleUp.size()>0);
						
						string sType=jcAtmcConvertDLL::zwGetJcxmlMsgType(strSingleUp.c_str());
						assert(sType.size()>0);
						int delayMs=2000;
						//���ͱ�������Ҫ���죬����Ĭ��2���ӳ�
						if ("Lock_Open_Ident"==sType)
						{
							delayMs=200;
						}
						VLOG(4)<<__FUNCTION__<<" sType="<<sType<<"\tdelayMs="<<delayMs<<endl;
						//���һ������ѭ���ı��ķ������߽��ղ�����ʱ���
						//ֻ������2��û���˲���������ѭ�����ĵ��ϴ��������һ��һ��
						// ������ʱ���Ϲ�ȥ������һ������ѭ���ķ��ر������
						//��λ�������Ľ������󡱣�
						if((zwGetMs()-s_LastNormalMsgUpTimeMs<delayMs) ||
							true==zwccbthr::s_bLockInitDur)
						{
							myDbgPrintMs("my515UpMsgThr��û���ϴ��ӳ����ޣ���continue");
							continue;
						}

						set<RecvMsgRotine>::iterator it; //����ǰ������� 
						//���������ϴ����ķ���ÿһ���̵߳Ļص�����
						int icc=1;
						RecvMsgRotine pOld=NULL;
						assert(zwccbthr::s_vecSingleUp.size()>0);
						myDbgPrintMs("  UPLOAD");
						for (int i=0;i<zwccbthr::s_vecSingleUp.size();i++)
						{							
							RecvMsgRotine pCallBack=zwccbthr::s_vecSingleUp[i];
							assert(NULL!=pCallBack);
							if (pCallBack!=pOld)
							{

								pushToCallBack(strSingleUp.c_str(),pCallBack);
								pOld=pCallBack;
								LOG(WARNING)<<"�ӳ��ϴ����ĵ��ص�������ַ"<<std::hex<<pCallBack
									<<"\t��"<<(icc++)<<"��"<<endl;
								VLOG(4)<<"strSingleUp="<<strSingleUp<<endl;
							}							
						}
						if (s_LockFirstUpMsg.size()>0)
						{
							s_LockFirstUpMsg.pop_front();
						}
					}//end if (s_LockFirstUpMsg.size()>0)
				}	//end mutex
				Sleep(100);	//mutex���Ʒ�Χ������Sleepһ�£��ó����������շ��߳�
				}//end while
				//VLOG(3)<<__FUNCTION__<<"\t�����ϴ��߳������У��ս���һ��ѭ��"<<endl;
	}

//////////////////////////////////////////////////////////////////////////
}				//namespace zwccbthr{

