#pragma once

// 计算机生成了由 Microsoft Visual C++ 创建的 IDispatch 包装类

// 注意: 不要修改此文件的内容。如果此类由
//  Microsoft Visual C++ 重新生成，您的修改将被覆盖。

/////////////////////////////////////////////////////////////////////////////
// CZjelockctrl1 包装类

class CZjelockctrl1 : public CWnd
{
protected:
	DECLARE_DYNCREATE(CZjelockctrl1)
public:
	CLSID const& GetClsid()
	{
		static CLSID const clsid
			= { 0xFBD9F9B5, 0x5A34, 0x4BD6, { 0x9F, 0xF1, 0xBD, 0xE2, 0x98, 0x68, 0x5A, 0xFB } };
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

// 特性
public:


// 操作
public:

// _DZJELock

// Functions
//

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
	void AboutBox()
	{
		InvokeHelper(DISPID_ABOUTBOX, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}

// Properties
//



};
