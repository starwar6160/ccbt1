#pragma once

// ������������� Microsoft Visual C++ ������ IDispatch ��װ��

// ע��: ��Ҫ�޸Ĵ��ļ������ݡ����������
//  Microsoft Visual C++ �������ɣ������޸Ľ������ǡ�

/////////////////////////////////////////////////////////////////////////////
// CJcelock16ctrl1 ��װ��

class CJcelock16ctrl1 : public CWnd
{
protected:
	DECLARE_DYNCREATE(CJcelock16ctrl1)
public:
	CLSID const& GetClsid()
	{
		static CLSID const clsid
			= { 0xED9E331A, 0xB93C, 0x4EC8, { 0x9E, 0x89, 0x77, 0xC4, 0x86, 0xED, 0xA9, 0x10 } };
		return clsid;
	}
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle,
						const RECT& rect, CWnd* pParentWnd, UINT nID, 
						CCreateContext* pContext = NULL)
	{ 
		return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID); 
	}

    BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, 
				UINT nID, CFile* pPersist = NULL, BOOL bStorage = FALSE,
				BSTR bstrLicKey = NULL)
	{ 
		return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID,
		pPersist, bStorage, bstrLicKey); 
	}

// ����
public:


// ����
public:

// _DJCElock16

// Functions
//

	void AboutBox()
	{
		InvokeHelper(DISPID_ABOUTBOX, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	long Open(long lTimeOut)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x1, DISPATCH_METHOD, VT_I4, (void*)&result, parms, lTimeOut);
		return result;
	}
	long Close()
	{
		long result;
		InvokeHelper(0x2, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long Notify(VARIANT varMsg)
	{
		long result;
		static BYTE parms[] = VTS_VARIANT ;
		InvokeHelper(0x3, DISPATCH_METHOD, VT_I4, (void*)&result, parms, &varMsg);
		return result;
	}

// Properties
//



};
