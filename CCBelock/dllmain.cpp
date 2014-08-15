// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <direct.h>
#include "zwCcbElockHdr.h"
Poco::LogStream *g_log=NULL;
int PocoLogInit(void)  ;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	PocoLogInit();
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		g_log->notice("JINCHU ELOCK DLL_PROCESS_ATTACH");
		break;
	case DLL_THREAD_ATTACH:
		g_log->notice("JINCHU ELOCK DLL_THREAD_ATTACH");
		break;
	case DLL_THREAD_DETACH:
		g_log->notice("JINCHU ELOCK DLL_THREAD_DETACH");
		break;
	case DLL_PROCESS_DETACH:
		g_log->notice("JINCHU ELOCK DLL_PROCESS_DETACH");
		break;
	}
	//g_log->notice()<<33<<"hello"<<endl;
	return TRUE;
}

string zwGetLogFileName(void)
{
	const string jcLogPath=".\\JinChuLog815";
	string logdate,logtime;
	_mkdir(jcLogPath.c_str());
	time_t now=time(NULL);
	time_t tail=now % 60;
	now=now-tail;
	zwGetDateTimeString(now,logdate,logtime);
	return jcLogPath+"\\"+logdate+"."+logtime+".txt";
}


int PocoLogInit(void)  
{  
	Poco::AutoPtr<Poco::Channel> channel;  
	{  

		Poco::AutoPtr<Poco::Channel> fileChannel(new Poco::FileChannel());  
		fileChannel->setProperty("path", zwGetLogFileName());
			//"c:\\wsap\\bin\\tmp\\jinchuElock201408151047.txt");  
		//"jinchuElock2014081510.txt");  
		fileChannel->setProperty("archive", "timestamp");  
		fileChannel->setProperty("compress", "true");  

		Poco::AutoPtr<Poco::PatternFormatter> patternFormatter(new Poco::PatternFormatter());  
		patternFormatter->setProperty("pattern", "%H:%M:%S:%i %s(%l): %t");  
		channel = new Poco::FormattingChannel(patternFormatter, fileChannel);  
	}  


	Poco::Logger::root().setChannel(channel.get());  
	Poco::Logger& logger = Poco::Logger::get("JinChuElock");  
	logger.setLevel(Poco::Message::PRIO_TRACE);
	g_log=new Poco::LogStream(logger); 
	return 0;  
}  

