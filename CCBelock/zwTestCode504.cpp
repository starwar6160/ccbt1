#include "stdafx.h"
#include <boost/thread/condition.hpp>
#include "zwHidComm.h"
#include "zwHidDevClass2015.h"
#include "CCBelock.h"


using jchidDevice2015::jcHidDevice;
using boost::condition_variable;
using boost::condition_variable_any;


namespace zwtest504
{

	using namespace boost;


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
	boost::condition_variable cv1;
	deque<string> dqStr;

	//生产者线程，需要比消费者线程晚一点启动，100毫秒即可
	void zwtest514thr1(void)
	{
		//此处是为了等待消费者线程先启动后用条件变量阻塞在那里
		Sleep(100);
		cout<<endl<<__FUNCTION__<<" START data input"<<endl;
		for(int i=0;i<60;i++)
		{
			//需要保护的操作代码段加锁
			boost::mutex::scoped_lock lock(mu1);	
			char sbuf[32];
			memset(sbuf,0,32);
			sprintf(sbuf,"%d",i+1);
			dqStr.push_back(sbuf);			
			//printf("^%s^\t",sbuf);
			//操作完毕，利用条件变量的notify_all或者notify_one通知
			//其他等待线程可以开始操作了									
			cv1.notify_all();				
			Sleep(10);

		}
		
		
		cout<<endl<<__FUNCTION__<<" END data input dqSize="<<dqStr.size()<<endl;
	}

	//消费者线程，需要首先启动，用条件变量阻塞等待生产者线程
	void zwtest514thr2(void)
	{
		cout<<__FUNCTION__<<" START"<<endl;		
		cout<<__FUNCTION__<<" start output data from thr1's deque"<<endl;
		while(1)
		{
				//和另一个线程使用同一个锁，利用条件变量等待该锁解开
				//注意此处必须要把这两行包括在一个花括号中，以便起到
				//阻塞执行到这里的作用。否则就是阻塞整个线程了
				boost::mutex::scoped_lock lock(mu1);
				cv1.wait(lock);
				cout<<".";
				assert(dqStr.size()>0);
				for (int i=0;i<dqStr.size();i++)
				{
					cout<<dqStr[i]<<" ";

				}				 
				dqStr.clear();
				//Sleep(10);
		}
		cout<<__FUNCTION__<<" END"<<endl;
	}

	void zwtest514thr3(void)
	{
		cout<<__FUNCTION__<<" START"<<endl;		
		cout<<__FUNCTION__<<" start output data from thr1's deque"<<endl;
		for(int i=0;i<22;i++)
		{
			cout<<"[";
			//和另一个线程使用同一个锁，利用条件变量等待该锁解开
			//注意此处必须要把这两行包括在一个花括号中，以便起到
			//阻塞执行到这里的作用。否则就是阻塞整个线程了
			boost::mutex::scoped_lock lock(mu1);
				cv1.wait(lock);
			for (auto iter=dqStr.begin();iter!=dqStr.end();iter++)
			{
				cout<<(*iter)<<" ";
			}
			dqStr.clear();
			//Sleep(20);
			cout<<"]";
		}
		cout<<__FUNCTION__<<" END"<<endl;
	}
//////////////////////////////////////////////////////////////////////////

	void zwtest514Main(void)
	{
		//boost::thread tt1(zwtest514thr1);
		//boost::thread tt2(zwtest514thr2);
		boost::thread_group grp;		
						
		grp.create_thread(zwtest514thr1);
		grp.create_thread(zwtest514thr2);
		//grp.create_thread(zwtest514thr3);
		//cv1.notify_all();
		grp.join_all();
	}

	timer t;

	void mybtimerTest518()
	{	
		cout << t.elapsed_max() / 3600 << "h elapsed_max" << endl;
		cout << t.elapsed_min() << "s elapsed_min" <<endl;
		Sleep(500);
		cout << t.elapsed()<< "s" << endl;
		Sleep(700);
		cout << t.elapsed()<< "s" << endl;
		

	}

}



#ifdef _DEBUG_GTEST20151219

CCBELOCK_API int zwStartGtestInDLL(void)
{
	zwtest504::mybtimerTest518();
	//return 1;
#define _DEBUG514
#ifdef _DEBUG514
	int argc=1;
	char *argv[1];
	argv[0]=NULL;
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
#else
	zwtest504::zwtest514Main();
#endif // _DEBUG514

	return 1;
}
#endif // _DEBUG_GTEST20151219