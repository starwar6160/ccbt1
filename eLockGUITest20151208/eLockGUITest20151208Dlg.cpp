
// eLockGUITest20151208Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "eLockGUITest20151208.h"
#include "eLockGUITest20151208Dlg.h"
#include "afxdialogex.h"
#include "zwLibTools.h"

using zwLibTools2015::myGetUs;

UINT MyThreadProc( LPVOID pParam );
UINT zw711SpeedTestThr1(LPVOID pParam);

//获取XML报文类型
//临时复制过来的代码，因为该段代码在DLL中被上层应用调用会导致堆栈破坏，原因暂时未知
// 20151216.1702.周伟
string zwGetJcxmlMsgType(const char *jcXML) 
{
	assert(NULL!=jcXML);
	assert(strlen(jcXML)>0);
	ptree ptccb;
	std::stringstream ss;
	ss << jcXML;
	read_xml(ss, ptccb);
	string msgType=ptccb.get<string>("root.TransCode");		
	assert(msgType.size()>0);
	return msgType;
}

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static const char *g_msg00="<?xml version='1.0' encoding='UTF-8'?><root><TransCode>0000</TransCode><TransName>CallForActInfo</TransName><TransDate>20160708</TransDate><TransTime>110900</TransTime><DevCode>515067001098</DevCode><SpareString1></SpareString1><SpareString2></SpareString2></root>";
static const char *g_msg01="<?xml version='1.0' encoding='UTF-8'?><root><TransCode>0001</TransCode><TransName>SendActInfo</TransName><TransDate>20160708</TransDate><TransTime>110900</TransTime><DevCode>515067001098</DevCode><LockMan>BeiJing.JinChu</LockMan><LockId>22222222</LockId><ActInfo>BFr4af4YvXxtsGmJnDCKsZ3OhmkZimSur0itl6fwuc3fqkiK6j05arPl2on3N4rfVLQkfo9GRceMmbXDebf7rdY=.h/UOVQjtcby5I3wQyUXDdB/uDTeUq1oW.vNDgbpmArJ2CTkBSAJ0NKDeZ6vUifPLKjbLZH3eiFE+QkEBOy5+r2ZJEYEgBmjD1KGIbVfridTa3sjuqaFo0lby7YpqBXTo56v5yIzyz28k=</ActInfo><SpareString1></SpareString1><SpareString2></SpareString2></root>";
static const char *g_msg02="<?xml version='1.0' encoding='UTF-8'?><root><TransCode>0002</TransCode><TransName>QueryForLockStatus</TransName><TransDate>20160708</TransDate><TransTime>110900</TransTime><DevCode>515067001098</DevCode><LockMan></LockMan><LockId></LockId><SpareString1></SpareString1><SpareString2></SpareString2></root>";
static const char *g_msg03="<?xml version='1.0' encoding='UTF-8'?><root><TransCode>0003</TransCode><TransName>TimeSync</TransName><TransDate>20160708</TransDate><TransTime>110900</TransTime></root>";
static const char *g_msg04="<?xml version='1.0' encoding='UTF-8'?><root><TransCode>0004</TransCode><TransName>ReadShutLockCode</TransName><TransDate>20160708</TransDate><TransTime>110900</TransTime><DevCode>515067001098</DevCode><SpareString1></SpareString1><SpareString2></SpareString2></root>"; 

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CeLockGUITest20151208Dlg 对话框




CeLockGUITest20151208Dlg::CeLockGUITest20151208Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CeLockGUITest20151208Dlg::IDD, pParent)
{
	EnableActiveAccessibility();
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CeLockGUITest20151208Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ZJELOCKCTRL1, m_zjOCX);
}

BEGIN_MESSAGE_MAP(CeLockGUITest20151208Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CeLockGUITest20151208Dlg::OnBnClickedButton1)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CeLockGUITest20151208Dlg 消息处理程序

BOOL CeLockGUITest20151208Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	DWORD iOpen=m_zjOCX.Open(22);
	if (0!=iOpen)
	{
		MessageBoxA(NULL,"金储电子密码锁打开失败","失败",MB_OK);
	}
	
	AfxBeginThread(zw711SpeedTestThr1, &m_zjOCX);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CeLockGUITest20151208Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CeLockGUITest20151208Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void myStr2Bstr(const char *strIn,VARIANT &bstrOut)
{
	_bstr_t strMessage = strIn;
	bstrOut.vt=VT_BSTR;
	bstrOut.bstrVal= strMessage;	
}


void CeLockGUITest20151208Dlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码	
	// 
	// 
	// std::string to variant.
	VARIANT tmpMsg;
	myStr2Bstr(g_msg04,tmpMsg);	
	m_zjOCX.Notify(tmpMsg);

}
BEGIN_EVENTSINK_MAP(CeLockGUITest20151208Dlg, CDialogEx)
	ON_EVENT(CeLockGUITest20151208Dlg, IDC_ZJELOCKCTRL1, 1, CeLockGUITest20151208Dlg::OnRecvMsgZjelockctrl1, VTS_VARIANT)
END_EVENTSINK_MAP()


void CeLockGUITest20151208Dlg::OnRecvMsgZjelockctrl1(const VARIANT& varMsg)
{
	// TODO: 在此处添加消息处理程序代码
	//OutputDebugStringA(__FUNCTION__);
	char *rMsg=_com_util::ConvertBSTRToString(varMsg.bstrVal);
	string sType=zwGetJcxmlMsgType(rMsg);
	int64_t nowUs=myGetUs();
	char timeStampBuf[32];
	memset(timeStampBuf,0,32);
	sprintf(timeStampBuf,"%s%lld","TIPZW1216 ",nowUs);
	int rMsgLen=strlen(rMsg);
	assert(rMsgLen<512);
	OutputDebugStringA(rMsg);
	//MessageBoxA(NULL,rMsg,buf,MB_OK);
	
}


void CeLockGUITest20151208Dlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	Sleep(800);
	m_zjOCX.Close();
	CDialogEx::OnClose();
}


#include <cstdint>
#include "zwLibTools.h"
using std::int64_t;
int g_totalRunCount=5;
using zwLibTools2015::myGetUs;
UINT zw711SpeedTestThr1(LPVOID pParam)
{
	assert(g_totalRunCount>0);
	CZjelockctrl1 *pZjOCX=reinterpret_cast<CZjelockctrl1 *>(pParam);
	const char *msgarr[]=
	{g_msg00,g_msg02,g_msg03,g_msg04,g_msg03,g_msg04,g_msg03,g_msg04,g_msg03,g_msg04};
	int aSize=sizeof(msgarr)/sizeof(char *);	
	int nCount=0;

	while(nCount <(g_totalRunCount))
	{		

		int idxMsg=static_cast<int64_t>(myGetUs()) % aSize;
		assert(idxMsg>=0 && idxMsg <aSize);
		//myTestPush712(msgarr[idxMsg]);
		VARIANT tmpMsg;
		myStr2Bstr(msgarr[idxMsg],tmpMsg);	
		pZjOCX->Notify(tmpMsg);
		nCount++;
		Sleep(1000);
	}
	//cout<<"zw1209SpeedTestThr1 结束"<<endl;
	return 0;
}

UINT MyThreadProc( LPVOID pParam )
{
	int aa= (int)pParam;

	char buf[256];
	for (int i=0;i<20;i++)
	{
		memset(buf,0,256);
		sprintf(buf,"%s %d %d",__FUNCTION__,aa,time(NULL));
		OutputDebugStringA(buf);

		Sleep(1000);
	}
	// do something with 'pObject'

	return 0;   // thread completed successfully
}

