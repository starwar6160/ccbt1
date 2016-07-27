
// eLockGUITest20151208Dlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "eLockGUITest20151208.h"
#include "eLockGUITest20151208Dlg.h"
#include "afxdialogex.h"
#include "zwLibTools.h"
using zwLibTools2015::myGetUs;
void myStr2Bstr(const char *strIn,VARIANT &bstrOut);
string zwGetJcxmlMsgType(const char *jcXML);
bool myIsJsonMsgFromLockFirstUp(const string &jcMsg);
bool myIsXMLMsgCodeFromLockFirstUp(const string &jcMsg);
extern const char *g_msg00;
extern const char *g_msg01;
extern const char *g_msg02;
extern const char *g_msg03;
extern const char *g_msg04;



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
	EnableActiveAccessibility();
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CeLockGUITest20151208Dlg �Ի���




CeLockGUITest20151208Dlg::CeLockGUITest20151208Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CeLockGUITest20151208Dlg::IDD, pParent)
	, m_runMsgNum(0)
	, m_curMsg1(0)
	, m_curMsg2(0)
	, m_failCount1(0)
	, m_failCount2(0)
	, m_failRate1(0)
	, m_failRate2(0)
	,m_msgInvMs(0)
{
	EnableActiveAccessibility();
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CeLockGUITest20151208Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ZJELOCKCTRL1, m_zjOCX);
	DDX_Text(pDX, IDC_EDIT_MSGNUM, m_runMsgNum);
	DDX_Text(pDX, IDC_LBL_CURMSG1, m_curMsg1);
	DDX_Text(pDX, IDC_LBL_CURMSG2, m_curMsg2);
	DDX_Text(pDX, IDC_LBL_FAILCOUNT1, m_failCount1);
	DDX_Text(pDX, IDC_LBL_FAILCOUNT2, m_failCount2);
	DDX_Text(pDX, IDC_LBL_FAILRATE1, m_failRate1);
	DDX_Text(pDX, IDC_LBL_FAILRATE2, m_failRate2);
	DDX_Text(pDX, IDC_EDT_INVMS, m_msgInvMs);

	DDX_Control(pDX, IDC_BTNRUN, m_btnRun);
	DDX_Control(pDX, IDC_LBLCURITEM, m_lblAccMsgNum);
}

BEGIN_MESSAGE_MAP(CeLockGUITest20151208Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CeLockGUITest20151208Dlg::OnBnClickedButton1)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CeLockGUITest20151208Dlg ��Ϣ�������

BOOL CeLockGUITest20151208Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	m_runMsgNum=300;
	m_curMsg1=0;
	m_failCount1=0;
	m_failRate1=0.0f;
	m_msgInvMs=200;
	UpdateData(FALSE);
	m_btnRun.SetFocus();
	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	DWORD iOpen=m_zjOCX.Open(22);
	if (0!=iOpen)
	{
		MessageBoxA(NULL,"�𴢵�����������ʧ��","ʧ��",MB_OK);
	}
	m_lblAccMsgNum.SetWindowText(TEXT("�ۼ�����"));
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CeLockGUITest20151208Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CeLockGUITest20151208Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CeLockGUITest20151208Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CeLockGUITest20151208Dlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������	
	// 
	// 
	// std::string to variant.
	VARIANT tmpMsg;
	myStr2Bstr(g_msg04,tmpMsg);	
	m_zjOCX.Notify(tmpMsg);
	UpdateData(TRUE);
	if (m_runMsgNum<=0)
	{
		m_runMsgNum=1;
	}
	AfxBeginThread(zw711SpeedTestThr1, this);
	AfxBeginThread(zw711SpeedTestThr2, this);	
	m_btnRun.EnableWindow(FALSE);
}
BEGIN_EVENTSINK_MAP(CeLockGUITest20151208Dlg, CDialogEx)
	ON_EVENT(CeLockGUITest20151208Dlg, IDC_ZJELOCKCTRL1, 1, CeLockGUITest20151208Dlg::OnRecvMsgZjelockctrl1, VTS_VARIANT)
END_EVENTSINK_MAP()


void CeLockGUITest20151208Dlg::OnRecvMsgZjelockctrl1(const VARIANT& varMsg)
{
	// TODO: �ڴ˴������Ϣ����������
	char *rMsg=_com_util::ConvertBSTRToString(varMsg.bstrVal);
	string sType=zwGetJcxmlMsgType(rMsg);
	int64_t nowUs=myGetUs();
	char timeStampBuf[32];
	memset(timeStampBuf,0,32);
	sprintf(timeStampBuf,"%s%lld","TIPZW1216 ",nowUs);
	int rMsgLen=strlen(rMsg);
	assert(rMsgLen<768);
	OutputDebugStringA(rMsg);
	UpdateData(FALSE);

	bool upMatched=false;
	string upMsgType;
	upMsgType=zwGetJcxmlMsgType(rMsg);
	m_secDqNotify.Lock();
	if (m_dqNotifyT2.size()>0 && false==upMatched)
	{
		string downMsgType;
		downMsgType=m_dqNotifyT2.front()->msgType;
		if (downMsgType==upMsgType
			&& !myIsXMLMsgCodeFromLockFirstUp(upMsgType))
		{
			m_dqNotifyT2.front()->msgDiffUs=(myGetUs()-m_dqNotifyT2.front()->msgStartUs)/1000.0f;
			m_vecTimeStatus.push_back(m_dqNotifyT2.front());
			upMatched=true;			
		}	
		if (downMsgType!=upMsgType
			&& !myIsXMLMsgCodeFromLockFirstUp(upMsgType))
		{
			m_failCount2++;					
		}
		m_dqNotifyT2.pop_front();
	}
	if (m_dqNotifyT1.size()>0 && false==upMatched)
	{
		string downMsgType;
		downMsgType=m_dqNotifyT1.front()->msgType;		
		if (downMsgType==upMsgType
			&& !myIsXMLMsgCodeFromLockFirstUp(upMsgType))
		{
			m_dqNotifyT1.front()->msgDiffUs=(myGetUs()-m_dqNotifyT1.front()->msgStartUs)/1000.0f;
			m_vecTimeStatus.push_back(m_dqNotifyT1.front());
			upMatched=true;			
		}		
		if (downMsgType!=upMsgType
			&& !myIsXMLMsgCodeFromLockFirstUp(upMsgType))
		{
			m_failCount1++;					
			string myErrMsg="CCBMFC����722.downMsgType="+downMsgType+"upMsg="+rMsg;
			OutputDebugStringA(myErrMsg.c_str());			 
		}
		m_dqNotifyT1.pop_front();
	}	
	if (m_curMsg1>=m_runMsgNum)
	{
		m_btnRun.EnableWindow(TRUE);
	}
		m_failRate1=100.0f*(m_failCount1)/(m_curMsg1+0.001f);
		m_failRate1=ceil(m_failRate1*100)/100.0f;
		m_failRate2=100.0f*(m_failCount2)/(m_curMsg2+0.001f);
		m_failRate2=ceil(m_failRate2*100)/100.0f;
	m_secDqNotify.Unlock();	
	
	//MessageBoxA(NULL,rMsg,timeStampBuf,MB_OK);
	UpdateData(FALSE);
	
}


void CeLockGUITest20151208Dlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	Sleep(800);
	m_zjOCX.Close();
	CDialogEx::OnClose();
}


