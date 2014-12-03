#include "stdafx.h"
#include "jclmsCCB2014.h"
#include "dCodeHdr.h"


//只是作为一个基本的块规整大小单位方便处理
const int ZW_SYNCALG_BLOCK_SIZE = (128 / 8);
const int ZW_MAXDATA32 = 2048 * ZWMEGA - 3;	//32位有符号整数可能表示的最大时间值
const int ZW_LOWEST_DATE = 1400 * ZWMEGA - 24 * 3600;	//考虑到取整运算可能使得时间值低于1400M，所以把最低点时间提前一整天该足够了
const int ZW_ONE_DAY = 24 * 60 * 60;

//时间GMT秒数转为字符串
char * zwTimeSecond2String(const time_t sec)
{
	static char strTime[32];
	memset(strTime, 0, 32);
	struct tm *p;
	time_t tsec = sec;
	p = localtime(&tsec);
	sprintf(strTime, "%04d.%02d%02d:%02d:%02d:%02d",
		(1900 + p->tm_year), (1 + p->tm_mon), p->tm_mday,
		p->tm_hour, p->tm_min, p->tm_sec);
	return strTime;
}


//设置命令类型(第一开锁码，初始闭锁码等等)
JCERROR JCLMSCCB2014_API JcLockSetCmdType(const int handle, const JCITYPE mtype,
					  const JCCMD cmd)
{
	assert(handle > 0);
	assert(mtype > JCI_START && mtype < JCI_END);
	assert(cmd > JCCMD_START && cmd < JCCMD_END);
	if (handle <= 0 || mtype <= JCI_START || mtype >= JCI_END
	    || cmd <= JCCMD_START || cmd >= JCCMD_END) {
		return EJC_INPUT_NULL;
	}
	JCINPUT *jcp = (JCINPUT *) handle;

	jcp->CmdType = cmd;

	return EJC_SUSSESS;
}

//}     //end of namespace jclms

//设置字符串类型的值
JCERROR JCLMSCCB2014_API JcLockSetString(const int handle, const JCITYPE mtype,
					 const char *str)
{
	assert(handle > 0);
	assert(mtype > JCI_START && mtype < JCI_END);
	assert(str != NULL && strlen(str) > 0);
	if (handle <= 0 || mtype <= JCI_START || mtype >= JCI_END
	    || str == NULL || strlen(str) == 0) {
		return EJC_INPUT_NULL;
	}
	JCINPUT *jcp = (JCINPUT *) handle;
	zwJcLockDumpJCINPUT(handle);
	switch (mtype) {
	case JCI_ATMNO:
		strncpy(jcp->AtmNo, str, sizeof(jcp->AtmNo));
		break;
	case JCI_LOCKNO:
		strncpy(jcp->LockNo, str, sizeof(jcp->LockNo));
		break;
	case JCI_PSK:
		strncpy(jcp->PSK, str, sizeof(jcp->PSK));
		break;
	}
	return EJC_SUSSESS;

}

//设置整数类型的值
JCERROR JCLMSCCB2014_API JcLockSetInt(const int handle, const JCITYPE mtype,
				      int num)
{
	assert(handle > 0);
	assert(mtype > JCI_START && mtype < JCI_END);
	assert(num > JC_INVALID_VALUE);
	if (handle <= 0 || mtype <= JCI_START || mtype >= JCI_END
	    || num <= JC_INVALID_VALUE) {
		return EJC_INPUT_NULL;
	}
	JCINPUT *jcp = (JCINPUT *) handle;
	assert(jcp->SearchTimeStep >= 6 && jcp->SearchTimeStep <= ZW_ONE_DAY);
	zwJcLockDumpJCINPUT(handle);
	switch (mtype) {
	case JCI_DATETIME:
		//时间必须经过规格化
		if (num < (1400 * 1000 * 1000)) {
			return EJC_DATETIME_INVALID;
		}
		jcp->CodeGenDateTime = myGetNormalTime(num, jcp->SearchTimeStep);
		//jcp->dbgSearchTimeStart=jcp->CodeGenDateTime;	//20141128.从外部传入时间,不再依赖time函数
		break;
	case JCI_VALIDITY:
		assert(num > 0 && num <= 1440 * 7);
		if (num <= 0 || num > (1440 * 7)) {
			return EJC_VALIDRANGE_INVALID;
		}
		jcp->Validity = num;
		break;
	case JCI_CLOSECODE:
		//assert(num>=10000000 && num<=99999999);
		//if (num<10000000 || num>99999999)
		//{
		//      return EJC_CLOSECODE_INVALID;
		//}
		jcp->CloseCode = num;
		break;
	case JCI_TIMESTEP:	//反推时间步长
		assert(num >= 3 && num <= 3600);
		if (num < 0 || num > 3600) {
			return EJC_CMDTYPE_TIMESTEP_INVALID;
		}
		jcp->SearchTimeStep = num;
		break;
	case JCI_SEARCH_TIME_START:	//反推时间起始值
		//时间必须经过规格化
		if (num < (1400 * 1000 * 1000)) {
			return EJC_DATETIME_INVALID;
		}
		jcp->dbgSearchTimeStart = myGetNormalTime(num, jcp->SearchTimeStep);
		break;
	case JCI_SEARCH_TIME_LENGTH:	//反推时间长度
		if (num<=0 || num >(25*3600))	//一般而言反推最多不超过一天
		{
			return EJC_CMDTYPE_TIMELEN_INVALID;
		}
		jcp->SearchTimeLength=num;
	}
	return EJC_SUSSESS;
}

JCERROR JCLMSCCB2014_API JcLockCheckInput(const int handle)
{
	zwJcLockDumpJCINPUT(handle);
	const int ZW_DIGI8_LOW = 10 * ZWMEGA;
	const int ZW_DIGI8_HIGH = 100 * ZWMEGA;
	JCINPUT *jcp = (JCINPUT *) handle;
	//假定这些数字字段在二进制层面都是等同于int的长度的，以便通过一个统一的函数进行HASH运算
	assert(sizeof(jcp->CodeGenDateTime) == sizeof(int));
	assert(sizeof(jcp->Validity) == sizeof(int));
	assert(sizeof(jcp->CloseCode) == sizeof(int));
#ifdef _WIN32
	//似乎ARM上枚举大小和整型大小不一样，所以只在PC端检查这一点。20141203.1108
	//但是由于我最后计算HASH时，是使用该字段的数字值作为整数传入的，所以这个差异
	//并没有产生实质性影响；
	assert(sizeof(jcp->CmdType) == sizeof(int));
#endif // _WIN32

	assert(jcp->CodeGenDateTime >= (ZW_LOWEST_DATE)
	       && jcp->CodeGenDateTime < ZW_MAXDATA32);
	assert(jcp->CmdType > JCCMD_START && jcp->CmdType < JCCMD_END);
	if (JCCMD_INIT_CLOSECODE != jcp->CmdType && JCCMD_CCB_CLOSECODE != jcp->CmdType) {	//生成初始闭锁码,以及真正闭锁码时，不检查有效期和闭锁码的值
		assert(jcp->Validity >= 0 && jcp->Validity <= (24 * 60));
		//10,000,000 8位数，也就是10-100M之间
		assert(jcp->CloseCode >= ZW_DIGI8_LOW
		       && jcp->CloseCode <= ZW_DIGI8_HIGH);
	}

	//限度是小于14开头的时间(1.4G秒)或者快要超出ZW_MAXDATA32秒的话就是非法了
	if (jcp->CodeGenDateTime < (ZW_LOWEST_DATE) || jcp->CodeGenDateTime > ZW_MAXDATA32) {	//日期时间秒数在2014年的某个1.4G秒之前的日子，或者超过2038年(32位有符号整数最大值)则无效
		return EJC_DATETIME_INVALID;
	}
	if (JCCMD_INIT_CLOSECODE != jcp->CmdType && JCCMD_CCB_CLOSECODE != jcp->CmdType) {	//生成初始闭锁码,以及真正闭锁码时，不检查有效期和闭锁码的值
		if (jcp->Validity < 0 || jcp->Validity > (24 * 60)) {	//有效期分钟数为负数或者大于一整天则无效
			return EJC_VALIDRANGE_INVALID;
		}
		if (jcp->CloseCode < ZW_DIGI8_LOW || jcp->CloseCode > ZW_DIGI8_HIGH) {	//闭锁码小于8位或者大于8位则无效
			return EJC_CLOSECODE_INVALID;
		}
	}			//if (JCCMD_INIT_CLOSECODE!=jcp->m_cmdtype)
	if (jcp->SearchTimeStep <= 0 || jcp->SearchTimeStep >= ZW_ONE_DAY) {	//搜索步长为负数或者大于一整天则无效
		return EJC_CMDTYPE_TIMESTEP_INVALID;
	}
	if (jcp->SearchTimeLength <= 0 || jcp->SearchTimeLength >= (365 * ZW_ONE_DAY)) {	//往前搜索时间为负数或者大于一整年则无效
		return EJC_CMDTYPE_TIMELEN_INVALID;
	}

	if (jcp->CmdType <= JCCMD_START || jcp->CmdType >= JCCMD_END) {
		return EJC_CMDTYPE_INVALID;
	}
	return EJC_SUSSESS;
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

void JCLMSCCB2014_API zwJcLockDumpJCINPUT(const int handle)
{
	JCINPUT *jcp = (JCINPUT *) handle;
	assert(NULL != jcp);
	if (NULL == jcp) {
		printf("%s input is NULL", __FUNCTION__);
		return;
	}
	static int dedupTime;
	//防止重复输出同一个数据结构
	if (dedupTime==jcp->CodeGenDateTime)
	{
		return;
	}

	//printf("########JCINPUT DUMP START############\n");
	printf("\n[");
	printf("ATMNO:%s\t", jcp->AtmNo);
	printf("LOCKNO:%s\t", jcp->LockNo);
	printf("PSK:%s\n", jcp->PSK);
	printf("DATETIME:%d\t%s\t", jcp->CodeGenDateTime,
		zwTimeSecond2String(jcp->CodeGenDateTime));
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
	dedupTime=jcp->CodeGenDateTime;
}

int JcLockGetVersion(void)
{
	//含义是是日期
	return 20140901;
}

//获得规格化的时间，也就是按照某个值取整的时间
int myGetNormalTime(int gmtTime, const int TIMEMOD)
{
	int tail = gmtTime % TIMEMOD;
	return gmtTime - tail;
}

void mySM3Update(SM3 * ctx, const char *data, const int len)
{
	assert(ctx != NULL);
	assert(data != NULL);
	if (NULL==ctx || NULL==data)
	{
		return;
	}
	assert(ctx->length > 0);
	assert(len > 0);
	for (int i = 0; i < len; i++) {
		SM3_Update(ctx, *(data + i));
	}
}

void mySM3Update(SM3 * ctx, const int data)
{
	assert(ctx != NULL);
	if (NULL==ctx)
	{
		return;
	}
	assert(ctx->length > 0);
	assert(data >= 0);	//几个整数参数，都是0或者正整数
	int td = data;
	for (int i = 0; i < sizeof(data); i++) {
		unsigned char t = td & 0xff;
		SM3_Update(ctx, t);
		td = td >> 8;
	}
	assert(td == 0);
}

