// CCBelock.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "CCBelock.h"

CCBELOCK_API long Open(long lTimeOut)
{
	return 624;
}

CCBELOCK_API long Close()
{
	return 1544;
}

CCBELOCK_API long Notify(const char *pszMsg)
{
	return 333;
}

void cdecl myRecvMsgRotine(const char *pszMsg)
{

}

typedef void (cdecl *RecvMsgRotine)(const char *pszMsg);

CCBELOCK_API int SetRecvMsgRotine(RecvMsgRotine pRecvMsgFun)
{
	return 0;
}