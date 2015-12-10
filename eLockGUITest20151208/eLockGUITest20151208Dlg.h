
// eLockGUITest20151208Dlg.h : 头文件
//

#pragma once
#include "zjelockctrl1.h"


// CeLockGUITest20151208Dlg 对话框
class CeLockGUITest20151208Dlg : public CDialogEx
{
// 构造
public:
	CeLockGUITest20151208Dlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_ELOCKGUITEST20151208_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	// 紫金系统的OCX
	CZjelockctrl1 m_zjOCX;
	afx_msg void OnBnClickedButton1();
	DECLARE_EVENTSINK_MAP()
	void OnRecvMsgZjelockctrl1(const VARIANT& varMsg);
};
