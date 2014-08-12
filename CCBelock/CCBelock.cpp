// CCBelock.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "CCBelock.h"

CCBELOCK_API long Open(long lTimeOut)
{
	ZWFUNCTRACE
	return 624;
}

CCBELOCK_API long Close()
{
	ZWFUNCTRACE
	return 1544;
}

CCBELOCK_API long Notify(const char *pszMsg)
{
	ZWFUNCTRACE
	return 333;
}

void cdecl myRecvMsgRotine(const char *pszMsg)
{
	ZWFUNCTRACE
}

typedef void (cdecl *RecvMsgRotine)(const char *pszMsg);

CCBELOCK_API int SetRecvMsgRotine(RecvMsgRotine pRecvMsgFun)
{
	ZWFUNCTRACE
	return 0;
}