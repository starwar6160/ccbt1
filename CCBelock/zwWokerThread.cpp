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
	bool s_wsioing=false;
	string s_HeartJump="HEART1054";

	void wait(int milliseconds)
	{ 
		boost::this_thread::sleep(boost::posix_time::milliseconds(milliseconds));		
	} 

	string zwGetLockIP(void)
	{
		string myLockIp="10.0.0.10";	//默认值是真实锁具IP
		if (s_LockIp.length()>0)
		{
			myLockIp=s_LockIp;	//如果有配置文件的IP值，就使用之；
		}
		return myLockIp;
	}

	//与锁具之间的通讯线程
	void ThreadLockComm()
	{		
	ZWFUNCTRACE
	OutputDebugStringA("COMM1");
	boost:: mutex:: scoped_lock lock( thr_mutex); 
	string myLockIp;
	try{
	myLockIp=zwGetLockIP();
	ZWTRACE("USED JCLOCKIP=");
	ZWTRACE(myLockIp.c_str());
		OutputDebugStringA("COMM2");
			zwWebSocket wsconn(myLockIp.c_str(),8088);
			zwscthr=&wsconn;
			wsconn.wsConnect();
			wsconn.zwSetLongTimeOut();
		while(1)
		{			
			OutputDebugStringA("COMM3");
			string recstr;
			{
				s_wsioing=true;
				OutputDebugStringA("COMM4");
				try{
					wsconn.ReceiveString(recstr);
				}
				catch(...)
				{
					ZWFATAL("WS CONNECT TO LOCK DISCONNECTED,TRY TO RECONNECT");
					OutputDebugStringA("WS CONNECT TO LOCK DISCONNECTED,TRY TO RECONNECT");
					//Sleep(10*1000);
					OutputDebugStringA("SLEEP 10 SEC END");
					return;
				}
				OutputDebugStringA(recstr.c_str());
				s_wsioing=false;
				OutputDebugStringA("COMM5");
			}
			
			//检查是否为心跳包
			if (s_HeartJump==recstr)
			{
				OutputDebugStringA("COMM6");
				//如果是心跳包的返回字符串，就直接忽略；
				continue;
			}

			string outXML;
			OutputDebugStringA("COMM7.1");
			jcAtmcConvertDLL::zwJCjson2CCBxml(recstr,outXML);
			OutputDebugStringA("COMM7.2");
			assert(outXML.length()>42);	//XML开头的固定内容38个字符，外加起码一个标签的两对尖括号合计4个字符
			ZWTRACE(outXML.c_str());		

			{
				boost:: mutex:: scoped_lock lock( recv_mutex); 
				s_dbgReturn=outXML;
			}
			
			if (NULL!=zwCfg::g_WarnCallback)
			{
				OutputDebugStringA("COMM8");
				//调用回调函数传回信息，然后就关闭连接，结束通信线程；
				zwCfg::g_WarnCallback(outXML.c_str());
				//zwscthr->wsClose();
				//break;
			}
			else
			{
				OutputDebugStringA("COMM8.1");
				ZWFATAL("回调函数指针为空，无法调用回调函数")
			}
		} 		
		ZWTRACE("JCLOCK COMM THREAD NORMAL EXIT 20140817");

	}	//try
	catch(...){
		OutputDebugStringA("COMM9");
		OutputDebugStringA("JC CCB ELOCK THREAD CONNECT FAIL! 20140817");
		ZWFATAL("##########JC CCB ELOCK THREAD CONNECT FAIL! 20140817");
		return;
	}
	}


	void myLoadConfig(const string &cfgFileName)
	{
		try{
		// 1. 载入配置文件  
		AutoPtr<IniFileConfiguration> cfg(new IniFileConfiguration(cfgFileName));  

		// 2. 获取节点的值  
		s_LockIp = cfg->getString("ELock.LockIp");  
		assert(s_LockIp.length()>0);
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