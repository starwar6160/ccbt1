#include "stdafx.h"
#include <time.h>
#include <memory.h>
#include "jclmsCCB2014.h"
#include "dCodeHdr.h"

//////////////////////////////////////////////////////////////////////////
int JCLMSCCB2014_API JcLockNew(void)
{
	JCINPUT *pjc = new JCINPUT;
	assert(pjc != NULL);
	memset(pjc, 0, sizeof(JCINPUT));
	memset(pjc->AtmNo, 0, JC_ATMNO_MAXLEN + 1);
	memset(pjc->LockNo, 0, JC_LOCKNO_MAXLEN + 1);
	memset(pjc->PSK, 0, JC_PSK_LEN + 1);
	//为没有可变输入的初始闭锁码指定3个常量
	pjc->CodeGenDateTime = 1400 * 1000 * 1000;
	pjc->Validity = 5;	//用的最多的是5分钟有效期，所以直接初始化为
	pjc->CloseCode = 0;	//防备初始闭锁码生成的时候此处未初始化
	pjc->CmdType = JCCMD_INIT_CLOSECODE;
	pjc->SearchTimeStep = 6;
	//默认在线模式，反推时间步长60秒.
	//20140805.0903.按照昨天张靖钰的要求，暂时改为5分钟默认值
	// 20140820.2329.按照建行要求从任意时间点开始5分钟有效期的要求，
	// 步长改为6秒 以便尽量接近该要求
	//默认在线模式，反推6分钟，比要求的5分钟多一点，保险一点
	pjc->SearchTimeLength = 9 * 60;
	////将5分钟，4小时这样最常用到的有效期排列在前面，提高效率
	//int valarr[]={5,MIN_OF_HOUR*4,MIN_OF_HOUR*8,MIN_OF_HOUR*12,15,30,60,MIN_OF_HOUR*24};
	pjc->ValidityArray[0] = 5;
	pjc->ValidityArray[1] = 60 * 4;
	pjc->ValidityArray[2] = 60 * 8;
	pjc->ValidityArray[3] = 60 * 12;
	pjc->ValidityArray[4] = 15;
	pjc->ValidityArray[5] = 30;
	pjc->ValidityArray[6] = 60;
	pjc->ValidityArray[7] = 60 * 24;
	return (int)pjc;
}

int JCLMSCCB2014_API JcLockDelete(const int handle)
{
	JCINPUT *jcp = (JCINPUT *) handle;
	assert(NULL != jcp);
	if (NULL == jcp) {
		return EJC_INPUT_NULL;
	}
	memset(jcp, 0xCC, sizeof(JCINPUT));
	delete jcp;
	return EJC_SUSSESS;
}

//时间GMT秒数转为字符串
static string zwTimeSecond2String(const time_t sec)
{
	char strTime[32];
	memset(strTime, 0, 32);
	struct tm *p;
	time_t tsec = sec;
	p = localtime(&tsec);
	sprintf(strTime, "%04d.%02d%02d:%02d:%02d:%02d",
		(1900 + p->tm_year), (1 + p->tm_mon), p->tm_mday,
		p->tm_hour, p->tm_min, p->tm_sec);
	string rStr = strTime;
	return rStr;
}

void JCLMSCCB2014_API zwJcLockDumpJCINPUT(const int handle)
{
	JCINPUT *jcp = (JCINPUT *) handle;
	assert(NULL != jcp);
	if (NULL == jcp) {
		printf("%s input is NULL", __FUNCTION__);
		return;
	}
	//printf("########JCINPUT DUMP START############\n");
	printf("\n[");
	printf("ATMNO:%s\t", jcp->AtmNo);
	printf("LOCKNO:%s\t", jcp->LockNo);
	printf("PSK:%s\n", jcp->PSK);
	printf("DATETIME:%d\t%s\t", jcp->CodeGenDateTime,
	       zwTimeSecond2String(jcp->CodeGenDateTime).c_str());
	printf("STEP:%d\t", jcp->SearchTimeStep);
	printf("RTIME:%d\n", jcp->SearchTimeLength);
	printf("VAL:%d\tCloseCode:%d\t", jcp->Validity,
	       jcp->CloseCode);
	printf("CMDTYPE:");
	switch (jcp->CmdType) {
	case JCI_ATMNO:
		printf("JCI_ATMNO");
		break;
	case JCI_LOCKNO:
		printf("JCI_LOCKNO");
		break;
	case JCI_PSK:
		printf("JCI_PSK");
		break;
	case JCI_DATETIME:
		printf("JCI_DATETIME");
		break;
	case JCI_VALIDITY:
		printf("JCI_VALIDITY");
		break;
	case JCI_CLOSECODE:
		printf("JCI_CLOSECODE");
		break;
	case JCI_CMDTYPE:
		printf("JCI_CMDTYPE");
		break;
	case JCI_TIMESTEP:
		printf("JCI_TIMESTEP");
		break;
	}
	//printf("\n");
	//printf("M_VALIDITY_ARRAY:\n");
	//for (int i = 0; i < NUM_VALIDITY; i++) {
	//	printf("%d\t", jcp->m_validity_array[i]);
	//}
	printf("]\n");
}

void JCLMSCCB2014_API JcLockDebugPrint(const int handle)
{
	JCINPUT *jcp = (JCINPUT *) handle;
	zwJcLockDumpJCINPUT(handle);
	if (EJC_SUSSESS != JcLockCheckInput((const int)jcp)) {
		printf("JcLock Input Para Error!\n");
	}
	//三个固定条件组合在一起,还要为NULL，连接符等留出余量
	char mainstr[JC_ATMNO_MAXLEN + JC_LOCKNO_MAXLEN + JC_PSK_LEN + 5];
	memset(mainstr, 0, sizeof(mainstr));
	sprintf(mainstr, "%s.%s.%s.", jcp->AtmNo, jcp->LockNo, jcp->PSK);
	//可变条件逐个化为字符串，组合到一起
	char vstr[40];		//大致把各个可变字段的位数估计一下
	int mdatetime = jcp->CodeGenDateTime;
	int mvalidity = jcp->Validity;
	int mclosecode = jcp->CloseCode;
	if (JCCMD_INIT_CLOSECODE == jcp->CmdType) {	//如果是生成初始闭锁码，就用临时计算的值替代之
		myGetInitCloseCodeVarItem(&mdatetime, &mvalidity, &mclosecode);
	}
	sprintf(vstr, "%d.%d.%d.%d", mdatetime, mvalidity,
		mclosecode, jcp->CmdType
		//,jcp->m_stepoftime,jcp->m_reverse_time_length
	    );
	//allItems=allItems+buf;
	char allStr[128];
	memset(allStr, 0, 128);
	strncpy(allStr, mainstr, 128);
	strcat(allStr, vstr);
	printf("All Items = %s \n", allStr);
}
