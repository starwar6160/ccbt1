#include "stdafx.h"
#include <boost/thread/condition.hpp>
#include "zwHidComm.h"
#include "zwHidDevClass2015.h"
using jchidDevice2015::jcHidDevice;
using boost::condition_variable;
using boost::condition_variable_any;


namespace zwtest504
{
	void zwtest504hidClass(void)
	{
		const char *msg02="{\"Command\":\"Lock_Now_Info\"}";
		char recvJson[256];
		memset(recvJson,0,256);

		jcHidDevice *jc1=new jcHidDevice();	
		printf("%s\n",__FUNCTION__);
		jc1->SendJson(	msg02);
		jc1->CloseJc();
		jc1->OpenJc();
		//jc1->OpenJc();
		//jc1->SendJson(	msg02);
		jc1->RecvJson(recvJson,256);
		ZWWARN(recvJson)
			Sleep(2000);

		//jc1->OpenJc();

		//Sleep(3000);
		//memset(recvJson,0,256);
		//jc1->RecvJson(recvJson,256);
		//ZWWARN(recvJson)
		//jc1->CloseJc();

		//jcHidDevice *jc2=new jcHidDevice();
		//memset(recvJson,0,256);
		//jc2->SendJson(	msg02);
		//jc2->RecvJson(recvJson,256);
		//ZWWARN(recvJson)
	}

	boost::mutex mu1;
	//boost::mutex mu2;
	boost::condition_variable cv1;
	//boost::condition_variable cv2;
	deque<string> dqStr;
	

	void zwtest514deque1(void)
	{
		cout<<__FUNCTION__<<" START"<<endl;
		{
			//需要保护的操作代码段加锁
			boost::mutex::scoped_lock lock(mu1);
			//cv1.wait(lock);
			cout<<"dqStr empty size is "<<dqStr.size()<<endl;
			dqStr.push_back("dqstr Line1");
			dqStr.push_back("dqstr Line2");
			dqStr.push_back("dqstr Line3");
			cout<<"dq input data end"<<endl;	
			Sleep(1000);			
		}
		//操作完毕，利用条件变量的notify_all或者notify_one通知
		//其他等待线程可以开始操作了
		cv1.notify_all();
		cout<<"Sleep 2s start\n";
		Sleep(2000);
		cout<<"Sleep 2s end\n";		
		cout<<__FUNCTION__<<" END"<<endl;
	}

	void zwtest514thr2(void)
	{
		cout<<__FUNCTION__<<" START"<<endl;		
		{
			//和另一个线程使用同一个锁，利用条件变量等待该锁解开
			//注意此处必须要把这两行包括在一个花括号中，以便起到
			//阻塞执行到这里的作用。否则就是阻塞整个线程了
			boost::mutex::scoped_lock lock(mu1);
			cv1.wait(lock);
		}		
		
		for (auto iter=dqStr.begin();iter!=dqStr.end();iter++)
		{
			cout<<(*iter)<<endl;
		}
		cout<<__FUNCTION__<<" END"<<endl;

	}
//////////////////////////////////////////////////////////////////////////

	void zwtest514Main(void)
	{
		//boost::thread tt1(zwtest514deque1);
		//boost::thread tt2(zwtest514thr2);
		boost::thread_group grp;		
		grp.create_thread(zwtest514thr2);
		Sleep(500);
		grp.create_thread(zwtest514deque1);
		//cv1.notify_all();
		grp.join_all();
	}


}


