#include "stdafx.h"
#include <direct.h>
#include <WinBase.h>
#include "zwCcbElockHdr.h"

int PocoLogInit(void)  ;
string zwGetConfigFileName(void);
void testjson819(void);

Poco::LogStream *pocoLog=NULL;

string zwGetConfigFileName(void)
{
	HMODULE hdl=GetModuleHandleA("HidProtocol.dll");
	char dllName[256];
	memset(dllName,0,256);
	GetModuleFileNameA(hdl,dllName,256);
	char * tt=strstr(dllName,".dll");
	strcpy(tt,".ini");
	string cfgFileName=dllName;
	return cfgFileName;
}

string zwGetLogFileName(void)
{
	const string jcLogPath=".\\JinChuLog2014";
	string logdate,logtime;
	_mkdir(jcLogPath.c_str());
	time_t now=time(NULL);
	time_t tail=now % 10;
	now=now-tail;
	zwGetLocalDateTimeString(now,logdate,logtime);
	return jcLogPath+"\\"+logdate+"."+logtime+"JinChuLog.txt";
}


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


void myLoadConfigs()
{
	//在DLL的同一个目录下寻找同名的INI文件载入
	zwccbthr::myLoadConfig(zwGetConfigFileName());
	PocoLogInit();
}
