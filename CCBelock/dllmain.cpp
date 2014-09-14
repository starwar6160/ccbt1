// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "zwCcbElockHdr.h"
void myLoadCfgs();
extern Poco::LogStream * pocoLog;

#ifdef _DEBUG
string zwTest912(string now)
{
	time_t logGenData=boost::lexical_cast<time_t>(now);
	string exDate, exTime;
	zwGetLocalDateTimeString(logGenData, exDate, exTime);
	return exDate+"."+exTime;
}
#endif // _DEBUG



BOOL APIENTRY DllMain(HMODULE hModule,
		      DWORD ul_reason_for_call, LPVOID lpReserved)
{
	myLoadCfgs();
#ifdef _DEBUG
	//cout<<"TEST912 LEXICAST.1557.\t"<<zwTest912("1409023024")<<endl;
#endif // _DEBUG
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		pocoLog->warning("JINCHU ELOCK DLL_PROCESS_ATTACH");
		break;
	case DLL_THREAD_ATTACH:
		pocoLog->notice("JINCHU ELOCK DLL_THREAD_ATTACH");
		break;
	case DLL_THREAD_DETACH:
		pocoLog->notice("JINCHU ELOCK DLL_THREAD_DETACH");
		break;
	case DLL_PROCESS_DETACH:
		pocoLog->warning("JINCHU ELOCK DLL_PROCESS_DETACH");
		break;
	}
	return TRUE;
}


