// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
int zwLoadExtEXE(const char *exePathName,const char *exeCmdLine);

BOOL APIENTRY DllMain(HMODULE hModule,
		      DWORD ul_reason_for_call, LPVOID lpReserved)
{
	OutputDebugStringA("JCLMSCCB2014.DLL 20141125.1700");
	zwLoadExtEXE("C:\\Windows\\notepad.exe", NULL);
	
//////////////////////////////////////////////////////////////////////////
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
