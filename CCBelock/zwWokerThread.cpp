#include "stdafx.h"

void wait(int seconds) 
{ 
	boost::this_thread::sleep(boost::posix_time::seconds(seconds)); 
} 

void thread() 
{ 
	for (int i = 0; i < 5; ++i) 
	{ 
		wait(1); 
		std::cout << i << std::endl; 
		OutputDebugStringA("20140725.1618");
	} 
} 
//boost::thread t(thread); 

int	zwThrTest1(int num)
{
	
	//t.join(); 
	
	//t.start_thread();
	//cout<<__FUNCTION__<<" END"<<endl;
	//t.timed_join(boost::posix_time::seconds(1));
	
	return num+100;
}