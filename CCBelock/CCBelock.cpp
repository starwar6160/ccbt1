// CCBelock.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "CCBelock.h"
using namespace std;

//�ص�����ָ�����Ͷ���
typedef void (cdecl *RecvMsgRotine)(const char *pszMsg);

namespace zwCfg{
//#ifdef _DEBUG
	const long	JC_CCBDLL_TIMEOUT=30;	//���ʱʱ��Ϊ30��,���ڲ���Ŀ�ľ���ﵽ���Ʊ�¶����
	const int	JC_MSG_MAXLEN=128;	//�Ϊ128�ֽ�,���ڲ���Ŀ�ľ���ﵽ���Ʊ�¶����
//#else
//	const long	JC_CCBDLL_TIMEOUT=3600;	//���ʱʱ��Ϊ1��Сʱ������Ҳû��������
//	const int	JC_MSG_MAXLEN=128*1024;	//�Ϊ��λ��RAM�Ĵ�С������Ҳû��������
//#endif // _DEBUG
	//����һ���ص�����ָ��
	RecvMsgRotine g_WarnCallback=NULL;
}





CCBELOCK_API long Open(long lTimeOut)
{
	//�������0��С��JC_CCBDLL_TIMEOUT��������һ������Χ��
	assert(lTimeOut>0 && lTimeOut<zwCfg::JC_CCBDLL_TIMEOUT);
	if (lTimeOut<=0 || lTimeOut>=zwCfg::JC_CCBDLL_TIMEOUT)
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
	//������������ݣ�������󲻵ó�����λ���ڴ��С������������
	assert(NULL!=pszMsg);
	if (NULL==pszMsg)
	{
		return ELOCK_ERROR_PARAMINVALID;
	}
	int inlen=strlen(pszMsg);
	assert(inlen>0 && inlen<zwCfg::JC_MSG_MAXLEN);
	if (inlen==0 || inlen>=zwCfg::JC_MSG_MAXLEN )
	{
		return ELOCK_ERROR_PARAMINVALID;
	}
	//���ӣ�����Notify����һ�»ص�����
	if (NULL!=zwCfg::g_WarnCallback)
	{
		zwCfg::g_WarnCallback("MsgSend2ATMCThroughtCallBack");
	}
	return ELOCK_ERROR_SUCCESS;
}

void cdecl myATMCRecvMsgRotine(const char *pszMsg)
{
	//������������ݣ�������󲻵ó�����λ���ڴ��С������������
	assert(NULL!=pszMsg);
	int inlen=strlen(pszMsg);
	assert(inlen>0 && inlen<zwCfg::JC_MSG_MAXLEN);
	if (NULL==pszMsg || inlen==0 || inlen>=zwCfg::JC_MSG_MAXLEN )
	{
		return;
	}
}


CCBELOCK_API int SetRecvMsgRotine(RecvMsgRotine pRecvMsgFun)
{
	assert(NULL!=pRecvMsgFun);
	if (NULL==pRecvMsgFun)
	{
		return ELOCK_ERROR_PARAMINVALID;
	}
	zwCfg::g_WarnCallback=pRecvMsgFun;
	return ELOCK_ERROR_SUCCESS;
}