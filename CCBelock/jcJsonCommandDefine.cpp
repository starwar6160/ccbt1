#include "stdafx.h"
#include "zwCcbElockHdr.h"

namespace jcAtmcConvertDLL{
	//JSON命令字符串
	const char *JCSTR_CMDTITLE="Command";
	const char *JCSTR_LOCK_ACTIVE_REQUEST="Lock_Secret_Key";
	const char *JCSTR_LOCK_INIT="Lock_System_Init";
	const char *JCSTR_READ_CLOSECODE="Lock_Close_Code_ATM";
}

//从给定的时间秒数，获取日期(YYYYMMDD)和时间(HHMMSS)字符串
void zwGetDateTimeString(time_t inTime,string &outDate,string &outTime)
{
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
