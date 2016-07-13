//2016/7/6 17:20:05 [星期三] 当前ATMC DLL的收据收发线程处理逻辑：
//	Notify函数下发报文，用一个互斥锁thrhid_mutex和数据收发线程互斥，避免在收发过程中
//	把报文加入下发队列s_jcNotify，同时还有一个标志位s_bPendingNotify用来标志当前有
//	已经下发但是由于互斥尚未加入下发队列s_jcNotify的报文(用于避免下发报文和锁具
//	单向上传报文撞车造成上层ATMC系统“报文解析错误”)
//	数据收发线程my706LockRecvThr循环处理报文，从下发队列s_jcNotify取出一条报文，
//	发送给锁具，然后看看该报文类型是不是锁具主动上送报文的确认报文，如果是，
//	?就不去读取锁具的返回报文了；
//	如果不是，则读取锁具返回报文，获取返回报文类型upType，将返回报文转换为
//	outXML建行要求的XML格式作为outXML，获取下发报文的报文类型downType；
//	如果upType和downType相等说明这是一条正常下发报文的匹配返回报文(潘飞保证
//	按照顺序处理和返回报文，而且不会丢失报文),则将其通过回调函数返回给
//	上层ATMC应用；否则说明这是一条锁具主动上送报文，需要延迟上传以免打破
//	一问一答，造成上层应用“报文解析错误",所以将其放到单独的上传队列s_jcLockToC里面；
//	读取完毕返回报文后，下发队列s_jcNotify可以弹出刚才处理完毕的报文了；
//	然后开始上传单向上传报文，条件是下发队列为空，最后一条下发消息的回应消息
//	已经上传后过了500毫秒，而且Notify函数那里没有由于线程锁的缘故等待进入
//	下发队列的下发报文，那么就可以通过回调函数把单向上传报文传递给上层应用了；

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

jcHidDevice *g_jhc=NULL;	//实际的HID设备类对象

namespace zwccbthr {	
	//建行给的接口，没有设置连接参数的地方，也就是说，完全可以端口，抑或是从配置文件读取
	boost::mutex thrhid_mutex;
	void pushToCallBack( const char * recvConvedXML,RecvMsgRotine pRecvMsgFun );	
	deque<jcLockMsg1512_t *> s_jcNotify;		//下发命令队列
	deque<jcLockMsg1512_t *> s_dbgMatchNotify;		//下发命令队列复制品，用于调试匹配
	RecvMsgRotine s_CallBack=NULL;
	double s_LastNormalMsgUpTimeMs=0.0;	//最后一次正常循环报文上传的时间，毫秒计算
	////供单向上传报文专用的保存所有回调函数指针的向量,好让单向报文发给所有线程;
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
		static double lastUpMsg=0;	//最后一条上传报文的时间戳，用于检测过长的间隔
		static int nUpCount=0;	//第多少条上传报文
		static int nExceedCount=0;	//超过2秒间隔的计数器
		static double nExceedMsTotal=0;	//超过2秒间隔的共有多少毫秒
		static double nExceedMaxMs=0;	//超过2秒间隔的最大多少毫秒
		assert(NULL!=recvConvedXML);
		assert(strlen(recvConvedXML)>0);
		if (NULL==recvConvedXML || strlen(recvConvedXML)==0)
		{
			ZWERROR("收到的锁具返回内容为空，无法返回有用信息给回调函数");
			return;
		}		


		if (strlen(recvConvedXML)>0){		
			//XML开头的固定内容38个字符，外加起码一个标签的两对尖括号合计4个字符
			assert(strlen(recvConvedXML) > 42);
		}

		if (NULL==pRecvMsgFun)
		{
			const char *err1="回调函数指针为空，无法调用回调函数返回从电子锁收到的报文20151221";
			ZWERROR(err1);
			MessageBoxA(NULL,err1,"严重警告",MB_OK);
		}
		if (NULL != pRecvMsgFun && strlen(recvConvedXML)>0) {
			//调用回调函数传回信息，
			//20150415.1727.为了万敏的要求，控制上传消息速率最多每2秒一条防止ATM死机
			//Sleep(2920);
			pRecvMsgFun(recvConvedXML);
			LOG(INFO)<<"上位机收到 "<<recvConvedXML<<endl;
			string upMsgType= jcAtmcConvertDLL::zwGetJcxmlMsgType(recvConvedXML);
			if (s_dbgMatchNotify.size()>0)
			{
				for(size_t i=0;i<s_dbgMatchNotify.size();i++)
				{
					
					string downType=s_dbgMatchNotify.front()->getNotifyNumType();
					//LOG_IF(INFO,downType==upMsgType)<<"下行报文类型"<<downType<<"等于上行报文类型"<<upMsgType<<endl;
					LOG_IF(ERROR,downType!=upMsgType)<<"下行报文类型"<<downType<<"不匹配上行报文类型"<<upMsgType<<endl;
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
				LOG_IF(WARNING,diffMs>nExceedMaxMs)<<"报文间隔异常达到"<<diffMs<<"毫秒"<<endl;
				if (diffMs>nExceedMaxMs)
				{
					nExceedMaxMs=diffMs;
				}
				
			}
			LOG_IF(WARNING,(nUpCount%20==0))<<"报文之间间隔时间"<<diffMs<<"毫秒，当前第"<<nUpCount<<"条报文"<<endl;				
			LOG_IF(WARNING,(nUpCount%3==0) && nExceedMaxMs>100)<<"最大"<<nExceedMaxMs<<"毫秒 平均"<<nExceedMsTotal/(nExceedCount+0.001)<<"毫秒"<<endl;
			lastUpMsg=curMs;
		}
		for (size_t i=0;i<s_dbgMatchNotify.size();i++)
		{
			LOG(WARNING)<<"s_dbgMatchNotify."<<s_dbgMatchNotify[i]->getNotifyType()<<endl;
		}
	}
	
	//是否是锁具主动上送报文这样的反向循环报文
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
		ZWERROR("与锁具之间的数据接收线程启动.20160713.v867")
		deque<jcLockMsg1512_t *> s_jcLockToC;		//锁具单向上传队列
		double s_lastNotifyMs=0;
		const int BLEN = 1024;
		char recvBuf[BLEN];					
		using zwccbthr::s_jcNotify;
		while (1)
		{	
			boost::this_thread::interruption_point();
			{
				boost::mutex::scoped_lock lock(thrhid_mutex);		
//////////////////////////////先处理单向上传队列////////////////////////////////////////////
				double curMs=zwccbthr::zwGetMs();
				//LOG_IF(WARNING,curMs-s_lastNotifyMs>2000)<<"curMs-s_lastNotifyMs="<<curMs-s_lastNotifyMs<<"ms"<<endl;
				LOG_IF(WARNING,s_jcLockToC.size()>0)<<"s_jcLockToC.size()="<<s_jcLockToC.size()
					<<" s_jcNotify.size()="<<s_jcNotify.size()
					<<" curMs-s_lastNotifyMs="<<(curMs-s_lastNotifyMs)<<endl;		
				//当下发队列已经为空，而且此时因为线程锁的缘故暂时不能加入新的消息，而且最后一条正常下发消息
				//的回应消息已经上传了500毫秒，而且没有等待进入下发队列的消息，就是一个空闲时刻可以上传消息了；
				LOG_IF(WARNING,s_jcLockToC.size()>0)<<"s_jcLockToC.size()="<<s_jcLockToC.size()<<endl;
				while (s_jcLockToC.size()>0)
				{
					string sUpMsg;
					jcAtmcConvertDLL::zwJCjson2CCBxml(s_jcLockToC.front()->getNotifyMsg(),sUpMsg);					
					double curMs=zwccbthr::zwGetMs();
					double diffMs= curMs- s_jcLockToC.front()->getNotifyMs();
					LOG(WARNING)<<"单向上传报文延迟了"<<diffMs<<"毫秒才上传"<<sUpMsg<<endl;					
					RecvMsgRotine pRecvMsgFun=zwccbthr::s_CallBack;
					pushToCallBack(sUpMsg.c_str(),pRecvMsgFun);
					s_jcLockToC.pop_front();
				}			
//////////////////////////////////////////////////////////////////////////
				JCHID_STATUS sts=JCHID_STATUS_FAIL;		
				if (s_jcNotify.size()>0)
				{
					jcLockMsg1512_t *nItem=s_jcNotify.front();					
					//记录真正下发的时间
					nItem->setInitNotifyMs();
					//放入用于调试匹配的队列
					s_dbgMatchNotify.push_back(nItem);
					string sCmd=nItem->getNotifyMsg();
					
					LOG(INFO)<<"下发给锁具的消息内容是"<<sCmd<<endl;
					sts=static_cast<JCHID_STATUS>( g_jhc->SendJson(sCmd.c_str()));
					//断线重连探测机制
					if (JCHID_STATUS_OK!=static_cast<JCHID_STATUS>(sts))
					{
						g_jhc->OpenJc();
					}
					//如果是锁具主动上送报文的返回确认报文，那么下发完毕后不用读取锁具的返回报文
					if(true==myIsJsonMsgFromLockFirstUp(nItem->getNotifyType())) 
					{
						continue;
					}
				}	//if (s_jcNotify.size()>0)
				//读取返回值
				double msgReadStart=zwccbthr::zwGetMs();
				do 
				{
					VLOG(4)<<"读取返回值循环进行中"<<endl;
					memset(recvBuf,0,BLEN);
					sts=static_cast<JCHID_STATUS>(g_jhc->RecvJson(recvBuf,BLEN));
					if (strlen(recvBuf)>0)
					{										
					jcLockMsg1512_t *nUpItem=new jcLockMsg1512_t(recvBuf);
					string upType=jcAtmcConvertDLL::zwGetJcJsonMsgType(recvBuf);
					if (myIsJsonMsgFromLockFirstUp(upType)==true)
					{
						LOG(WARNING)<<"收到锁具单向上传消息 "<<recvBuf<<endl;
					}
					else{
						VLOG(3)<<"收到锁具返回消息1607= "<<recvBuf<<endl;
					}
					
					string outXML;
					jcAtmcConvertDLL::zwJCjson2CCBxml(recvBuf,outXML);
					VLOG(3)<<"outXML.size()="<<outXML.size()<<"s_jcNotify.size()="<<s_jcNotify.size()<<endl;
						if (outXML.size()>0)
						{																					
							if (s_jcNotify.front()>0 && s_jcNotify.front()->matchResponJsonMsg(recvBuf)==true)
							{
								VLOG(3)<<"上下行报文匹配正确"<<endl;
								RecvMsgRotine pRecvMsgFun=zwccbthr::s_CallBack;
								pushToCallBack(outXML.c_str(),pRecvMsgFun);
								s_lastNotifyMs=zwccbthr::zwGetMs();
								VLOG(3)<<"消息"<<upType<<"处理时间"
									<<s_lastNotifyMs-s_jcNotify.front()->getNotifyMs()<<"毫秒"<<endl;
								if (s_jcNotify.size()>0){
									s_jcNotify.pop_front();
								}
								break;
							}							
							if (myIsJsonMsgFromLockFirstUp(upType)==true)
							{	
								LOG(WARNING)<<" upType="<<upType<<"该报文将会放入另一个队列延迟上传"<<endl;
								//锁具主动上送报文，暂且放到单独的队列里面有待于延迟处理
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
			//在互斥锁范围以外留出一点时间给Notify添加新的报文到下发队列
			Sleep(100);
		}	//end of while(1)

	}



//////////////////////////////////////////////////////////////////////////
}				//namespace zwccbthr{

