
// eLockGUITest20151208Dlg.h : ͷ�ļ�
//

#pragma once
#include "zjelockctrl1.h"
#include <deque>
#include "afxwin.h"
using std::deque;

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
	afx_msg void OnClose();
	static UINT zw711SpeedTestThr1(LPVOID pParam);
	static UINT zw711SpeedTestThr2(LPVOID pParam);
private:
	// ��Ҫ���еı�������
	int m_runMsgNum;
	// ��ǰ�ظ���Ϣ���
	int m_curMsg;
	// ʧ������
	int m_failCount;
	// �ɹ���
	float m_failRate;
	//���ļ��ʱ�������
	int m_msgInvMs;
	CCriticalSection  m_secDqNotify;
	deque<string> m_dqNotify;
public:
	// ��ʼ���Եİ�ť
	CButton m_btnRun;
	// �ۼ�ִ���˶�����������
	CStatic m_lblAccMsgNum;
};
