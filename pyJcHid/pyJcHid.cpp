// pyJcHid.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "pyJcHid.h"


// ���ǵ���������һ��ʾ��
PYJCHID_API int npyJcHid=0;

// ���ǵ���������һ��ʾ����
PYJCHID_API int fnpyJcHid(void)
{
	return 42;
}

// �����ѵ�����Ĺ��캯����
// �й��ඨ�����Ϣ������� pyJcHid.h
CpyJcHid::CpyJcHid()
{
	return;
}
