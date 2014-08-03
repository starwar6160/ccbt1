#include "stdafx.h"
#include "zwCcbElockHdr.h"
using namespace boost::property_tree;

namespace zwccbthr{
	int ns_thr_run=ZWTHR_RUN;	//控制通讯线程的开始和停止
	boost:: mutex thr_mutex; 
	void wait(int milliseconds)
	{ 
		boost::this_thread::sleep(boost::posix_time::milliseconds(milliseconds));		
	} 

	int ns_thrindex=0;

	//与锁具之间的通讯线程
	void ThreadLockComm()
	{		
		boost:: mutex:: scoped_lock lock( thr_mutex); 
		int i=0;
		while(1)
		{
			cout<<__FUNCTION__<<" \tComm with Lock"<<ns_thrindex<<" "<<i<<endl;
			i++;
			wait(150);
		} 
	}


	void zwStartLockCommThread(void)
	{
		ns_thrindex++;
		boost::thread t(ThreadLockComm); 
	}











//////////////////////////////////////////////////////////////////////////
}	//namespace zwccbthr{
