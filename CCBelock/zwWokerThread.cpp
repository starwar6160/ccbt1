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
		ZWFATAL("ͨ��WebSocket�������ݵ������쳣��������δ����")
	}
	
}


namespace zwccbthr{
	void ThreadLockComm();
	//�̶߳�����Ϊһ��ȫ�־�̬����������Ҫ��ʾ������������һ���߳�
	boost::thread t(ThreadLockComm); 
	boost:: mutex thr_mutex; //�û��Ᵽ��WebSocket���Ӷ���
	//���и��Ľӿڣ�û���������Ӳ����ĵط���Ҳ����˵����ȫ����д��IP�Ͷ˿ڣ��ֻ��Ǵ������ļ���ȡ
	zwWebSocket * zwscthr = NULL;


	void wait(int milliseconds)
	{ 
		boost::this_thread::sleep(boost::posix_time::milliseconds(milliseconds));		
	} 

	//������֮���ͨѶ�߳�
	void ThreadLockComm()
	{		
	ZWFUNCTRACE
	boost:: mutex:: scoped_lock lock( thr_mutex); 
	char *LockHost=getenv("JCLOCKIP");
	string myLockIp="10.0.0.10";	//Ĭ��ֵ����ʵ����IP
	ZWTRACE("JCLOCKIP=");
	if (NULL!=LockHost)
	{
		ZWTRACE(LockHost);
		myLockIp=LockHost;	//���������JCLOCKIP�������������ø�ֵ���
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
			assert(outXML.length()>42);	//XML��ͷ�Ĺ̶�����38���ַ����������һ����ǩ�����Լ����źϼ�4���ַ�
			ZWTRACE(outXML.c_str());		

			if (NULL!=zwCfg::g_WarnCallback)
			{
				zwCfg::g_WarnCallback(outXML.c_str());
			}
			else
			{
				ZWFATAL("�ص�����ָ��Ϊ�գ��޷����ûص�����")
			}

		} 		
		zwscthr->wsClose();
	}

//////////////////////////////////////////////////////////////////////////
}	//namespace zwccbthr{
