#include "stdafx.h"
#include "jcLockLog.h"
#include "myLXLockDecHdr.h"

static ReturnTouchKeyLog	g_TouchKeyLogCallback=NULL;
//黑钥匙日志处理

//设置黑钥匙回调函数的函数：
void SetReturnTouchKeyLog(ReturnTouchKeyLog _TouchKeyLogHandleFun)
{
	assert(NULL!=_TouchKeyLogHandleFun);
	if (NULL==_TouchKeyLogHandleFun)
	{
		return ;
	}
	g_TouchKeyLogCallback=_TouchKeyLogHandleFun;
}

int SwapTouchKeyLog(char * TouchKeyLogItem)
{
	assert(NULL!=TouchKeyLogItem);
	assert(strlen(TouchKeyLogItem)>0);
	if ((NULL==TouchKeyLogItem) || (strlen(TouchKeyLogItem)==0))
	{
		return -1;
	}
	if (NULL==g_TouchKeyLogCallback)
	{
		MessageBox(NULL,TEXT("黑钥匙日志解码回调函数未设置"),TEXT("错误"),MB_OK);
		return -4;
	}
	string blackKeyLogBin;
	myHex2Bin(TouchKeyLogItem,blackKeyLogBin);
	assert(blackKeyLogBin.length()>0);

	return 0;
}
