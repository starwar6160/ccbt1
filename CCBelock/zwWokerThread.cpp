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
using jcAtmcConvertDLL::zwGetJcJsonMsgType;

jcHidDevice *g_jhc=NULL;	//实际的HID设备类对象



namespace zwccbthr {
	//建行给的接口，没有设置连接参数的地方，也就是说，完全可以端口，抑或是从配置文件读取
	boost::mutex thrhid_mutex;
	boost::mutex upDeque_mutex;
	void pushToCallBack( const char * recvConvedXML,RecvMsgRotine pCallBack );
	boost::timer g_LatTimer;	//用于自动计算延迟

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
	//为每个调用者线程准备的专用下发和上传消息的队列
	std::deque<string> s_cmdType;	//记录下发命令类型的队列

	JcLockSendRecvData::JcLockSendRecvData(DWORD callerID,RecvMsgRotine pCallBack)
	{
		m_CallerThreadID=callerID;
		m_CallBack=pCallBack;
		VLOG(3)<<"分配了一个JCATMC DLL上层应用每个线程专用的上传下发队列 线程ID= "<<m_CallerThreadID<<endl;
	}

	JcLockSendRecvData::~JcLockSendRecvData()
	{
		m_Notify.clear();
		m_UpMsg.clear();
		VLOG(3)<<"删除了一个JCATMC DLL上层应用每个线程专用的上传下发队列 线程ID= "<<m_CallerThreadID<<endl;
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
		string cmdType=zwGetJcJsonMsgType(NotifyMsg.c_str());
		VLOG(4)<<__FUNCTION__<<"保存的发送给锁具的消息类型是 "<<cmdType<<endl;
		s_cmdType.push_back(cmdType) ;
		VLOG(4)<<"threadID="<<m_CallerThreadID<<" s_cmdType size ="<<s_cmdType.size()<<endl; 
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
			VLOG(3)<<"下面的锁具返回报文上传给线程ID="<<m_CallerThreadID<<"的线程"<<endl;
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
			ZWERROR("收到的锁具返回内容为空，无法返回有用信息给回调函数");
			return;
		}		


		if (strlen(recvConvedXML)>0){		
			//ZWINFO("分析锁具回传的Json并转换为建行XML成功");
			//XML开头的固定内容38个字符，外加起码一个标签的两对尖括号合计4个字符
			assert(strlen(recvConvedXML) > 42);
		}

		if (NULL==pCallBack)
		{
			const char *err1="回调函数指针为空，无法调用回调函数返回从电子锁收到的报文";
			ZWERROR(err1);
			MessageBoxA(NULL,err1,"严重警告",MB_OK);
		}
		if (NULL != pCallBack && strlen(recvConvedXML)>0) {
			//调用回调函数传回信息，
			//20150415.1727.为了万敏的要求，控制上传消息速率最多每2秒一条防止ATM死机
			//Sleep(2920);
			pCallBack(recvConvedXML);
			ZWERROR(recvConvedXML)
			VLOG_IF(4,strlen(recvConvedXML)>0)<<"回调函数收到以下内容\n"<<recvConvedXML<<endl;
#ifdef _DEBUG401
			
			VLOG(4)<<"成功把从锁具接收到的数据传递给回调函数\n";
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
				LOG(WARNING)<<"从线程"<<zwCfg::vecCallerCmdDq[i]->getCallerID()<<
					"的收发队列取出下发给锁具的消息.内容是"<<curCmd;

				sts=static_cast<JCHID_STATUS>( g_jhc->SendJson(curCmd.c_str()));
				
				//断线重连探测机制
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
			//考虑到有可能锁具单向上行信息导致一条下发信息有多条
			//上行信息，所以多读取几次直到读不到信息为止
			memset(recvBuf,0,BLEN);
			//VLOG(3)<<"接收数据的RecvJson之前"<<endl;
			sts=static_cast<JCHID_STATUS>(g_jhc->RecvJson(recvBuf,BLEN));				
			VLOG_IF(3,strlen(recvBuf)>0)<<"接收数据的RecvJson之后 收到 "<<
				strlen(recvBuf)<<" 字节的数据"<<endl;
			if (strlen(recvBuf)>0)
			{
				VLOG(1)<<"收到锁具返回消息= "<<recvBuf<<endl;
				assert(strlen(recvBuf)>0);
				VLOG(4)<<"收到锁具返回消息类型是 "<<zwGetJcJsonMsgType(recvBuf)<<endl;
				string recvType=zwGetJcJsonMsgType(recvBuf);				
				string notifyType="";
				VLOG(4)<<"s_cmdType size ="<<zwccbthr::s_cmdType.size()<<endl;
				if (zwccbthr::s_cmdType.size()>0)
				{
					notifyType=zwccbthr::s_cmdType.front();
				}				
				VLOG(4)<<"recvType="<<recvType<<"\tnotifyType="<<notifyType<<endl;
				VLOG(4)<<"before s_cmdType.pop_front()\tonThreadID= "<<
					zwCfg::vecCallerCmdDq[i]->getCallerID()
					<<" s_cmdType.size()="<<zwccbthr::s_cmdType.size()
					<<endl;
				if (""!=notifyType  && recvType==notifyType)
				{					
					zwccbthr::s_cmdType.pop_front();					
					VLOG(4)<<"after s_cmdType.pop_front()\tonThreadID= "<<
						zwCfg::vecCallerCmdDq[i]->getCallerID()
						<<" s_cmdType.size()="<<zwccbthr::s_cmdType.size()
						<<endl;
					VLOG(1)<<"收到锁具返回消息.内容是\n"<<recvBuf<<endl;
					string outXML;
					jcAtmcConvertDLL::zwJCjson2CCBxml(recvBuf,outXML);					
					if (outXML.size()>0)
					{						
						boost::mutex::scoped_lock lock(upDeque_mutex);	
						//收到的报文放入上传队列，由类成员函数去上传
						zwCfg::vecCallerCmdDq[i]->PushUpMsg(outXML);
						zwCfg::vecCallerCmdDq[i]->UploadLockResult();						
					}													
				}
				if (""!=notifyType  && recvType!=notifyType)
				{
					LOG(ERROR)<<"答非所问，需要延迟上传"<<endl;					
				}

			}
		}while(strlen(recvBuf)>0);			
	}

	void my515LockRecvThr(void)
	{
		ZWERROR("与锁具之间的数据接收线程启动.20151210.v788")
		while (1)
		{	
			boost::this_thread::interruption_point();
			//遍历所有收发队列
			int cmdDqSize=zwCfg::vecCallerCmdDq.size();
			for (int i=0;i<cmdDqSize;i++)
			{
				//下发命令和接收锁具回复的整个过程加锁
				boost::mutex::scoped_lock lock(thrhid_mutex);		
				mySendCmd2Lock(i);
				myRecvFromLock(i);
			}	//end for
			Sleep(100);	//接收完毕一轮报文后暂停100毫秒给下发报文腾出时间；
		}
	}

	void my515UpMsgThr(void)
	{
		ZWERROR("与ATMC之间的数据上传线程启动.20151203.1720")
		return;
			while (1)
			{
				boost::this_thread::interruption_point();
					//等待数据接收线程操作完毕“收到的数据”队列
					//获得该队列的锁的所有权，开始操作
					boost::mutex::scoped_lock lock(upDeque_mutex);	
					for(int i=0;i<zwCfg::vecCallerCmdDq.size();i++)
					{
						DWORD tid=zwCfg::vecCallerCmdDq[i]->getCallerID();

					}
					Sleep(100);
			}
	}

//////////////////////////////////////////////////////////////////////////
}				//namespace zwccbthr{

