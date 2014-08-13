#include "stdafx.h"
#include "zwCcbElockHdr.h"

namespace jcAtmcConvertDLL{
	//JSON命令字符串
	const char *JCSTR_CMDTITLE="Command";
	const char *JCSTR_LOCK_ACTIVE_REQUEST="Lock_Secret_Key";
	const char *JCSTR_LOCK_INIT="Lock_System_Init";
	const char *JCSTR_READ_CLOSECODE="Lock_Close_Code_ATM";
	const char *JCSTR_SEND_INITCLOSECODE="Lock_Close_Code_Lock";
	const char *JCSTR_SEND_UNLOCK_CERTCODE="Lock_Open_Ident";
	
	const char *CCBSTR_CODE="root.TransCode";
	const char *CCBSTR_NAME="root.TransName";
	const char *CCBSTR_DATE="root.TransDate";
	const char *CCBSTR_TIME="root.TransTime";
	const char *CCBSTR_DEVCODE="root.DevCode";
}

void ZWTRACE(const char *x)
{
	OutputDebugStringA(x);
	//cout<<x<<endl;	
	g_log->trace(x);
}

zw_trace::zw_trace(const char *funcName)
{
	m_str=funcName;
	m_start=m_str+"\tSTART";
	m_end=m_str+"\tEND";
	OutputDebugStringA(m_start.c_str());
	//cout<<m_start<<endl;
	g_log->trace(m_start);
}

zw_trace::~zw_trace()
{
	
	OutputDebugStringA(m_end.c_str());
	//cout<<m_end<<endl;
	g_log->trace(m_end);
}


//从给定的时间秒数，获取日期(YYYYMMDD)和时间(HHMMSS)字符串
void zwGetDateTimeString(time_t inTime,string &outDate,string &outTime)
{
	ZWFUNCTRACE
	char zwDate[16];	//YYYYMMDD+NULL
	char zwTime[16];	//HHMMSS+NULL
	memset(zwDate,0,16);
	memset(zwTime,0,16);
	time_t tmpTime=inTime;
	assert(tmpTime>1400*1000*1000);
	struct tm *p;
	p = localtime(&tmpTime); //取得当地时间
	sprintf (zwDate,	"%04d%02d%02d", (1900+p->tm_year), (p->tm_mon+1), p->tm_mday);
	sprintf(zwTime,		"%02d%02d%02d",  p->tm_hour, p->tm_min, p->tm_sec);
	outDate=zwDate;
	outTime=zwTime;
}
