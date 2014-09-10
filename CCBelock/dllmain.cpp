// dllmain.cpp : ���� DLL Ӧ�ó������ڵ㡣
#include "stdafx.h"
#include "zwPocoLog.h"
void myLoadCfgs();

BOOL APIENTRY DllMain(HMODULE hModule,
		      DWORD ul_reason_for_call, LPVOID lpReserved)
{
	myLoadCfgs();

	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		ZWWARN("JINCHU ELOCK DLL_PROCESS_ATTACH");
		break;
	case DLL_THREAD_ATTACH:
		ZWWARN("JINCHU ELOCK DLL_THREAD_ATTACH");
		break;
	case DLL_THREAD_DETACH:
		ZWWARN("JINCHU ELOCK DLL_THREAD_DETACH");
		break;
	case DLL_PROCESS_DETACH:
		ZWWARN("JINCHU ELOCK DLL_PROCESS_DETACH");
		break;
	}
	return TRUE;
}



//��ATMC DLL��XML��JSON��ת���������ڴˣ����ڵ�Ԫ���ԣ�
namespace jcAtmcConvertDLL {
	const char *LOCKMAN_NAME = "BeiJing.JinChu";
	string ns_ActReqName;
	string ns_LockInitName;
	string ns_ReadCloseCodeName;
	string ns_ccbAtmno;	//ATM���

}				//namespace jcAtmcConvertDLL
