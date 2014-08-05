#include "stdafx.h"
#include "zwCcbElockHdr.h"

namespace jcAtmcConvertDLL{
	//JSON�����ַ���
	const char *JCSTR_CMDTITLE="Command";
	const char *JCSTR_LOCK_ACTIVE_REQUEST="Lock_Secret_Key";
	const char *JCSTR_LOCK_INIT="Lock_System_Init";
	const char *JCSTR_READ_CLOSECODE="Lock_Close_Code_ATM";
	const char *JCSTR_SEND_INITCLOSECODE="Lock_Close_Code_Lock";
	const char *JCMSG_SEND_UNLOCK_CERTCODE="Lock_Open_Ident";
}

//�Ӹ�����ʱ����������ȡ����(YYYYMMDD)��ʱ��(HHMMSS)�ַ���
void zwGetDateTimeString(time_t inTime,string &outDate,string &outTime)
{
	char zwDate[16];	//YYYYMMDD+NULL
	char zwTime[16];	//HHMMSS+NULL
	memset(zwDate,0,16);
	memset(zwTime,0,16);
	time_t tmpTime=inTime;
	assert(tmpTime>1400*1000*1000);
	struct tm *p;
	p = localtime(&tmpTime); //ȡ�õ���ʱ��
	sprintf (zwDate,	"%04d%02d%02d", (1900+p->tm_year), (p->tm_mon+1), p->tm_mday);
	sprintf(zwTime,		"%02d%02d%02d",  p->tm_hour, p->tm_min, p->tm_sec);
	outDate=zwDate;
	outTime=zwTime;
}
