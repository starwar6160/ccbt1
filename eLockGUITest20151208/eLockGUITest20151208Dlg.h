
// eLockGUITest20151208Dlg.h : ͷ�ļ�
//

#pragma once
#include "zjelockctrl1.h"


// CeLockGUITest20151208Dlg �Ի���
class CeLockGUITest20151208Dlg : public CDialogEx
{
// ����
public:
	CeLockGUITest20151208Dlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_ELOCKGUITEST20151208_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	// �Ͻ�ϵͳ��OCX
	CZjelockctrl1 m_zjOCX;
	afx_msg void OnBnClickedButton1();
	DECLARE_EVENTSINK_MAP()
	void OnRecvMsgZjelockctrl1(const VARIANT& varMsg);
};
