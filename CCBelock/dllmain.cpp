// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <direct.h>
#include <WinBase.h>
#include "zwCcbElockHdr.h"

string zwGetLogFileName(void);
void myLoadCfgs();
extern Poco::LogStream *pocoLog;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	myLoadCfgs();

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		pocoLog->notice("JINCHU ELOCK DLL_PROCESS_ATTACH");
		break;
	case DLL_THREAD_ATTACH:
		pocoLog->notice("JINCHU ELOCK DLL_THREAD_ATTACH");
		break;
	case DLL_THREAD_DETACH:
		pocoLog->notice("JINCHU ELOCK DLL_THREAD_DETACH");
		break;
	case DLL_PROCESS_DETACH:
		pocoLog->notice("JINCHU ELOCK DLL_PROCESS_DETACH");
		break;
	}
	return TRUE;
}



Poco::LogStream *pocoLog=NULL;
int PocoLogInit(void)  
{  
	Poco::AutoPtr<Poco::Channel> channel;  
	{  

		Poco::AutoPtr<Poco::Channel> fileChannel(new Poco::FileChannel());  
		fileChannel->setProperty("compress", "true");  
		fileChannel->setProperty("path", 
			zwGetLogFileName()); 
		fileChannel->setProperty("archive", "number");  
		fileChannel->setProperty("purgeAge", "30 days");  
		fileChannel->setProperty("rotation", "6 hours");  
		fileChannel->setProperty("times", "local"); 

		Poco::AutoPtr<Poco::PatternFormatter> patternFormatter(new Poco::PatternFormatter());  
		patternFormatter->setProperty("pattern", "%H:%M:%S:%i %s(%l): %t");  
		channel = new Poco::FormattingChannel(patternFormatter, fileChannel);  
	}  


	Poco::Logger::root().setChannel(channel.get());  
	Poco::Logger& logger = Poco::Logger::get("LockDLL");  
	logger.setLevel(Poco::Message::PRIO_TRACE);
	pocoLog=new Poco::LogStream(logger); 
	return 0;  
}  






