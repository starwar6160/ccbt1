// eLockTest1.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
long Open(long lTimeOut);

#pragma comment(lib,"..\\YJHBIN\\CcbELockDll.lib")

int _tmain(int argc, _TCHAR* argv[])
{
	int opr=Open(500);
	return 0;
}

