#include "stdafx.h"
#include "zwCcbElockHdr.h"
#include "zwwsClient.h"
using namespace boost::property_tree;

namespace zwccbthr{
	int ns_thr_run=ZWTHR_RUN;	//控制通讯线程的开始和停止
	boost:: mutex thr_mutex; 
	zwWebSocket zwscthr("localhost",1425);
	void wait(int milliseconds)
	{ 
		boost::this_thread::sleep(boost::posix_time::milliseconds(milliseconds));		
	} 

	//与锁具之间的通讯线程
	void ThreadLockComm()
	{		
	boost:: mutex:: scoped_lock lock( thr_mutex); 
	char sbuf[128];
	memset(sbuf,0,128);
	sprintf(sbuf,"%s thread Start",__FUNCTION__);
	OutputDebugStringA(sbuf);
	zwscthr.wsConnect();
		int i=0;
		cout<<"####################Start \t"<<__FUNCTION__<<endl;
		while(1)
		{			
			if (ZWTHR_STOP==ns_thr_run)
			{
				break;
			}
			memset(sbuf,0,128);
			sprintf(sbuf,"%s Comm with Lock times %d",__FUNCTION__,i++);
			cout<<sbuf<<endl;
			OutputDebugStringA(sbuf);
			zwscthr.SendString("STRINGFROMZWLOCKCOMMTHREAD20140803.1403");
			string recstr;
			zwscthr.ReceiveString(recstr);
			//cout<<recstr<<endl;
			if (NULL!=zwCfg::g_WarnCallback)
			{
				zwCfg::g_WarnCallback(recstr.c_str());
			}
			wait(150);
			if (i>20)
			{
				break;
			}
		} 
		memset(sbuf,0,128);
		sprintf(sbuf,"%s thread End",__FUNCTION__);
		OutputDebugStringA(sbuf);
		cout<<"####################End \t"<<__FUNCTION__<<endl;
		zwscthr.wsClose();
	}

	//线程对象作为一个全局静态变量，则不需要显示启动就能启动一个线程
	boost::thread t(ThreadLockComm); 
	void zwStartLockCommThread(void)
	{
		OutputDebugStringA(__FUNCTION__);
	}

	void zwStopLockCommThread(void)
	{
		OutputDebugStringA(__FUNCTION__);
		ns_thr_run=ZWTHR_STOP;
		zwscthr.wsClose();
	}











//////////////////////////////////////////////////////////////////////////
}	//namespace zwccbthr{
