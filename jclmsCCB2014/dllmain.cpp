// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"

static void myShowDLLVersion(const char * fnName)
{
	char version[128];
	memset(version,0,128);
	sprintf(version,"%s Version is %s %s\n",fnName,__DATE__,__TIME__);
	printf(version);
}
 
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{ 
	case DLL_PROCESS_ATTACH:
		myShowDLLVersion("jclmsCCB2014.DLL");
		break;
	case DLL_THREAD_ATTACH:
		myShowDLLVersion("jclmsCCB2014.DLL");
		break;
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

