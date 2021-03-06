// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "zwCcbElockHdr.h"
//void myLoadCfgs(const char *DLLPath);
//extern Poco::LogStream * pocoLog;



HMODULE G_DLL_HMODULE = NULL;
uint32_t G_JCDBG_CODE=0;

#ifdef _DEBUG
string zwTest912(string now)
{
	time_t logGenData = boost::lexical_cast < time_t > (now);
	string exDate, exTime;
	zwGetLocalDateTimeString(logGenData, exDate, exTime);
	return exDate + "." + exTime;
}
#endif // _DEBUG

/////////////////////////////GLOG INIT START/////////////////////////////////////////////
void zwFatalExec(void)
{	
	cout<<"ZWGLOG FATAL ERROR FOUND!"<<endl;
	//exit(10000);
}

//设置环境变量为某个特定数字才能开启调试功能比如卸载锁具等等 74484053
uint32_t myJcDbgCodeGet1608(void)
{
	const char *jcDbgEnv= getenv("JCDEVDBG1608");
	if (NULL==jcDbgEnv)
	{
		return 0;
	}
	uint32_t jcDbgCode=0;
	sscanf(jcDbgEnv,"%u",&jcDbgCode);	
	return jcDbgCode;
}


#include <direct.h>
void zwGlogInit()
{
	static int runOnce=0;
	if (1==runOnce)
	{
		return;
	}
	runOnce=1;
	google::InitGoogleLogging("JCATMC154");    // 初始化
	google::InstallFailureFunction(zwFatalExec);	//安装默认FATAL处理函数，免得FATAL时直接非正常停止工作	
	//FLAGS_minloglevel=google::GLOG_WARNING;
	//FLAGS_minloglevel=google::GLOG_FATAL;
	FLAGS_minloglevel=google::GLOG_INFO;

	//用这一条，使得调试可以看到日志，但是不产生日志
	//FLAGS_logtostderr=true;	//输出到stderr代替输出到文件
	//////////////////////////////////////////////////////////////////////////
	const char *zwLogDir=".\\JCLOG423";
	_mkdir(zwLogDir);
	FLAGS_log_dir=zwLogDir;	//设定在指定目录下记录日志
	FLAGS_alsologtostderr=true;	//同时输出到stderr和文件;
	FLAGS_max_log_size=1;	//最大日志文件大小多少M字节，之后就会自动生成新的日志文件，以便前面的日志被查看或者备份
	FLAGS_colorlogtostderr=true;	//如果终端支持，采用彩色输出
	FLAGS_logbufsecs=1;	//日志缓冲1秒输出，有利于大量日志输出时的性能，我认为
	FLAGS_v=3;	//只有VLOG值小于等于该数字，才会被输出，尤其适合于调试日志
	//
	//google::SetLogDestination(google::GLOG_INFO,"./zw1231");  
#ifdef _DEBUG113
	LOG(WARNING)<<"Zhou Wei 20141231.1353 test1";
	VLOG(4)<<"VLOG 4";
	VLOG(3)<<"VLOG 3";
	VLOG(2)<<"VLOG 2";
	VLOG(1)<<"VLOG 1";
	for (int i=0;i<10;i++)
	{
		LOG(INFO)<<"zhouwei use google glog 20150108.1635.info "<<i<<" line";
	}

	LOG(FATAL)<<"zhouwei use google glog 20141231.1354.FATAL";
	LOG(WARNING)<<"zhouwei use google glog 20141231.1354.warn";
	LOG(ERROR)<<"zhouwei use google glog 20141231.1354.ERROR";
#endif // _DEBUG
	//使用FATAL级别会直接导致程序退出。。看来这就是FATAL的含义了

}

/////////////////////////////GLOG INIT END/////////////////////////////////////////////



BOOL APIENTRY DllMain(HMODULE hModule,
		      DWORD ul_reason_for_call, LPVOID lpReserved)
{//
	try
	{ 
		zwGlogInit();
		static time_t myBaseTime=0;
		time_t curTime=time(NULL);
		const int MYTIMEOUT=10;
		LOG_IF(WARNING,curTime-myBaseTime>MYTIMEOUT)<<"ATMC DLL LOADED 20160715.1510"<<endl;
		if (curTime-myBaseTime>MYTIMEOUT)
		{
			myBaseTime=curTime;
		}		
		//设置代码页为简体中文，936是简体中文的代码页。
		std::locale loc1 = std::locale::global(std::locale(".936"));
		//OutputDebugStringA("JinChu Elock DllMain Loaded.20151210.1523A01");
		G_DLL_HMODULE = hModule;	//方便其他需要获取DLL绝对路径的地方使用；
		char myDllPath[256];
		memset(myDllPath, 0, 256);
		zwGetDLLPath(hModule, myDllPath, 256);		
		G_JCDBG_CODE=myJcDbgCodeGet1608();
		//myLoadCfgs(myDllPath);
#ifdef _DEBUG
		//cout<<"TEST912 LEXICAST.1557.\t"<<zwTest912("1409023024")<<endl;
#else
		OutputDebugStringA(myDllPath);
#endif // _DEBUG
		switch (ul_reason_for_call) {
		case DLL_PROCESS_ATTACH:
			//OutputDebugStringA("JINCHU ELOCK DLL_PROCESS_ATTACH");
			//LOG(WARNING)<<"JINCHU ELOCK DLL_PROCESS_ATTACH 20150116.1717"<<endl;
			break;
		case DLL_THREAD_ATTACH:
			//OutputDebugStringA("JINCHU ELOCK DLL_THREAD_ATTACH");
			//LOG(WARNING)<<"JINCHU ELOCK DLL_THREAD_ATTACH 20150116.1717"<<endl;
			break;
		case DLL_THREAD_DETACH:
			//OutputDebugStringA("JINCHU ELOCK DLL_THREAD_DETACH");
			//LOG(WARNING)<<"JINCHU ELOCK DLL_THREAD_DETACH 20150116.1717"<<endl;
			break;
		case DLL_PROCESS_DETACH:
			//OutputDebugStringA("JINCHU ELOCK DLL_PROCESS_DETACH");
			//LOG(WARNING)<<"JINCHU ELOCK DLL_PROCESS_DETACH 20150116.1717"<<endl;
			break;
		}
		//OutputDebugStringA("JinChu Elock DllMain Success End.20151210.1517");
		//恢复原来的代码页
		std::locale::global(std::locale(loc1));
	}
	catch(...)
	{
		//为了获取中文目录下Poco的日志库出异常时的问题
		//printf("PocoLog in Chinese PathName Execption!20150108\n");
		printf("请把本ATMC DLL放到纯英文，无空格的路径下才能正确被调用，谢谢！Zhou Wei.20150108.\n");
		exit(2015);
	}

	return TRUE;
}

namespace zwtest504
{
void zwtest504hidClass(void);
void zwtest514thr1(void);
void zwtest514Main(void);
}

