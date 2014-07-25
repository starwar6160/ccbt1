// CCBelock.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "CCBelock.h"
using namespace std;

//回调函数指针类型定义
typedef void (cdecl *RecvMsgRotine)(const char *pszMsg);

namespace zwCfg{
//#ifdef _DEBUG
	const long	JC_CCBDLL_TIMEOUT=30;	//最长超时时间为30秒,用于测试目的尽快达到限制暴露问题
	const int	JC_MSG_MAXLEN=128;	//最长为128字节,用于测试目的尽快达到限制暴露问题
//#else
//	const long	JC_CCBDLL_TIMEOUT=3600;	//最长超时时间为1个小时，更长也没有意义了
//	const int	JC_MSG_MAXLEN=128*1024;	//最长为下位机RAM的大小，更大也没有意义了
//#endif // _DEBUG
	//定义一个回调函数指针
	RecvMsgRotine g_WarnCallback=NULL;
}





CCBELOCK_API long Open(long lTimeOut)
{
	//必须大于0，小于JC_CCBDLL_TIMEOUT，限制在一个合理范围内
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
	//输入必须有内容，但是最大不得长于下位机内存大小，做合理限制
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
	//例子，利用Notify测试一下回调函数
	if (NULL!=zwCfg::g_WarnCallback)
	{
		zwCfg::g_WarnCallback("MsgSend2ATMCThroughtCallBack");
	}
	return ELOCK_ERROR_SUCCESS;
}

void cdecl myATMCRecvMsgRotine(const char *pszMsg)
{
	//输入必须有内容，但是最大不得长于下位机内存大小，做合理限制
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