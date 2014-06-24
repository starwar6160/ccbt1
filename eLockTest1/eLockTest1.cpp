// eLockTest1.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
long Open(long lTimeOut);

#pragma comment(lib,"..\\YJHBIN\\CcbELockDll.lib")

int _tmain(int argc, _TCHAR* argv[])
{
	int opr=Open(500);
	return 0;
}

