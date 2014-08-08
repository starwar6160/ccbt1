#include "stdafx.h"
#include "zwCcbElockHdr.h"
#include "zwwsClient.h"
#include <stdio.h>
using namespace boost::property_tree;


namespace zwccbthr{
	extern zwWebSocket *zwscthr;
}

void zwPushString(const string &str)
{
	ZWFUNCTRACE
	assert(str.length()>0);
	try{
		zwccbthr::zwscthr->SendString(str);
	}
	catch(...)
	{
		ZWTRACE(__FUNCTION__);
		ZWTRACE("\t zwWebSocket Send String Exeception!20140805.1626");
		ZWFATAL("通过WebSocket发送数据到锁具异常，可能是未连接")
	}
	
}


namespace zwccbthr{
	void ThreadLockComm();
	//线程对象作为一个全局静态变量，则不需要显示启动就能启动一个线程
	boost::thread t(ThreadLockComm); 
	boost:: mutex thr_mutex; //用互斥保护WebSocket连接对象
	//建行给的接口，没有设置连接参数的地方，也就是说，完全可以写死IP和端口，抑或是从配置文件读取
	zwWebSocket * zwscthr = NULL;


	void wait(int milliseconds)
	{ 
		boost::this_thread::sleep(boost::posix_time::milliseconds(milliseconds));		
	} 

	//与锁具之间的通讯线程
	void ThreadLockComm()
	{		
	ZWFUNCTRACE
	boost:: mutex:: scoped_lock lock( thr_mutex); 
	char *LockHost=getenv("JCLOCKIP");
	string myLockIp="10.0.0.10";	//默认值是真实锁具IP
	ZWTRACE("JCLOCKIP=");
	if (NULL!=LockHost)
	{
		ZWTRACE(LockHost);
		myLockIp=LockHost;	//如果设置了JCLOCKIP环境变量，就用该值替代
	}

	zwscthr=new zwWebSocket(myLockIp.c_str(),8088);
	zwscthr->wsConnect();
		int i=0;
		while(1)
		{			
			string recstr;
			zwscthr->ReceiveString(recstr);
			if (recstr.length()>9)
			{
				MessageBoxA(NULL,recstr.c_str(),"ATMC ConcertDLL Received json is",MB_OK);			
			}
			
			char sbuf[128];
			memset(sbuf,0,128);
			sprintf(sbuf,"%s Comm with Lock times %d",__FUNCTION__,i++);
			ZWTRACE(sbuf);
			string outXML;
			int msgTypeRecv=jcAtmcConvertDLL::zwJCjson2CCBxml(recstr,outXML);
			assert(outXML.length()>42);	//XML开头的固定内容38个字符，外加起码一个标签的两对尖括号合计4个字符
			ZWTRACE(outXML.c_str());		

			if (NULL!=zwCfg::g_WarnCallback)
			{
				zwCfg::g_WarnCallback(outXML.c_str());
			}
			else
			{
				ZWFATAL("回调函数指针为空，无法调用回调函数")
			}

		} 		
		zwscthr->wsClose();
	}

//////////////////////////////////////////////////////////////////////////
}	//namespace zwccbthr{
