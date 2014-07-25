// CCBelock.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "CCBelock.h"
using namespace std;
const long JC_CCBDLL_TIMEOUT=3600;	//最长超时时间为1个小时，更长也没有意义了
const int	JC_MSG_MAXLEN=128*1024;	//最长为下位机RAM的大小，更大也没有意义了


CCBELOCK_API long Open(long lTimeOut)
{
	//必须大于0，小于JC_CCBDLL_TIMEOUT，限制在一个合理范围内
	assert(lTimeOut>0 && lTimeOut<JC_CCBDLL_TIMEOUT);
	if (lTimeOut<=0 || lTimeOut>=JC_CCBDLL_TIMEOUT)
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
	assert(NULL!=pszMsg);
	int inlen=strlen(pszMsg);
	assert(inlen>0 && inlen<JC_MSG_MAXLEN);
	if (NULL==pszMsg || inlen==0 || inlen>=JC_MSG_MAXLEN )
	{
		return ELOCK_ERROR_PARAMINVALID;
	}
	return ELOCK_ERROR_SUCCESS;
}

void cdecl myRecvMsgRotine(const char *pszMsg)
{
	assert(NULL!=pszMsg);
	int inlen=strlen(pszMsg);
	assert(inlen>0 && inlen<JC_MSG_MAXLEN);
	if (NULL==pszMsg || inlen==0 || inlen>=JC_MSG_MAXLEN )
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