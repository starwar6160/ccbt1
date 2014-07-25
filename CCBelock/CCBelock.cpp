// CCBelock.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "CCBelock.h"
using namespace std;

CCBELOCK_API long Open(long lTimeOut)
{
	assert(lTimeOut>0);
	if (lTimeOut<=0)
	{
		return ELOCK_ERROR_PARAMINVALID;
	}
	return ELOCK_ERROR_SUCCESS;
}

CCBELOCK_API long Close()
{
	return ELOCK_ERROR_SUCCESS;
}

CCBELOCK_API long Notify(const char *pszMsg)
{
	assert(NULL!=pszMsg && strlen(pszMsg)>0);
	if (NULL==pszMsg || strlen(pszMsg)==0)
	{
		return ELOCK_ERROR_PARAMINVALID;
	}
	return ELOCK_ERROR_SUCCESS;
}

void cdecl myRecvMsgRotine(const char *pszMsg)
{
	assert(NULL!=pszMsg && strlen(pszMsg)>0);
	if (NULL==pszMsg || strlen(pszMsg)==0)
	{
		return;
	}
}

typedef void (cdecl *RecvMsgRotine)(const char *pszMsg);

CCBELOCK_API int SetRecvMsgRotine(RecvMsgRotine pRecvMsgFun)
{
	assert(NULL!=pRecvMsgFun);
	if (NULL==pRecvMsgFun)
	{
		return ELOCK_ERROR_PARAMINVALID;
	}
	return ELOCK_ERROR_SUCCESS;
}