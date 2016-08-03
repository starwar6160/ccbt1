// jcLockLog.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include <cassert>
#include "jcLockLog.h"

//黑钥匙日志处理
//该函数通过下面的回调函数返回处理结果
int SwapTouchKeyLog(char * TouchKeyLogItem)
{
	assert(NULL!=TouchKeyLogItem);
	assert(strlen(TouchKeyLogItem)>0);
	if ((NULL==TouchKeyLogItem) || (strlen(TouchKeyLogItem)==0))
	{
		return -1;
	}
	return 0;
}
//设置回调函数的函数：
void SetReturnTouchKeyLog(ReturnTouchKeyLog _TouchKeyLogHandleFun)
{
	assert(NULL!=_TouchKeyLogHandleFun);
	if (NULL==_TouchKeyLogHandleFun)
	{
		return ;
	}
}

//////////////////////////////////////////////////////////////////////////
//红钥匙日志处理
void SetReturnLockLog(ReturnLockLog _LockLogHandleFun)
{
	assert(NULL!=_LockLogHandleFun);
	if (NULL==_LockLogHandleFun)
	{
		return ;
	}
}



int SwapLockLog(char * LockLogItem,int IsFinish)
{
	assert(NULL!=LockLogItem);
	assert(strlen(LockLogItem)>0);

	if ((NULL==LockLogItem) || (strlen(LockLogItem)==0))
	{
		return -2;
	}

	return 0;
}
