// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "boostLogger.h"
void myLoadCfgs();
//extern Poco::LogStream * pocoLog;

BOOL APIENTRY DllMain(HMODULE hModule,
		      DWORD ul_reason_for_call, LPVOID lpReserved)
{
	//初始化日志库
	g_InitLog(debug, debug);
	vdsu_logger_mt& vlg = vdsu_logger::get();
	BOOST_LOG_SEV(vlg, info) << "Hello World!,using BOOST.LOG 20140910.1526";
	myLoadCfgs();

	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		//pocoLog->warning("JINCHU ELOCK DLL_PROCESS_ATTACH");
		break;
	case DLL_THREAD_ATTACH:
		//pocoLog->notice("JINCHU ELOCK DLL_THREAD_ATTACH");
		break;
	case DLL_THREAD_DETACH:
		//pocoLog->notice("JINCHU ELOCK DLL_THREAD_DETACH");
		break;
	case DLL_PROCESS_DETACH:
		//pocoLog->warning("JINCHU ELOCK DLL_PROCESS_DETACH");
		break;
	}
	return TRUE;
}


