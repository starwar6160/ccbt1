#include "stdafx.h"
#include "zwCcbElockHdr.h"
using namespace boost::property_tree;

namespace zwccbthr{
	int ns_thr_run=ZWTHR_RUN;	//控制通讯线程的开始和停止

	void wait(int seconds) 
	{ 
		boost::this_thread::sleep(boost::posix_time::seconds(seconds)); 
	} 


	//与锁具之间的通讯线程
	void ThreadLockComm(void)
	{
		int i=0;
		while(1)
		{
			cout<<__FUNCTION__<<" \tComm with Lock"<<i++<<endl;
			wait(1);
		} 
	}


	void zwStartLockCommThread(void)
	{
		boost::thread t(ThreadLockComm); 
	}











//////////////////////////////////////////////////////////////////////////
}	//namespace zwccbthr{
