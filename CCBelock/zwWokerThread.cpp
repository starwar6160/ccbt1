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

jcHidDevice *g_jhc=NULL;	//实际的HID设备类对象

namespace zwccbthr {
	//建行给的接口，没有设置连接参数的地方，也就是说，完全可以端口，抑或是从配置文件读取
	boost::mutex thrhid_mutex;
	void pushToCallBack( const char * recvConvedXML,RecvMsgRotine pRecvMsgFun );
	deque<string> g_dqLockUpMsg;	//锁具主动上送的答非所问消息的临时队列
	bool myWaittingReturnMsg=false;	//等待返回报文期间不要下发报文
	boost::condition_variable condJcLock;
	boost::timer g_LatTimer;	//用于自动计算延迟
	deque<jcLockMsg1512_t *> s_jcNotify;		//下发命令队列，下发完毕后移动到上传队列
	deque<jcLockMsg1512_t *> s_jcUpMsg;		//上传命令队列
	deque<jcLockMsg1512_t *> s_LockFirstUpMsg;				//单向上传队列
	map<DWORD,RecvMsgRotine> s_thrIdToPointer;	//线程ID到回调函数指针的map
	////供单向上传报文专用的保存所有回调函数指针的集合,好让单向报文发给所有线程;
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
			ZWERROR("收到的锁具返回内容为空，无法返回有用信息给回调函数");
			return;
		}		


		if (strlen(recvConvedXML)>0){		
			//ZWINFO("分析锁具回传的Json并转换为建行XML成功");
			//XML开头的固定内容38个字符，外加起码一个标签的两对尖括号合计4个字符
			assert(strlen(recvConvedXML) > 42);
		}

		if (NULL==pRecvMsgFun)
		{
			const char *err1="回调函数指针为空，无法调用回调函数返回从电子锁收到的报文";
			ZWERROR(err1);
			MessageBoxA(NULL,err1,"严重警告",MB_OK);
		}
		if (NULL != pRecvMsgFun && strlen(recvConvedXML)>0) {
			//调用回调函数传回信息，
			//20150415.1727.为了万敏的要求，控制上传消息速率最多每2秒一条防止ATM死机
			//Sleep(2920);
			pRecvMsgFun(recvConvedXML);
			ZWERROR(recvConvedXML)
			VLOG_IF(4,strlen(recvConvedXML)>0)<<"回调函数收到以下内容\n"<<recvConvedXML<<endl;
#ifdef _DEBUG401
			
			VLOG(4)<<"成功把从锁具接收到的数据传递给回调函数\n";
#endif // _DEBUG401
		}
	}
	

	void my515LockRecvThr(void)
	{
		
		ZWERROR("与锁具之间的数据接收线程启动.20151215.0925.v804")
		const int BLEN = 1024;
		char recvBuf[BLEN];			
		using zwccbthr::s_jcNotify;

		//Sleep(1300);	//接收锁具上行报文的线程启动前的适当延迟
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
					LOG(WARNING)<<"线程["<<tid<<"]发送给锁具的"<<
						sType<<"类型消息.内容是"<<endl<<sCmd;
					sts=static_cast<JCHID_STATUS>( g_jhc->SendJson(sCmd.c_str()));
					if (s_LockFirstUpMsg.size()>0 && 
						s_LockFirstUpMsg.front()->NotifyType==sType)
					{
						s_LockFirstUpMsg.pop_front();
					}
					//断线重连探测机制
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
				//考虑到有可能锁具单向上行信息导致一条下发信息有多条
				//上行信息，所以多读取几次直到读不到信息为止
				memset(recvBuf,0,BLEN);
				VLOG(4)<<"接收数据的RecvJson之前"<<endl;
				sts=static_cast<JCHID_STATUS>(g_jhc->RecvJson(recvBuf,BLEN));	
				int lRecvBytes=strlen(recvBuf);
				VLOG_IF(3,lRecvBytes>0)<<"RecvJson收到"<<lRecvBytes<<"字节的数据"<<endl;
				if (strlen(recvBuf)>0)
				{
					VLOG(4)<<"收到锁具返回消息= "<<recvBuf<<endl;
					assert(strlen(recvBuf)>0);
					string sType=jcAtmcConvertDLL::zwGetJcJsonMsgType(recvBuf);
					VLOG(1)<<"收到锁具返回消息.类型是"<<sType<<"内容是\n"<<recvBuf<<endl;
					LOG(WARNING)<<"锁具下发和返回消息类型分别是 "
						<<"下发类型="<<s_jcUpMsg.front()->NotifyType
						<<"\t返回类型="<<sType<<endl;

					string outXML;
					jcAtmcConvertDLL::zwJCjson2CCBxml(recvBuf,outXML);	
					//首先单独处理下位机主动发送闭锁码和主动请求时间同步
					//这两个反向循环报文需要单独处理
					if (outXML.size()>0 &&
						(sType=="Lock_Close_Code_Lock" ||
						sType=="Lock_Time_Sync_Lock"))
					{
						ZWERROR("该锁具主动上传消息将会被另一个上传线程在不打破一问一答的前提下延迟上传")
						jcLockMsg1512_t *nItem=new jcLockMsg1512_t;
						nItem->CallerThreadID=0;
						nItem->UpMsg=outXML;
						nItem->NotifyType=sType;
						s_LockFirstUpMsg.push_back(nItem);
						LOG(INFO)<<__FUNCTION__<<"单向上传队列s_LockFirstUpMsg大小="<<s_LockFirstUpMsg.size()<<endl;
					}
						//除了这两个报文以外符合正向循环一问一答的，正常上传						
						if (outXML.size()>0 && s_jcUpMsg.size()>0 &&
							s_jcUpMsg.front()->NotifyType==sType
							&& (sType!="Lock_Close_Code_Lock" &&
							sType!="Lock_Time_Sync_Lock"))
						{
							//ZWWARN("正常上传报文")
							DWORD tid=s_jcUpMsg.front()->CallerThreadID;
							VLOG(3)<<"消息返回给线程ID="<<tid<<endl;
							RecvMsgRotine pRecvMsgFun=
								zwccbthr::s_thrIdToPointer[tid];
							pushToCallBack(outXML.c_str(),pRecvMsgFun);
							s_jcUpMsg.pop_front();
							VLOG(3)<<"普通上传队列大小s_jcUpMsg.size()="<<s_jcUpMsg.size()<<endl;
						}
				}	//if (strlen(recvBuf)>0)
			}while(strlen(recvBuf)>0);
			
		}	//收发thrhid_mutex结束			
			Sleep(100);	//接收完毕一轮报文后暂停100毫秒给下发报文腾出时间；
			VLOG(4)<<__FUNCTION__<<"\tSleep 100 ms"<<endl;
		}

	}

	void my515UpMsgThr(void)
	{
		ZWERROR("与ATMC之间的数据上传线程启动.20151215")
			while (1)
			{		
				Sleep(1000);
				//VLOG(3)<<__FUNCTION__<<"RUNNING"<<endl;
				//等待数据接收线程操作完毕“收到的数据”队列
				//获得该队列的锁的所有权，开始操作
				{
					boost::mutex::scoped_lock lock(thrhid_mutex);				
					//只有当等待配对上传的消息都已经上传完毕后
					// 才上传该被延迟上传的报文以免打乱一问一答
					if (s_LockFirstUpMsg.size()>0 && s_jcUpMsg.size()==0)
					{
						string &strSingleUp=s_LockFirstUpMsg.front()->UpMsg;					
						set<RecvMsgRotine>::iterator it; //定义前向迭代器 
						//锁具主动上传报文发给每一个线程的回调函数
						for (it=zwccbthr::s_setSingleUp.begin();
							it!=zwccbthr::s_setSingleUp.end();it++)
						{
							RecvMsgRotine pCallBack=(*it);
							LOG(WARNING)<<"延迟上传报文到回调函数地址"<<std::hex<<pCallBack<<endl;
							pushToCallBack(strSingleUp.c_str(),pCallBack);
						}
						//s_LockFirstUpMsg.pop_front();
						//LOG(INFO)<<__FUNCTION__<<"单向上传队列s_SingleUpMsg大小="<<s_LockFirstUpMsg.size()<<endl;
					}
				}
			}
	}

//////////////////////////////////////////////////////////////////////////
}				//namespace zwccbthr{

