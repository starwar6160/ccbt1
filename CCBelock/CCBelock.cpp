// CCBelock.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "CCBelock.h"

CCBELOCK_API long Open(long lTimeOut)
{
	return ELOCK_ERROR_SUCCESS;
}

CCBELOCK_API long Close()
{
	return ELOCK_ERROR_SUCCESS;
}

CCBELOCK_API long Notify(const char *pszMsg)
{
	return ELOCK_ERROR_SUCCESS;
}

void cdecl myRecvMsgRotine(const char *pszMsg)
{

}

typedef void (cdecl *RecvMsgRotine)(const char *pszMsg);

CCBELOCK_API int SetRecvMsgRotine(RecvMsgRotine pRecvMsgFun)
{
	return ELOCK_ERROR_SUCCESS;
}