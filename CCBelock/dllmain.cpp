// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "zwCcbElockHdr.h"
void myLoadCfgs(const char *DLLPath);
extern Poco::LogStream * pocoLog;

HMODULE G_DLL_HMODULE = NULL;

#ifdef _DEBUG
string zwTest912(string now)
{
	time_t logGenData = boost::lexical_cast < time_t > (now);
	string exDate, exTime;
	zwGetLocalDateTimeString(logGenData, exDate, exTime);
	return exDate + "." + exTime;
}
#endif // _DEBUG

BOOL APIENTRY DllMain(HMODULE hModule,
		      DWORD ul_reason_for_call, LPVOID lpReserved)
{//
	try
	{ 
		//设置代码页为简体中文，936是简体中文的代码页。
		std::locale loc1 = std::locale::global(std::locale(".936"));
		OutputDebugStringA("JinChu Elock DllMain Loaded.20141017.1023");
		G_DLL_HMODULE = hModule;	//方便其他需要获取DLL绝对路径的地方使用；
		char myDllPath[256];
		memset(myDllPath, 0, 256);
		zwGetDLLPath(hModule, myDllPath, 256);
		OutputDebugStringA(myDllPath);
		myLoadCfgs(myDllPath);
#ifdef _DEBUG
		//cout<<"TEST912 LEXICAST.1557.\t"<<zwTest912("1409023024")<<endl;
#endif // _DEBUG
		switch (ul_reason_for_call) {
		case DLL_PROCESS_ATTACH:
			OutputDebugStringA("JINCHU ELOCK DLL_PROCESS_ATTACH");
			pocoLog->warning("JINCHU ELOCK DLL_PROCESS_ATTACH");
			break;
		case DLL_THREAD_ATTACH:
			OutputDebugStringA("JINCHU ELOCK DLL_THREAD_ATTACH");
			pocoLog->notice("JINCHU ELOCK DLL_THREAD_ATTACH");
			break;
		case DLL_THREAD_DETACH:
			OutputDebugStringA("JINCHU ELOCK DLL_THREAD_DETACH");
			pocoLog->notice("JINCHU ELOCK DLL_THREAD_DETACH");
			break;
		case DLL_PROCESS_DETACH:
			OutputDebugStringA("JINCHU ELOCK DLL_PROCESS_DETACH");
			pocoLog->warning("JINCHU ELOCK DLL_PROCESS_DETACH");
			break;
		}
		OutputDebugStringA("JinChu Elock DllMain Success End.20141017.1023");
		//恢复原来的代码页
		std::locale::global(std::locale(loc1));
	}
	catch(...)
	{
		//为了获取中文目录下Poco的日志库出异常时的问题
		printf("PocoLog in Chinese PathName Execption!20150108\n");
		printf("请把本ATMC DLL放到纯英文，无空格的路径下才能正确被调用，谢谢！Zhou Wei.20150108.\n");
		exit(2015);
	}

	return TRUE;
}
