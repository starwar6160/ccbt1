// jcLockLog.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "jcLockLog.h"


// ���ǵ���������һ��ʾ��
JCLOCKLOG_API int njcLockLog=0;

// ���ǵ���������һ��ʾ����
JCLOCKLOG_API int fnjcLockLog(void)
{
	return 42;
}

// �����ѵ�����Ĺ��캯����
// �й��ඨ�����Ϣ������� jcLockLog.h
CjcLockLog::CjcLockLog()
{
	return;
}
