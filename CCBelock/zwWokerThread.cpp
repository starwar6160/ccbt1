#include "stdafx.h"
#include "zwCcbElockHdr.h"
#include "zwwsClient.h"
using namespace boost::property_tree;
namespace zwccbthr{
	extern zwWebSocket *zwscthr;
}

void zwPushString(const string &str)
{
	assert(str.length()>0);
	try{
		zwccbthr::zwscthr->SendString(str);
	}
	catch(...)
	{
		cout<<__FUNCTION__<<"\t zwWebSocket Send String Exeception!20140805.1626";
	}
	
}


namespace zwccbthr{
	int ns_thr_run=ZWTHR_RUN;	//控制通讯线程的开始和停止
	boost:: mutex thr_mutex; 
	//建行给的接口，没有设置连接参数的地方，也就是说，完全可以写死IP和端口，抑或是从配置文件读取
	//zwWebSocket zwscthr("10.0.0.10",8088);
	zwWebSocket * zwscthr = NULL;


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
	//zwscthr=new zwWebSocket("localhost",1425);
	zwscthr=new zwWebSocket("10.0.0.10",8088);
	zwscthr->wsConnect();
		int i=0;
		cout<<"####################Start \t"<<__FUNCTION__<<endl;
		while(1)
		{			
			if (ZWTHR_STOP==ns_thr_run)
			{
				break;
			}
			
			string recstr;
			zwscthr->ReceiveString(recstr);
			memset(sbuf,0,128);
			sprintf(sbuf,"%s Comm with Lock times %d",__FUNCTION__,i++);
			OutputDebugStringA(sbuf);
			string outXML;
			int msgTypeRecv=jcAtmcConvertDLL::zwJCjson2CCBxml(recstr,outXML);
			assert(outXML.length()>42);	//XML开头的固定内容38个字符，外加起码一个标签的两对尖括号合计4个字符
			//OutputDebugStringA(__FUNCTION__);
			//OutputDebugStringA(outXML.c_str());
			if (NULL!=zwCfg::g_WarnCallback)
			{
				zwCfg::g_WarnCallback(outXML.c_str());
			}

		} 
		memset(sbuf,0,128);
		sprintf(sbuf,"%s thread End",__FUNCTION__);
		OutputDebugStringA(sbuf);
		cout<<"####################End \t"<<__FUNCTION__<<endl;
		zwscthr->wsClose();
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
		zwscthr->wsClose();
	}











//////////////////////////////////////////////////////////////////////////
}	//namespace zwccbthr{
