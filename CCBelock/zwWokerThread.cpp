#include "stdafx.h"
#include "zwCcbElockHdr.h"
#include "zwwsClient.h"
#include "zwPocoLog.h"
#include <stdio.h>
using namespace boost::property_tree;
using Poco::AutoPtr;
using Poco::Util::IniFileConfiguration;


namespace zwccbthr{
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

	string zwGetLockIP(void)
	{
		string myLockIp="10.0.0.10";	//默认值是真实锁具IP
		if (s_LockIp.length()>0)
		{
			myLockIp=s_LockIp;	//如果有配置文件的IP值，就使用之；
		}
		ZWNOTICE("连接锁具IP使用");
		ZWNOTICE(myLockIp.c_str());
		return myLockIp;
	}

	//与锁具之间的通讯线程
	void ThreadLockComm()
	{		
	ZWFUNCTRACE
	boost:: mutex:: scoped_lock lock( thr_mutex); 
	string myLockIp;
	try{
	myLockIp=zwGetLockIP();
			zwWebSocket wsconn(myLockIp.c_str(),8088);
			zwscthr=&wsconn;
			wsconn.wsConnect();
			wsconn.zwSetLongTimeOut();
			ZWNOTICE("连接锁具成功");
		while(1)
		{			
			ZWINFO("通信线程的新一轮等待接收数据循环开始");
			string recstr;
			{
				try{
					wsconn.ReceiveString(recstr);
					ZWNOTICE("成功从锁具接收数据如下：");
				}
				catch(...)
				{
					ZWFATAL("到锁具的WS连接异常断开，数据接收线程将终止");
					return;
				}
				ZWNOTICE(recstr.c_str());
			}

			string outXML;
			jcAtmcConvertDLL::zwJCjson2CCBxml(recstr,outXML);
			ZWINFO("分析锁具回传的Json并转换为建行XML成功");
			assert(outXML.length()>42);	//XML开头的固定内容38个字符，外加起码一个标签的两对尖括号合计4个字符
			ZWDBGMSG(outXML.c_str());		

			{
				boost:: mutex:: scoped_lock lock( recv_mutex); 
				s_dbgReturn=outXML;
			}
			
			if (NULL!=zwCfg::g_WarnCallback)
			{
				//调用回调函数传回信息，然后就关闭连接，结束通信线程；
				zwCfg::g_WarnCallback(outXML.c_str());
				ZWINFO("成功把从锁具接收到的数据传递给回调函数");
			}
			else
			{
				ZWWARN("回调函数指针为空，无法调用回调函数")
			}
		} 		
		ZWINFO("金储通信数据接收线程正常退出");

	}	//try
	catch(...){
		ZWFATAL("金储通信数据接收线程WebSocket网络连接异常断开，现在数据接收线程将结束");
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
			pocoLog->warning()<<"ini config file"<<cfgFileName<<" not found"<<endl;
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
		ZWDBGMSG(__FUNCTION__);
		ZWDBGMSG("\t zwWebSocket Send String Exeception!20140805.1626");
		ZWFATAL("通过WebSocket发送数据到锁具异常，可能是未连接")
	}

}