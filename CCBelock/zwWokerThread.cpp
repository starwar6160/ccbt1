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

jcHidDevice *g_jhc=NULL;	//实际的HID设备类对象

namespace zwccbthr {
	//建行给的接口，没有设置连接参数的地方，也就是说，完全可以端口，抑或是从配置文件读取
	boost::mutex thrhid_mutex;
	boost::mutex upDeque_mutex;
	void pushToCallBack( const char * recvBuf );
	deque<string> g_dqLockUpMsg;	//锁具主动上送的答非所问消息的临时队列
	bool myWaittingReturnMsg=false;	//等待返回报文期间不要下发报文
	boost::condition_variable condJcLock;
	boost::timer g_LatTimer;	//用于自动计算延迟
	deque<string> s_jcNotify;		//下发命令

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
			ZWERROR("收到的锁具返回内容为空，无法返回有用信息给回调函数");
			return;
		}		


		if (strlen(recvConvedXML)>0){		
			//ZWINFO("分析锁具回传的Json并转换为建行XML成功");
			//XML开头的固定内容38个字符，外加起码一个标签的两对尖括号合计4个字符
			assert(strlen(recvConvedXML) > 42);
		}

		if (NULL==zwCfg::g_WarnCallback)
		{
			const char *err1="回调函数指针为空，无法调用回调函数返回从电子锁收到的报文";
			ZWERROR(err1);
			MessageBoxA(NULL,err1,"严重警告",MB_OK);
		}
		if (NULL != zwCfg::g_WarnCallback && strlen(recvConvedXML)>0) {
			//调用回调函数传回信息，
			//20150415.1727.为了万敏的要求，控制上传消息速率最多每2秒一条防止ATM死机
			//Sleep(2920);
			zwCfg::g_WarnCallback(recvConvedXML);
			ZWERROR(recvConvedXML)
			VLOG_IF(4,strlen(recvConvedXML)>0)<<"回调函数收到以下内容\n"<<recvConvedXML<<endl;
#ifdef _DEBUG401
			
			VLOG(4)<<"成功把从锁具接收到的数据传递给回调函数\n";
#endif // _DEBUG401
		}
	}
	

	void my515LockRecvThr(void)
	{
		ZWERROR("与锁具之间的数据接收线程启动.20151210.v788")
		const int BLEN = 1024;
		char recvBuf[BLEN];			
		using zwccbthr::s_jcNotify;

		//Sleep(1300);	//接收锁具上行报文的线程启动前的适当延迟
		while (1)
		{	
			boost::this_thread::interruption_point();
			{
			//下发命令和接收锁具回复的整个过程加锁
			boost::mutex::scoped_lock lock(thrhid_mutex);		
			JCHID_STATUS sts=JCHID_STATUS_FAIL;			
			{
				VLOG_IF(4,s_jcNotify.size()>0)<<"s_jcNotify.size()="<<s_jcNotify.size()<<endl;
				if (s_jcNotify.size()>0)
				{
					zwccbthr::myWaittingReturnMsg=true;					
					LOG(WARNING)<<"SendToLock JsonIS\n"<<s_jcNotify.front().c_str()<<endl;
					LOG(INFO)<<"发送给锁具的消息.内容是"<<s_jcNotify.front()<<endl;
					sts=static_cast<JCHID_STATUS>( g_jhc->SendJson(s_jcNotify.front().c_str()));
					
					//断线重连探测机制
					if (JCHID_STATUS_OK!=static_cast<JCHID_STATUS>(sts))
					{
						g_jhc->OpenJc();
					}
					s_jcNotify.pop_front();
				}
			}
			do{
				//考虑到有可能锁具单向上行信息导致一条下发信息有多条
				//上行信息，所以多读取几次直到读不到信息为止
				memset(recvBuf,0,BLEN);
				VLOG(3)<<"接收数据的RecvJson之前"<<endl;
				sts=static_cast<JCHID_STATUS>(g_jhc->RecvJson(recvBuf,BLEN));				
				VLOG(3)<<"接收数据的RecvJson之后 收到"<<strlen(recvBuf)<<"字节的数据"<<endl;
				if (strlen(recvBuf)>0)
				{
					VLOG(4)<<"收到锁具返回消息= "<<recvBuf<<endl;
					assert(strlen(recvBuf)>0);
					LOG(WARNING)<<"收到锁具返回消息.内容是\n"<<recvBuf<<endl;
					string outXML;
					jcAtmcConvertDLL::zwJCjson2CCBxml(recvBuf,outXML);					
						if (outXML.size()>0)
						{
							boost::mutex::scoped_lock lock(upDeque_mutex);	
							//收到的报文放入上传队列，由单独的线程去上传
							g_dqLockUpMsg.push_back(outXML);
						}													
				}
			}while(strlen(recvBuf)>0);			
		}	//收发thrhid_mutex结束			
			Sleep(100);	//接收完毕一轮报文后暂停100毫秒给下发报文腾出时间；
		}

	}

	void my515UpMsgThr(void)
	{
		ZWERROR("与ATMC之间的数据上传线程启动.20151203.1720")
			while (1)
			{
					//等待数据接收线程操作完毕“收到的数据”队列
					//获得该队列的锁的所有权，开始操作
					boost::mutex::scoped_lock lock(upDeque_mutex);	
					for (int i=0;i<g_dqLockUpMsg.size();i++)
					{
						LOG(WARNING)<<"单独线程上传报文"<<endl;
						pushToCallBack(g_dqLockUpMsg[i].c_str());
						g_dqLockUpMsg.pop_front();
					}
					Sleep(100);
			}
	}

//////////////////////////////////////////////////////////////////////////
}				//namespace zwccbthr{

