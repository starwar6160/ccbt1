
// eLockGUITest20151208Dlg.cpp : 实现文件
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
	, m_runMsgNum(0)
	, m_curMsg1(0)
	, m_curMsg2(0)
	, m_failCount1(0)
	, m_failCount2(0)
	, m_failRate1(0)
	, m_failRate2(0)
	,m_msgInvMs(0)
	, m_thr2InvMs(0)
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
	DDX_Text(pDX, IDC_EDT_INVT2MS, m_thr2InvMs);
	

	DDX_Control(pDX, IDC_BTNRUN, m_btnRun);
	DDX_Control(pDX, IDC_LBLCURITEM, m_lblAccMsgNum);
	DDX_Control(pDX, IDC_EDIT_STS, m_MsgSts);
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

	m_runMsgNum=500;
	m_curMsg1=0;
	m_failCount1=0;
	m_failRate1=0.0f;
	m_msgInvMs=500;
	m_thr2InvMs=5000;
	UpdateData(FALSE);
	m_btnRun.SetFocus();
	// TODO: 在此添加额外的初始化代码
	DWORD iOpen=m_zjOCX.Open(22);
	if (0!=iOpen)
	{
		MessageBoxA(NULL,"金储电子密码锁打开失败","失败",MB_OK);
	}
	m_lblAccMsgNum.SetWindowText(TEXT("累计条数"));
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

void CeLockGUITest20151208Dlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码	
	// 
	// 
	// std::string to variant.
	// 

	VARIANT tmpMsg;
	myStr2Bstr(g_msg04,tmpMsg);	
	m_zjOCX.Notify(tmpMsg);
	UpdateData(TRUE);
	if (m_runMsgNum<=0)
	{
		m_runMsgNum=1;
	}
	m_runStartSecond=time(NULL);
	AfxBeginThread(zw711SpeedTestThr1, this);
	AfxBeginThread(zw711SpeedTestThr2, this);	
	m_btnRun.EnableWindow(FALSE);
}
BEGIN_EVENTSINK_MAP(CeLockGUITest20151208Dlg, CDialogEx)
	ON_EVENT(CeLockGUITest20151208Dlg, IDC_ZJELOCKCTRL1, 1, CeLockGUITest20151208Dlg::OnRecvMsgZjelockctrl1, VTS_VARIANT)
END_EVENTSINK_MAP()


void CeLockGUITest20151208Dlg::OnRecvMsgZjelockctrl1(const VARIANT& varMsg)
{
	// TODO: 在此处添加消息处理程序代码
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
			m_dqNotifyT2.front()->msgDiffMs=(myGetUs()-m_dqNotifyT2.front()->msgStartUs)/1000.0f;
			m_vecTimeStatus.push_back(m_dqNotifyT2.front());
			upMatched=true;			
		}	
		if (downMsgType!=upMsgType
			&& !myIsXMLMsgCodeFromLockFirstUp(upMsgType))
		{
			m_failCount2++;					
			//干扰线程下发的报文也要参与时间统计
			m_dqNotifyT2.front()->msgDiffMs=(myGetUs()-m_dqNotifyT2.front()->msgStartUs)/1000.0f;
			m_vecTimeStatus.push_back(m_dqNotifyT2.front());
		}
		m_dqNotifyT2.pop_front();
		UpdateData(FALSE);
	}
	if (m_dqNotifyT1.size()>0 && false==upMatched)
	{
		string downMsgType;
		downMsgType=m_dqNotifyT1.front()->msgType;		
		if (downMsgType==upMsgType
			&& !myIsXMLMsgCodeFromLockFirstUp(upMsgType))
		{
			m_dqNotifyT1.front()->msgDiffMs=(myGetUs()-m_dqNotifyT1.front()->msgStartUs)/1000.0f;
			m_vecTimeStatus.push_back(m_dqNotifyT1.front());
			upMatched=true;			
		}		
		if (downMsgType!=upMsgType
			&& !myIsXMLMsgCodeFromLockFirstUp(upMsgType))
		{
			m_failCount1++;					
			string myErrMsg="CCBMFC错误722.downMsgType="+downMsgType+"upMsg="+rMsg;
			OutputDebugStringA(myErrMsg.c_str());			 
		}
		m_dqNotifyT1.pop_front();
		UpdateData(FALSE);
	}	

	UpdateData(TRUE);
	bool static bStsRuned=false;
	bStsRuned = myMsgTimeSts(bStsRuned);
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
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	Sleep(800);
	m_zjOCX.Close();
	CDialogEx::OnClose();
}

void myGetTimeStr1607(time_t inTime,string &outTimeStr)
{
	time_t rawtime=inTime;
	struct tm* timeinfo;
	char myTimeStr[80];
	memset(myTimeStr,0,80);
	timeinfo=localtime(&rawtime);
	strftime(myTimeStr,80,"%Y/%m/%d %H:%M:%S\n",timeinfo);
	outTimeStr=myTimeStr;
}



bool CeLockGUITest20151208Dlg::myMsgTimeSts( bool &bStsRuned )
{
	if (m_curMsg1>=m_runMsgNum && bStsRuned==false)
	{
		m_btnRun.EnableWindow(TRUE);
		bStsRuned=true;
		for (auto iter=m_vecTimeStatus.begin();iter!=m_vecTimeStatus.end();iter++)
		{
			string msgType=(*iter)->msgType;
			int tjCount=m_mapSts.count(msgType);
			if (tjCount==0)
			{
				//新生成一个统计用的元素
				myTestSts1607_t *tSts=new myTestSts1607_t;
				tSts->msgCount=1;
				tSts->prMaxMs=(*iter)->msgDiffMs;
				tSts->prMinMs=(*iter)->msgDiffMs;
				tSts->prTotalMs=(*iter)->msgDiffMs;
				m_mapSts[msgType]=tSts;
			}
			if (tjCount>0)
			{	myTestMsg1607_t *sItem=(*iter);
			myTestSts1607_t *tItem=m_mapSts[msgType];
			if (sItem->msgDiffMs>tItem->prMaxMs)
			{
				//找找看有没有更大的值
				tItem->prMaxMs=sItem->msgDiffMs;
			}
			if (sItem->msgDiffMs<tItem->prMinMs)
			{
				//找找看有没有更小的值
				tItem->prMinMs=sItem->msgDiffMs;
			}
			tItem->msgCount++;
			tItem->prTotalMs=tItem->prTotalMs+sItem->msgDiffMs;
			}
		}//for (auto iter=m_vecTimeStatus.begin()
		string myMsgSts;
		for (auto iter=m_mapSts.begin();iter!=m_mapSts.end();++iter)
		{
			char msgBuf[256];
			memset(msgBuf,0,256);
			myTestSts1607_t *tItem=iter->second;
			sprintf(msgBuf,"报文%s统计处理时间平均%.0f毫秒,最小%.0f毫秒,最大%.0f毫秒",
				iter->first.c_str()	,tItem->prTotalMs/tItem->msgCount,tItem->prMinMs,tItem->prMaxMs);
			if (myMsgSts.length()==0)
			{
				myMsgSts=msgBuf;			
			}
			else
			{
				myMsgSts=myMsgSts+"\r\n"+msgBuf;			
			}
		}

		string strStartTime;
		string strEndTime;
		myGetTimeStr1607(m_runStartSecond,strStartTime);
		myGetTimeStr1607(time(NULL),strEndTime);

		time_t runEndTime=time(NULL);
		time_t runLength=runEndTime-m_runStartSecond;

		int t1Succ=m_curMsg1-m_failCount1;
		int t2Succ=m_curMsg2-m_failCount2;
		float t1SuccRate=100.0f*t1Succ/m_curMsg1;
		float t1FailRate=100.0f*m_failCount1/(m_curMsg1+0.001);
		float t2SuccRate=100.0f*t2Succ/m_curMsg2;
		float t2FailRate=100.0f*m_failCount2/(m_curMsg2+0.001);

		char nbBuf[512];
		memset(nbBuf,0,512);
		sprintf(nbBuf, "正常报文%d条,成功%d条,成功率%.2f%%,失败%d条,失败率%.2f%%;\r\n"
			"干扰报文%d条,成功%d条,成功率%.2f%%,失败%d条,失败率%.2f%%;\r\n"
			"开始时间:%s,结束时间:%s,运行历时%.1f分,主线程间隔%.1f秒,副线程间隔%.1f秒",
			m_curMsg1,t1Succ,t1SuccRate, m_failCount1,t1FailRate,
			m_curMsg2,t2Succ,t2SuccRate, m_failCount2,t2FailRate,
			strStartTime.c_str(),strEndTime.c_str(),runLength/60.0f,
			m_msgInvMs/1000.0f,m_thr2InvMs/1000.0f);

		myMsgSts=myMsgSts+"\r\n"+nbBuf;
		//MessageBoxA(NULL,myMsgSts.c_str(),"报文统计1607",MB_OK);
		_bstr_t tjMsg=myMsgSts.c_str();
		m_MsgSts.SetWindowText(tjMsg);

	}
	return bStsRuned;
}


