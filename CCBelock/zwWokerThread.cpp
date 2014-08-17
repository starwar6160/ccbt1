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

	void wait(int milliseconds)
	{ 
		boost::this_thread::sleep(boost::posix_time::milliseconds(milliseconds));		
	} 

	//�������������߳�
	void ThreadHeartJump()
	{
		ZWFUNCTRACE
			try{
				while(1)
				{
					if (NULL!=zwscthr)
					{
						Sleep(1000*30);	//��಻����60�룬����Ͷ������ӣ�
						zwscthr->SendString("HEARTJUMP");
						OutputDebugStringA("HEART JUMP PACKAGE OVER ATMC DLL AND JINCHU ELOCK");
						
					}
				}
		}
		catch(...)
		{
			OutputDebugStringA("HEART JUMP THREAD FAIL!");
			return;
		}
	}

	//������֮���ͨѶ�߳�
	void ThreadLockComm()
	{		
	ZWFUNCTRACE
	boost:: mutex:: scoped_lock lock( thr_mutex); 
	try{
	string myLockIp="10.0.0.10";	//Ĭ��ֵ����ʵ����IP
	if (s_LockIp.length()>0)
	{
		myLockIp=s_LockIp;	//����������ļ���IPֵ����ʹ��֮��
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
			//����Ƿ�Ϊ������
			if (recstr=="HEARTJUMP")
			{
				//������������ķ����ַ�������ֱ�Ӻ��ԣ�
				continue;
			}

			char sbuf[128];
			memset(sbuf,0,128);
			sprintf(sbuf,"JCCOMM THREAD Comm with Lock times %d",i++);
			ZWTRACE(sbuf);
			string outXML;
			int msgTypeRecv=jcAtmcConvertDLL::zwJCjson2CCBxml(recstr,outXML);
			assert(outXML.length()>42);	//XML��ͷ�Ĺ̶�����38���ַ����������һ����ǩ�����Լ����źϼ�4���ַ�
			ZWTRACE(outXML.c_str());		

			{
				boost:: mutex:: scoped_lock lock( recv_mutex); 
				s_dbgReturn=outXML;
			}
			
			if (NULL!=zwCfg::g_WarnCallback)
			{
				//���ûص�����������Ϣ��Ȼ��͹ر����ӣ�����ͨ���̣߳�
				zwCfg::g_WarnCallback(outXML.c_str());
				//zwscthr->wsClose();
				break;
			}
			else
			{
				ZWFATAL("�ص�����ָ��Ϊ�գ��޷����ûص�����")
			}
			if (recstr.length()>9)
			{
				//MessageBoxA(NULL,recstr.c_str(),"ATMC ConcertDLL Received json is",MB_OK);			
			}

		} 		
		ZWTRACE("JCLOCK COMM THREAD NORMAL EXIT 20140817");

	}	//try
	catch(...){
		zwscthr->wsClose();	//�쳣�������ر����ӷ�ֹ��һ�����������޷�����
		OutputDebugStringA("JC CCB ELOCK THREAD CONNECT FAIL! 20140817");
		ZWFATAL("##########JC CCB ELOCK THREAD CONNECT FAIL! 20140817");
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
		// 1. ���������ļ�  
		AutoPtr<IniFileConfiguration> cfg(new IniFileConfiguration(cfgFileName));  

		// 2. ��ȡ�ڵ��ֵ  
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