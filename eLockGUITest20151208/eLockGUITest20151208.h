
// eLockGUITest20151208.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CeLockGUITest20151208App:
// �йش����ʵ�֣������ eLockGUITest20151208.cpp
//

class CeLockGUITest20151208App : public CWinApp
{
public:
	CeLockGUITest20151208App();

// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CeLockGUITest20151208App theApp;