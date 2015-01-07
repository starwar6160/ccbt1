#include "stdafx.h"

#include <winbase.h>
#include "zwCcbElockHdr.h"

string zwGetConfigFileName(void);
string zwGetLogFileName(HMODULE hmod,const char *head,const char *tail);
int PocoLogInit(void);

namespace zwccbthr {
	void myLoadConfig(const string & cfgFileName);
} //namespace zwccbthr{ 

string zwGetConfigFileName(void)
{
	string cfgFileName = "jinChuElock.ini";
	return cfgFileName;
}



Poco::LogStream * pocoLog = NULL;
int PocoLogInit(void)
{
	OutputDebugStringA(__FUNCTION__);
	Poco::AutoPtr < Poco::Channel > channel;
	{

		Poco::AutoPtr < Poco::Channel >
		    fileChannel(new Poco::FileChannel());
		fileChannel->setProperty("compress", "true");
		fileChannel->setProperty("path", zwGetLogFileName(G_DLL_HMODULE, "JC", "JinChuLog"));
		fileChannel->setProperty("archive", "number");
		fileChannel->setProperty("purgeAge", "30 days");
		fileChannel->setProperty("rotation", "6 hours");
		fileChannel->setProperty("times", "local");

		Poco::AutoPtr < Poco::PatternFormatter >
		    patternFormatter(new Poco::PatternFormatter());
		patternFormatter->setProperty("pattern",
					      "%H:%M:%S:%i %s(%l): %t");
		channel =
		    new Poco::FormattingChannel(patternFormatter, fileChannel);
	}

	Poco::Logger::root().setChannel(channel.get());
	Poco::Logger & logger = Poco::Logger::get("LockDLL");
	//logger.setLevel(Poco::Message::PRIO_TRACE);
	logger.setLevel(Poco::Message::PRIO_FATAL);
	pocoLog = new Poco::LogStream(logger);
	return 0;
}

void myLoadCfgs(const char *DLLPath)
{
	//在DLL的同一个目录下寻找同名的INI文件载入
	string cfgPathName = DLLPath;
	cfgPathName = cfgPathName + "\\" + zwGetConfigFileName();
	//现在不用串口了，所以不用配置文件了，所以去掉，以免引起DLL Load失败。
	//20141018.1541.周伟
	//zwccbthr::myLoadConfig(cfgPathName);
	PocoLogInit();
}

