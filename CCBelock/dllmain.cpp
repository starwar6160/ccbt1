// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "zwCcbElockHdr.h"
void myLoadCfgs(const char *DLLPath);
extern Poco::LogStream * pocoLog;

HMODULE G_DLL_HMODULE=NULL;



#ifdef _DEBUG
string zwTest912(string now)
{
	time_t logGenData=boost::lexical_cast<time_t>(now);
	string exDate, exTime;
	zwGetLocalDateTimeString(logGenData, exDate, exTime);
	return exDate+"."+exTime;
}
#endif // _DEBUG

void zwGetDLLPath(HMODULE hDLL,char *pDllPath,const int dllPathLen)
{
	//assert(	NULL!=hDLL );
	assert(NULL!=pDllPath);
	assert(dllPathLen>0);
	if (NULL==hDLL)
	{
		return;
	}
	GetModuleFileNameA(hDLL,pDllPath,dllPathLen);
	int dllNameLen=strlen(pDllPath);
	//从最后一个字节找起,找到"HidProtocol.dll"之前的'\\'字符就将其阶段
	//之所以不用strstr查找DLL文件名是因为不确定其有多少种大小写组合形式
	//要是统一变为大写，又破坏了路径名的大小写，可能会造成潜在问题，
	//尽管windows文件系统不区分大小写
	for (int i=dllNameLen-1;i>0;i--)
	{
		if (pDllPath[i]=='\\')
		{
			pDllPath[i]=NULL;
			break;
		}
	}
}

BOOL APIENTRY DllMain(HMODULE hModule,
		      DWORD ul_reason_for_call, LPVOID lpReserved)
{
	G_DLL_HMODULE=hModule;	//方便其他需要获取DLL绝对路径的地方使用；
	char myDllPath[256];
	memset(myDllPath,0,256);
	zwGetDLLPath(hModule,myDllPath,256);
	myLoadCfgs(myDllPath);	
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


