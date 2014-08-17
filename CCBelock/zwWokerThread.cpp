#include "stdafx.h"
#include "zwCcbElockHdr.h"
#include "zwwsClient.h"
#include "zwPocoLog.h"
#include <stdio.h>
using namespace boost::property_tree;
using Poco::AutoPtr;
using Poco::Util::IniFileConfiguration;


namespace zwccbthr{
	int ns_thr_run=ZWTHR_RUN;	//控制通讯线程的开始和停止
	boost:: mutex thr_mutex; 
	//建行给的接口，没有设置连接参数的地方，也就是说，完全可以写死IP和端口，抑或是从配置文件读取
	//zwWebSocket zwscthr("10.0.0.10",8088);
	zwWebSocket * zwscthr = NULL;
	string s_dbgReturn="";
	boost:: mutex recv_mutex; 
	std::string s_LockIp;

	void wait(int milliseconds)
	{ 
		boost::this_thread::sleep(boost::posix_time::milliseconds(milliseconds));		
	} 

	//发送心跳包的线程
	void ThreadHeartJump()
	{
		ZWFUNCTRACE
			try{
				while(1)
				{
					if (NULL!=zwscthr)
					{
						zwscthr->SendString("     ");
						string recstr;
						zwscthr->ReceiveString(recstr);
						OutputDebugStringA("HEART JUMP PACKAGE OVER ATMC DLL AND JINCHU ELOCK");
						Sleep(5000);	//最多不超过60秒，否则就断了连接；
					}
				}
		}
		catch(...)
		{
			OutputDebugStringA("HEART JUMP THREAD FAIL!");
			return;
		}
	}

	//与锁具之间的通讯线程
	void ThreadLockComm()
	{		
	ZWFUNCTRACE
	boost:: mutex:: scoped_lock lock( thr_mutex); 
	try{
	string myLockIp="10.0.0.10";	//默认值是真实锁具IP
	if (s_LockIp.length()>0)
	{
		myLockIp=s_LockIp;	//如果有配置文件的IP值，就使用之；
	}
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
			sprintf(sbuf,"JCCOMM THREAD Comm with Lock times %d",i++);
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
				//调用回调函数传回信息，然后就关闭连接，结束通信线程；
				zwCfg::g_WarnCallback(outXML.c_str());
				//zwscthr->wsClose();
				break;
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
		ZWTRACE("JCLOCK COMM THREAD NORMAL CLOSE WS CONNECT 20140815");

	}	//try
	catch(...){
		zwscthr->wsClose();	//异常就主动关闭连接防止下一次正常连接无法连上
		OutputDebugStringA("JC CCB ELOCK THREAD CONNECT FAIL! 20140815");
		ZWFATAL("##########JC CCB ELOCK THREAD CONNECT FAIL! 20140815");
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

	void myLoadConfig(const string &cfgFileName)
	{
		try{
		// 1. 载入配置文件  
		AutoPtr<IniFileConfiguration> cfg(new IniFileConfiguration(cfgFileName));  

		// 2. 获取节点的值  
		s_LockIp = cfg->getString("ELock.LockIp");  
		int  ThreadNum= cfg->getInt("ELock.ThreadNum");
		}
		catch(Poco::Exception e)
		{
			g_log->warning()<<"ini config file"<<cfgFileName<<" not found"<<endl;
		}
	}









//////////////////////////////////////////////////////////////////////////
}	//namespace zwccbthr{

void zwPushString(const string &str)
{
	ZWFUNCTRACE
		assert(str.length()>0);
	if (str.length()==0)
	{
		return;
	}
	if (NULL==zwccbthr::zwscthr)
	{
		ZWFATAL("WebSocket对象指针为空！");
		return;
	}
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