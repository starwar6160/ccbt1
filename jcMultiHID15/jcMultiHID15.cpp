// jcMultiHID15.cpp : ���� DLL Ӧ�ó���ĵ���������
//20150408.1609.����Ϊ�˰�֧�ֶ��HID�豸�Ĺ����ã�������DLL��
//������ĿATMC DLL�ֿ������ĵ���һ��DLL�����ò�����ȡ��zwBaseLib����

#include "stdafx.h"
#include "jcMultiHID15.h"


// ���ǵ���������һ��ʾ��
JCMULTIHID15_API int njcMultiHID15=0;

// ���ǵ���������һ��ʾ����
JCMULTIHID15_API int fnjcMultiHID15(void)
{
	return 42;
}

// �����ѵ�����Ĺ��캯����
// �й��ඨ�����Ϣ������� jcMultiHID15.h
CjcMultiHID15::CjcMultiHID15()
{
	return;
}
