// CCBelock.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "CCBelock.h"
using namespace std;
const long JC_CCBDLL_TIMEOUT=3600;	//���ʱʱ��Ϊ1��Сʱ������Ҳû��������
const int	JC_MSG_MAXLEN=128*1024;	//�Ϊ��λ��RAM�Ĵ�С������Ҳû��������


CCBELOCK_API long Open(long lTimeOut)
{
	//�������0��С��JC_CCBDLL_TIMEOUT��������һ������Χ��
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