#include "stdafx.h"
#include <direct.h>
#include <WinBase.h>
#include "zwCcbElockHdr.h"
#include "zwPocoLog.h"

string zwGetConfigFileName(void);
string zwGetLogFileName(void);
int PocoLogInit(void);

namespace zwccbthr{
	void myLoadConfig(const string &cfgFileName);
}	//namespace zwccbthr{


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


#ifdef _DEBUG_POCOLOG910
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
#endif // _DEBUG_POCOLOG910
void zwLogInit();
void myLoadCfgs()
{
	//在DLL的同一个目录下寻找同名的INI文件载入
	zwccbthr::myLoadConfig(zwGetConfigFileName());
	//PocoLogInit();
	zwLogInit();
}

namespace zwsTest1{
	int zwsTestAdd(int a,int b);
}

void zw901StaticLibTest1(void)
{
	cout<<"Result from zw Static Lib is "<<zwsTest1::zwsTestAdd(11,22)<<endl;
}

void zwFatalExec(void)
{
	cout<<"ZWGLOG FATAL ERROR FOUND!"<<endl;
	exit(10000);
}

static bool g_glog_inited=false;

void zwLogInit()
{
	if (true==g_glog_inited)
	{
		return;
	}
	g_glog_inited=true;
	google::InitGoogleLogging("JCELOCK.DLL");    // 初始化
	google::InstallFailureFunction(zwFatalExec);	//安装默认FATAL处理函数，免得FATAL时直接非正常停止工作	
	//FLAGS_minloglevel=google::GLOG_WARNING;
	FLAGS_minloglevel=google::GLOG_FATAL;
	//FLAGS_minloglevel=google::GLOG_INFO;

	//用这一条，使得调试可以看到日志，但是不产生日志
	FLAGS_logtostderr=true;	//输出到stderr代替输出到文件
	//////////////////////////////////////////////////////////////////////////
	FLAGS_log_dir=".";	//设定在当前目录下记录日志
	//FLAGS_alsologtostderr=true;	//同时输出到stderr和文件;
	//FLAGS_max_log_size=4;	//最大日志文件大小4M
	FLAGS_colorlogtostderr=true;	//如果终端支持，采用彩色输出
	FLAGS_logbufsecs=1;	//日志缓冲1秒输出，有利于大量日志输出时的性能，我认为
	FLAGS_v=4;	//只有VLOG值小于等于该数字，才会被输出，尤其适合于调试日志
	//
	//google::SetLogDestination(google::GLOG_INFO,"./zwjclmstmp");  
	LOG(WARNING)<<"Zhou Wei 20140616.1349b.JCLMSCORE.DLL Loaded!";
#ifdef _DEBUG_TESTGLOG411
	LOG(INFO)<<"zhouwei use google glog 20140411.1146.info";
	LOG(WARNING)<<"zhouwei use google glog 20140411.1146.warn";
	LOG(ERROR)<<"zhouwei use google glog 20140411.1146.ERROR";
	//使用FATAL级别会直接导致程序退出。。看来这就是FATAL的含义了
	LOG(FATAL)<<"zhouwei use google glog 20140411.1146.FATAL";
#endif // _DEBUG_TESTGLOG411

}
