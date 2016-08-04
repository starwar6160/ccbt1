#include "stdafx.h"
#include "jcLockLog.h"
#include "myLXLockDecHdr.h"

static ReturnLockLog		g_LockLogCallback=NULL;
//////////////////////////////////////////////////////////////////////////
//��Կ����־����
void SetReturnLockLog(ReturnLockLog _LockLogHandleFun)
{
	assert(NULL!=_LockLogHandleFun);
	if (NULL==_LockLogHandleFun)
	{
		return ;
	}
	g_LockLogCallback=_LockLogHandleFun;
}



int SwapLockLog(char * LockLogItem,int IsFinish)
{
	assert(NULL!=LockLogItem);
	assert(strlen(LockLogItem)>0);

	if ((NULL==LockLogItem) || (strlen(LockLogItem)==0))
	{
		return -2;
	}
	if (NULL==g_LockLogCallback)
	{
		MessageBox(NULL,TEXT("��Կ����־����ص�����δ����"),TEXT("����"),MB_OK);
		return -3;
	}
	string redkeyLogBin;
	myHex2Bin(LockLogItem,redkeyLogBin);
	assert(redkeyLogBin.length()>0);

	return 0;
}
