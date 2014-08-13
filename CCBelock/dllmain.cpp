// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"

int PocoLogTestMain(void)  ;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		OutputDebugStringA("JinChu.CCB.Elock.DLL Load 20140813.2123");
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	PocoLogTestMain();
	return TRUE;
}

int PocoLogTestMain(void)  
{  

	Poco::AutoPtr<Poco::Channel> channel;  
	{  
		Poco::AutoPtr<Poco::Channel> fileChannel(new Poco::FileChannel());  
		fileChannel->setProperty("path", "sample813.log");  
		fileChannel->setProperty("archive", "timestamp");  
		fileChannel->setProperty("compress", "true");  

		Poco::AutoPtr<Poco::PatternFormatter> patternFormatter(new Poco::PatternFormatter());  
		patternFormatter->setProperty("pattern", "%H:%M:%S:%i %s(%l): %t");  
		channel = new Poco::FormattingChannel(patternFormatter, fileChannel);  
	}  


	Poco::Logger::root().setChannel(channel.get());  
	Poco::Logger& logger = Poco::Logger::get("LoggerTest");  
	Poco::LogStream ls(logger);  
	ls.trace() << "This is a trace." << std::endl;  
	ls.warning() << "This is a warning." << std::endl;  
	ls.error() << "This is a error错误消息" << std::endl;  
	return 0;  
}  

