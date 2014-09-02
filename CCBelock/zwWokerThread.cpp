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
	//���и��Ľӿڣ�û���������Ӳ����ĵط���Ҳ����˵����ȫ����д��IP�Ͷ˿ڣ��ֻ��Ǵ������ļ���ȡ
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
		string myLockIp="10.0.0.10";	//Ĭ��ֵ����ʵ����IP
		if (s_LockIp.length()>0)
		{
			myLockIp=s_LockIp;	//����������ļ���IPֵ����ʹ��֮��
		}
		ZWNOTICE("��������IPʹ��");
		ZWNOTICE(myLockIp.c_str());
		return myLockIp;
	}

	//������֮���ͨѶ�߳�
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
			ZWNOTICE("�������߳ɹ�");
		while(1)
		{			
			ZWINFO("ͨ���̵߳���һ�ֵȴ���������ѭ����ʼ");
			string recstr;
			{
				try{
					wsconn.ReceiveString(recstr);
					ZWNOTICE("�ɹ������߽����������£�");
				}
				catch(...)
				{
					ZWFATAL("�����ߵ�WS�����쳣�Ͽ������ݽ����߳̽���ֹ");
					return;
				}
				ZWNOTICE(recstr.c_str());
			}

			string outXML;
			jcAtmcConvertDLL::zwJCjson2CCBxml(recstr,outXML);
			ZWINFO("�������߻ش���Json��ת��Ϊ����XML�ɹ�");
			assert(outXML.length()>42);	//XML��ͷ�Ĺ̶�����38���ַ����������һ����ǩ�����Լ����źϼ�4���ַ�
			ZWDBGMSG(outXML.c_str());		

			{
				boost:: mutex:: scoped_lock lock( recv_mutex); 
				s_dbgReturn=outXML;
			}
			
			if (NULL!=zwCfg::g_WarnCallback)
			{
				//���ûص�����������Ϣ��Ȼ��͹ر����ӣ�����ͨ���̣߳�
				zwCfg::g_WarnCallback(outXML.c_str());
				ZWINFO("�ɹ��Ѵ����߽��յ������ݴ��ݸ��ص�����");
			}
			else
			{
				ZWWARN("�ص�����ָ��Ϊ�գ��޷����ûص�����")
			}
		} 		
		ZWINFO("��ͨ�����ݽ����߳������˳�");

	}	//try
	catch(...){
		ZWFATAL("��ͨ�����ݽ����߳�WebSocket���������쳣�Ͽ����������ݽ����߳̽�����");
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
		ZWFATAL("WebSocket����ָ��Ϊ�գ�");
		return;
	}
	try{
		zwccbthr::zwscthr->SendString(str);
	}
	catch(...)
	{
		ZWDBGMSG(__FUNCTION__);
		ZWDBGMSG("\t zwWebSocket Send String Exeception!20140805.1626");
		ZWFATAL("ͨ��WebSocket�������ݵ������쳣��������δ����")
	}

}