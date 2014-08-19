#include "stdafx.h"
#include "zwCcbElockHdr.h"
#include "zwwsClient.h"
#include "zwPocoLog.h"
#include <stdio.h>
using namespace boost::property_tree;
using Poco::AutoPtr;
using Poco::Util::IniFileConfiguration;


namespace zwccbthr{
	int ns_thr_run=ZWTHR_RUN;	//����ͨѶ�̵߳Ŀ�ʼ��ֹͣ
	boost:: mutex thr_mutex; 
	//���и��Ľӿڣ�û���������Ӳ����ĵط���Ҳ����˵����ȫ����д��IP�Ͷ˿ڣ��ֻ��Ǵ������ļ���ȡ
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
		string myLockIp="10.0.0.10";	//Ĭ��ֵ����ʵ����IP
		if (s_LockIp.length()>0)
		{
			myLockIp=s_LockIp;	//����������ļ���IPֵ����ʹ��֮��
		}
		return myLockIp;
	}

	//������֮���ͨѶ�߳�
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
			
			//����Ƿ�Ϊ������
			if (s_HeartJump==recstr)
			{
				OutputDebugStringA("COMM6");
				//������������ķ����ַ�������ֱ�Ӻ��ԣ�
				continue;
			}

			string outXML;
			OutputDebugStringA("COMM7.1");
			jcAtmcConvertDLL::zwJCjson2CCBxml(recstr,outXML);
			OutputDebugStringA("COMM7.2");
			assert(outXML.length()>42);	//XML��ͷ�Ĺ̶�����38���ַ����������һ����ǩ�����Լ����źϼ�4���ַ�
			ZWTRACE(outXML.c_str());		

			{
				boost:: mutex:: scoped_lock lock( recv_mutex); 
				s_dbgReturn=outXML;
			}
			
			if (NULL!=zwCfg::g_WarnCallback)
			{
				OutputDebugStringA("COMM8");
				//���ûص�����������Ϣ��Ȼ��͹ر����ӣ�����ͨ���̣߳�
				zwCfg::g_WarnCallback(outXML.c_str());
				//zwscthr->wsClose();
				//break;
			}
			else
			{
				OutputDebugStringA("COMM8.1");
				ZWFATAL("�ص�����ָ��Ϊ�գ��޷����ûص�����")
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
		// 1. ���������ļ�  
		AutoPtr<IniFileConfiguration> cfg(new IniFileConfiguration(cfgFileName));  

		// 2. ��ȡ�ڵ��ֵ  
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
		ZWFATAL("WebSocket����ָ��Ϊ�գ�");
		return;
	}
	try{
		zwccbthr::zwscthr->SendString(str);
	}
	catch(...)
	{
		ZWTRACE(__FUNCTION__);
		ZWTRACE("\t zwWebSocket Send String Exeception!20140805.1626");
		ZWFATAL("ͨ��WebSocket�������ݵ������쳣��������δ����")
	}

}