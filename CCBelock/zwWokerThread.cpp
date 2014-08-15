#include "stdafx.h"
#include "zwCcbElockHdr.h"
#include "zwwsClient.h"
#include "zwPocoLog.h"
#include <stdio.h>
using namespace boost::property_tree;


namespace zwccbthr{
	extern zwWebSocket *zwscthr;
	extern boost:: mutex recv_mutex; 
	extern string s_dbgReturn;
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
	int ns_thr_run=ZWTHR_RUN;	//控制通讯线程的开始和停止
	boost:: mutex thr_mutex; 
	//建行给的接口，没有设置连接参数的地方，也就是说，完全可以写死IP和端口，抑或是从配置文件读取
	//zwWebSocket zwscthr("10.0.0.10",8088);
	zwWebSocket * zwscthr = NULL;
	string s_dbgReturn="";
	boost:: mutex recv_mutex; 

	void wait(int milliseconds)
	{ 
		boost::this_thread::sleep(boost::posix_time::milliseconds(milliseconds));		
	} 

	//与锁具之间的通讯线程
	void ThreadLockComm()
	{		
	ZWFUNCTRACE
	boost:: mutex:: scoped_lock lock( thr_mutex); 
	try{
	string myLockIp="10.0.0.10";	//默认值是真实锁具IP
#ifdef _DEBUG
	myLockIp="127.0.0.1";	//临时测试用模拟器IP
#endif // _DEBUG
	ZWTRACE("USED JCLOCKIP=");
	ZWTRACE(myLockIp.c_str());

	zwscthr=new zwWebSocket(myLockIp.c_str(),8088);
	OutputDebugStringA("ZWTHR2");
	zwscthr->wsConnect();
		int i=0;
		while(1)
		{			
			OutputDebugStringA("ZWTHR3");
			if (ZWTHR_STOP==ns_thr_run)
			{
				break;
			}
			string recstr;
			zwscthr->ReceiveString(recstr);

			char sbuf[128];
			memset(sbuf,0,128);
			sprintf(sbuf,"%s Comm with Lock times %d",__FUNCTION__,i++);
			ZWTRACE(sbuf);
			string outXML;
			int msgTypeRecv=jcAtmcConvertDLL::zwJCjson2CCBxml(recstr,outXML);
			assert(outXML.length()>42);	//XML开头的固定内容38个字符，外加起码一个标签的两对尖括号合计4个字符
			ZWTRACE(outXML.c_str());		

			{
				boost:: mutex:: scoped_lock lock( recv_mutex); 
				s_dbgReturn=outXML;
			}
			
			if (NULL!=zwCfg::g_WarnCallback)
			{
				zwCfg::g_WarnCallback(outXML.c_str());
			}
			else
			{
				ZWFATAL("回调函数指针为空，无法调用回调函数")
			}
			if (recstr.length()>9)
			{
				//MessageBoxA(NULL,recstr.c_str(),"ATMC ConcertDLL Received json is",MB_OK);			
			}

		} 		
		zwscthr->wsClose();

	}	//try
	catch(...){
		OutputDebugStringA("JC CCB ELOCK THREAD CONNECT FAIL! 20140812");
		return;
	}
	}


	void zwStartLockCommThread(void)
	{
		ZWFUNCTRACE
		ZWTRACE(__FUNCTION__);
	}

	void zwStopLockCommThread(void)
	{
		ZWFUNCTRACE
		ZWTRACE(__FUNCTION__);
		ns_thr_run=ZWTHR_STOP;
		zwscthr->wsClose();
	}











//////////////////////////////////////////////////////////////////////////
}	//namespace zwccbthr{
