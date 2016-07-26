#include "stdafx.h"
#include "eLockGUITest20151208.h"
#include "eLockGUITest20151208Dlg.h"
#include <cstdint>
#include "zwLibTools.h"
using std::int64_t;
using zwLibTools2015::myGetUs;


const char *g_msg00="<?xml version='1.0' encoding='UTF-8'?><root><TransCode>0000</TransCode><TransName>CallForActInfo</TransName><TransDate>20160708</TransDate><TransTime>110900</TransTime><DevCode>515067001098</DevCode><SpareString1></SpareString1><SpareString2></SpareString2></root>";
const char *g_msg01="<?xml version='1.0' encoding='UTF-8'?><root><TransCode>0001</TransCode><TransName>SendActInfo</TransName><TransDate>20160708</TransDate><TransTime>110900</TransTime><DevCode>515067001098</DevCode><LockMan>BeiJing.JinChu</LockMan><LockId>22222222</LockId><ActInfo>BFr4af4YvXxtsGmJnDCKsZ3OhmkZimSur0itl6fwuc3fqkiK6j05arPl2on3N4rfVLQkfo9GRceMmbXDebf7rdY=.h/UOVQjtcby5I3wQyUXDdB/uDTeUq1oW.vNDgbpmArJ2CTkBSAJ0NKDeZ6vUifPLKjbLZH3eiFE+QkEBOy5+r2ZJEYEgBmjD1KGIbVfridTa3sjuqaFo0lby7YpqBXTo56v5yIzyz28k=</ActInfo><SpareString1></SpareString1><SpareString2></SpareString2></root>";
const char *g_msg02="<?xml version='1.0' encoding='UTF-8'?><root><TransCode>0002</TransCode><TransName>QueryForLockStatus</TransName><TransDate>20160708</TransDate><TransTime>110900</TransTime><DevCode>515067001098</DevCode><LockMan></LockMan><LockId></LockId><SpareString1></SpareString1><SpareString2></SpareString2></root>";
const char *g_msg03="<?xml version='1.0' encoding='UTF-8'?><root><TransCode>0003</TransCode><TransName>TimeSync</TransName><TransDate>20160708</TransDate><TransTime>110900</TransTime></root>";
const char *g_msg04="<?xml version='1.0' encoding='UTF-8'?><root><TransCode>0004</TransCode><TransName>ReadShutLockCode</TransName><TransDate>20160708</TransDate><TransTime>110900</TransTime><DevCode>515067001098</DevCode><SpareString1></SpareString1><SpareString2></SpareString2></root>"; 

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

//是否是锁具主动上送报文这样的反向循环报文
bool myIsJsonMsgFromLockFirstUp(const string &jcMsg)
{
	if(						
		jcMsg=="Lock_Open_Ident"	  ||
		jcMsg=="Lock_Close_Code_Lock" ||
		jcMsg=="Lock_Time_Sync_Lock" ||
		jcMsg== "Lock_Alarm_Info")
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool myIsXMLMsgCodeFromLockFirstUp(const string &jcMsg)
{
	if(						
		jcMsg=="1000"	  ||
		jcMsg=="1001" ||
		jcMsg=="1002" ||
		jcMsg== "1003")
	{
		return true;
	}
	else
	{
		return false;
	}
}


void myStr2Bstr(const char *strIn,VARIANT &bstrOut)
{
	_bstr_t strMessage = strIn;
	bstrOut.vt=VT_BSTR;
	bstrOut.bstrVal= strMessage;	
}


UINT CeLockGUITest20151208Dlg::zw711SpeedTestThr1(LPVOID pParam)
{
	CeLockGUITest20151208Dlg *pDlg=reinterpret_cast<CeLockGUITest20151208Dlg *>(pParam);
	assert(pDlg->m_runMsgNum>0);
	const char *msgarr[]=
	{g_msg00,g_msg03,g_msg04,g_msg03,g_msg04,g_msg03,g_msg04,g_msg03,g_msg04};
	int aSize=sizeof(msgarr)/sizeof(char *);	
	int nCount=0;

	while(nCount <(pDlg->m_runMsgNum))
	{		

		int idxMsg=static_cast<int64_t>(myGetUs()) % aSize;
		assert(idxMsg>=0 && idxMsg <aSize);
		VARIANT tmpMsg;
		myStr2Bstr(msgarr[idxMsg],tmpMsg);	
		pDlg->m_zjOCX.Notify(tmpMsg);
		pDlg->m_secDqNotify.Lock();
		pDlg->m_dqNotify.push_back(zwGetJcxmlMsgType(msgarr[idxMsg]));
		pDlg->m_curMsg++;
		pDlg->m_secDqNotify.Unlock();

		nCount++;
		
		Sleep(pDlg->m_msgInvMs);
	}
	return 0;
}


UINT CeLockGUITest20151208Dlg::zw711SpeedTestThr2(LPVOID pParam)
{
	CeLockGUITest20151208Dlg *pDlg=reinterpret_cast<CeLockGUITest20151208Dlg *>(pParam);
	VARIANT tmpMsg;
	myStr2Bstr(g_msg02,tmpMsg);	

	while(1)
	{		
		Sleep(6000);
		pDlg->m_zjOCX.Notify(tmpMsg);
		pDlg->m_secDqNotify.Lock();
		pDlg->m_dqNotify.push_back(zwGetJcxmlMsgType(g_msg02));
		pDlg->m_curMsg++;
		pDlg->m_secDqNotify.Unlock();
		if (pDlg->m_curMsg>pDlg->m_runMsgNum)
		{
			break;
		}
	}
	return 0;
}