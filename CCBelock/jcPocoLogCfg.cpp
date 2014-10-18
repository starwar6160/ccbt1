#include "stdafx.h"
#include <direct.h>
#include <WinBase.h>
#include "zwCcbElockHdr.h"

string zwGetConfigFileName(void);
string zwGetLogFileName(void);
int PocoLogInit(void);

namespace zwccbthr{
	void myLoadConfig(const string &cfgFileName);
}	//namespace zwccbthr{


string zwGetConfigFileName(void)
{
	string cfgFileName="jinChuElock.ini";
	return cfgFileName;
}

string zwGetLogFileName(void)
{
	char myDllPath[256];
	memset(myDllPath,0,256);
	zwGetDLLPath(G_DLL_HMODULE,myDllPath,256);

	string jcLogPath=myDllPath;
	jcLogPath=jcLogPath+"\\JinChuLog2014";
	string logdate,logtime;
	_mkdir(jcLogPath.c_str());
	time_t now=time(NULL);
	time_t tail=now % 10;
	now=now-tail;
	zwGetLocalDateTimeString(now,logdate,logtime);
	return jcLogPath+"\\"+logdate+"."+logtime+"JinChuLog.txt";
}


Poco::LogStream *pocoLog=NULL;
int PocoLogInit(void)  
{  
	OutputDebugStringA(__FUNCTION__);
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

void myLoadCfgs( const char *DLLPath )
{
	//在DLL的同一个目录下寻找同名的INI文件载入
	string cfgPathName=DLLPath;
	cfgPathName=cfgPathName+"\\"+zwGetConfigFileName();
	//现在不用串口了，所以不用配置文件了，所以去掉，以免引起DLL Load失败。
	//20141018.1541.周伟
	//zwccbthr::myLoadConfig(cfgPathName);
	PocoLogInit();
}

namespace zwsTest1{
	int zwsTestAdd(int a,int b);
}

void zw901StaticLibTest1(void)
{
	cout<<"Result from zw Static Lib is "<<zwsTest1::zwsTestAdd(11,22)<<endl;
}
