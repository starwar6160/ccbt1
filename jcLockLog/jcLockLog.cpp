// jcLockLog.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include <cassert>
#include "jcLockLog.h"

//��Կ����־����
//�ú���ͨ������Ļص��������ش�����
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
//���ûص������ĺ�����
void SetReturnTouchKeyLog(ReturnTouchKeyLog _TouchKeyLogHandleFun)
{
	assert(NULL!=_TouchKeyLogHandleFun);
	if (NULL==_TouchKeyLogHandleFun)
	{
		return ;
	}
}

//////////////////////////////////////////////////////////////////////////
//��Կ����־����
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
