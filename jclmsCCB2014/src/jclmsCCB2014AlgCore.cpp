#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <time.h>
#include "jclmsCCB2014AlgCore.h"
#include "zwEcies529.h"
#include "sm3.h"
//ARM编译去掉assert，避免链接找不到符号
#ifndef _WIN32
#define assert
#endif // _WIN32

#include <string>
using std::string;


void mySM3Update(SM3 * ctx, const char *data, const int len);
void mySM3Update(SM3 * ctx, const int data);

#ifdef  __cplusplus
extern "C" {
#endif
const int ZWMEGA = 1000000;	//一百万
const int ZW_LOWEST_DATE = 1400 * ZWMEGA - 24 * 3600;	//考虑到取整运算可能使得时间值低于1400M，所以把最低点时间提前一整天该足够了
const int ZW_MAXDATA32 = 2048 * ZWMEGA - 3;	//32位有符号整数可能表示的最大时间值
extern const int ZW_ONE_DAY = 24 * 60 * 60;
int G_SM3DATA_TRACK=1;	//是否输出送到SM3算法的
//多段CRC8,第一次使用时,crc8Input参数输入必须为0
unsigned char crc8(const unsigned char crc8Input,const void *inputData, const int inputLen );
#ifdef  __cplusplus
}	//extern "C" {
#endif


const int ZW_SM3_DGST_SIZE = (256 / 8);
const int ZW_CLOSECODE_STEP = 12;	//闭锁码的计算步长时间精度
//从当前时间偏移到将来方向这么多秒，以防止生成密码的加密服务器时间比较快，结果下位机匹配
//的时候，从当前时间开始匹配，始终无法匹配到对应于“将来”某个时间点的动态码；
//这是20140821在建行广开中心发现的问题；
const int JC_DCODE_MATCH_FUTURE_SEC = 60 * 3;

//从包含二进制数据的字符串输入，获得一个8位整数的输出
unsigned int zwBinString2Int32(const char *data, const int len);
//获取闭锁码的3个可变条件的“固定值”
void myGetCloseCodeVarItem(int *mdatetime, int *mvalidity, int *mclosecode);

//////////////////////////////////////////////////////////////////////////

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

//获得规格化的时间，也就是按照某个值取整的时间
int myGetNormalTime(int gmtTime, const int TIMEMOD)
{
	int tail = gmtTime % TIMEMOD;
	return gmtTime - tail;
}

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
	//zwJcLockDumpJCINPUT(handle);
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
	//zwJcLockDumpJCINPUT(handle);
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
		jcp->SearchTimeStart = myGetNormalTime(num, jcp->SearchTimeStep);
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

//获取初始闭锁码的3个可变条件的“固定值”
void myGetInitCloseCodeVarItem(int *mdatetime, int *mvalidity, int *mclosecode)
{
	assert(NULL != mdatetime && NULL != mvalidity && NULL != mclosecode);
	if (NULL == mdatetime || NULL == mvalidity || NULL == mclosecode) {
		return;
	}
	//20141113.1748.经过前两天的讨论，锁具初始闭锁码不能因为时间变化而变化
	//所以时间值定死为1400M秒，或者其实哪个过去的方便人识别的时间点都可以；
	//这些参数后续要改为可以配置的，起码要可以通过函数调用来配置，最好能
	//使用配置文件来配置
	*mdatetime = 1400*ZWMEGA;
	*mvalidity = 1000;
	*mclosecode = 10000000;
}

//从包含二进制数据的字符串输入，获得一个8位整数的输出
unsigned int zwBinString2Int32(const char *data, const int len)
{
	//比1开头的8位数稍微大一些的质数
	const int dyLow = 10000019;
	//比9开头的8位数稍微小一些的质数
	const int dyMod = 89999969;
	const int dyMul = 257;	//随便找的一个质数作为相乘的因子

	unsigned __int64 sum = 0;
	for (int i = 0; i < len; i++) {
		unsigned char t = *(data + i);
		sum *= dyMul;
		sum += t;
	}
	//这两个数字结合使用，产生肯定是8位数的动态码
	sum %= dyMod;
	sum += dyLow;
	return sum;
}


//获取闭锁码的3个可变条件的“固定值”
void myGetCloseCodeVarItem(int *mdatetime, int *mvalidity, int *mclosecode)
{
	const int ZW_CLOSECODE_BASEINPUT = 20000000;	//计算正常的闭锁码时，m_closecode字段的固定值
	assert(NULL != mdatetime && NULL != mvalidity && NULL != mclosecode);
	if (NULL == mdatetime || NULL == mvalidity || NULL == mclosecode) {
		return;
	}
	*mdatetime = myGetNormalTime(*mdatetime, ZW_CLOSECODE_STEP);
	*mvalidity = 1440;
	*mclosecode = ZW_CLOSECODE_BASEINPUT;
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
		int ch=*(data + i);
#ifdef _DEBUG_20150309
		//我和孙玉龙，又是遇到ARM编译器优化级别0导致SM3算法结果错误的问题.20150309.1546
		//调试过程中用的代码
		if (1==G_SM3DATA_TRACK)
		{
			printf("%02X ",ch);
		}	
#endif // _DEBUG_20150309
	
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

JCERROR JCLMSCCB2014_API JcLockCheckInput(const int handle)
{
	//zwJcLockDumpJCINPUT(handle);
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
	//zwJcLockDumpJCINPUT(handle);
	if (EJC_SUSSESS != JcLockCheckInput((const int)jcp)) {
		ZWDBG_ERROR("JcLock Input Para Error!\n");
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
	ZWDBG_NOTICE("All Items = %s \n", allStr);
}

void JCLMSCCB2014_API zwJcLockDumpJCINPUT(const int handle)
{
	unsigned char crc=0;
	JCINPUT *jcp = (JCINPUT *) handle;
	assert(NULL != jcp);
	if (NULL == jcp) {
		ZWDBG_ERROR("%s input is NULL", __FUNCTION__);
		return;
	}
	static int dedupTime;
	//防止重复输出同一个数据结构
	if (dedupTime==jcp->CodeGenDateTime)
	{
		//return;
	}

	//ZWPRINTF("########JCINPUT DUMP START############\n");
	ZWDBG_INFO("[");
	ZWDBG_INFO("ATMNO:%s\t", jcp->AtmNo);
	crc=crc8(crc,(void *)&jcp->AtmNo,sizeof(jcp->AtmNo));
	ZWDBG_INFO("LOCKNO:%s\t", jcp->LockNo);
	crc=crc8(crc,(void *)&jcp->LockNo,sizeof(jcp->LockNo));
	ZWDBG_INFO("PSK:%s\n", jcp->PSK);
	crc=crc8(crc,(void *)&jcp->PSK,sizeof(jcp->PSK));
	ZWDBG_NOTICE("DATETIME:%d\t%s\t", jcp->CodeGenDateTime,
		zwTimeSecond2String(jcp->CodeGenDateTime));
	crc=crc8(crc,(void *)&jcp->CodeGenDateTime,sizeof(jcp->CodeGenDateTime));
	ZWDBG_INFO("STEP:%d\t", jcp->SearchTimeStep);
	crc=crc8(crc,(void *)&jcp->SearchTimeStep,sizeof(jcp->SearchTimeStep));
	ZWDBG_INFO("RTIME:%d\n", jcp->SearchTimeLength);
	crc=crc8(crc,(void *)&jcp->SearchTimeLength,sizeof(jcp->SearchTimeLength));
	ZWDBG_INFO("VAL:%d\tCloseCode:%d\t", jcp->Validity,
		jcp->CloseCode);
	crc=crc8(crc,(void *)&jcp->Validity,sizeof(jcp->Validity));
	crc=crc8(crc,(void *)&jcp->CloseCode,sizeof(jcp->CloseCode));
	ZWDBG_INFO("CMDTYPE:");
	crc=crc8(crc,(void *)&jcp->CmdType,sizeof(jcp->CmdType));
	ZWDBG_INFO("CRC8=%u\n",crc);
	switch (jcp->CmdType) {
	case JCI_ATMNO:
		ZWDBG_INFO("JCI_ATMNO");
		break;
	case JCI_LOCKNO:
		ZWDBG_INFO("JCI_LOCKNO");
		break;
	case JCI_PSK:
		ZWDBG_INFO("JCI_PSK");
		break;
	case JCI_DATETIME:
		ZWDBG_INFO("JCI_DATETIME");
		break;
	case JCI_VALIDITY:
		ZWDBG_INFO("JCI_VALIDITY");
		break;
	case JCI_CLOSECODE:
		ZWDBG_INFO("JCI_CLOSECODE");
		break;
	case JCI_CMDTYPE:
		ZWDBG_INFO("JCI_CMDTYPE");
		break;
	case JCI_TIMESTEP:
		ZWDBG_INFO("JCI_TIMESTEP");
		break;
	}
	//ZWPRINTF("\n");
	//ZWPRINTF("M_VALIDITY_ARRAY:\n");
	//for (int i = 0; i < NUM_VALIDITY; i++) {
	//	ZWPRINTF("%d\t", jcp->m_validity_array[i]);
	//}
	ZWDBG_INFO("]\n");
	dedupTime=jcp->CodeGenDateTime;
}

//////////////////////////////////////////////////////////////////////////
int JCLMSCCB2014_API JcLockNew(void)
{
	//myCjsonTest1();
	JCINPUT *pjc = new JCINPUT;
	assert(pjc != NULL);
	memset(pjc, 0, sizeof(JCINPUT));
	memset(pjc->AtmNo, 0, JC_ATMNO_MAXLEN + 1);
	memset(pjc->LockNo, 0, JC_LOCKNO_MAXLEN + 1);
	memset(pjc->PSK, 0, JC_PSK_LEN + 1);
	//#ifdef _DEBUG
	//	ZWPRINTF("sizeof JCINPUT=%d\n",sizeof(JCINPUT));
	//#endif // _DEBUG
	//为没有可变输入的初始闭锁码指定3个常量
	pjc->CodeGenDateTime = 1400 * 1000 * 1000;
	pjc->Validity = 5;	//用的最多的是5分钟有效期，所以直接初始化为5
	pjc->CloseCode = 0;	//防备初始闭锁码生成的时候此处未初始化
	pjc->CmdType = JCCMD_INIT_CLOSECODE;
	//pjc->dbgSearchTimeStart=time(NULL);
	pjc->SearchTimeStep = 6;
	//默认在线模式，反推时间步长60秒.
	//20140805.0903.按照昨天张靖钰的要求，暂时改为5分钟默认值
	// 20140820.2329.按照建行要求从任意时间点开始5分钟有效期的要求，
	// 步长改为6秒 以便尽量接近该要求
	//默认在线模式(由于起始值会往将来方向偏移3分钟所以是)反推6分钟，比要求的5分钟多一点，保险一点
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

//生成各种类型的动态码
int zwJcLockGetDynaCode(const int handle)
{
	//zwTrace1027 tmr(__FUNCTION__"1");
	ZWDBG_INFO("%s\n",__FUNCTION__);
	JcLockDebugPrint(handle);
	zwJcLockDumpJCINPUT(handle);
	const JCINPUT *lock = (const JCINPUT *)handle;
	SM3 sm3;
	char outHmac[ZW_SM3_DGST_SIZE];

	//规格化时间到G_TIMEMOD这么多秒
	int l_datetime = myGetNormalTime(lock->CodeGenDateTime,
		lock->SearchTimeStep);
	//60*5);        //20140804.1717.应张靖钰的测试需求，暂时改为5分钟取整
	//有效期和闭锁码需要根据不同情况分别处理
	int l_validity = lock->Validity;
	int l_closecode = lock->CloseCode;
	//计算初始闭锁码时，采用十天半月大致固定的时间，有效期，闭锁码的值
	//以便对于特定的锁具和PSK来说，初始闭锁码是一个十天半月内的恒定值
	if (JCCMD_INIT_CLOSECODE == lock->CmdType) {
		//l_datetime=myGetNormalTime(time(NULL),ZWMEGA);        //初始闭锁码采用1M秒(大约12天)的取整时间
		//l_validity=1000;      //初始有效期取一个有效范围内的规整值
		//l_closecode=1000000;  //初始闭锁码特选一个有效范围内的规整值
		myGetInitCloseCodeVarItem(&l_datetime, &l_validity,
			&l_closecode);
	}
	if (JCCMD_CCB_CLOSECODE == lock->CmdType) {	//计算真正的闭锁码，采用3个固定条件，外加特定的取整步长的时间，以及固定的有效期和“闭锁码”作为输入
		myGetCloseCodeVarItem(&l_datetime, &l_validity, &l_closecode);
	}
	JCERROR err = JcLockCheckInput((const int)lock);
	if (EJC_SUSSESS != err) {
		return err;
	}

	G_SM3DATA_TRACK=1;
	SM3_Init(&sm3);

	//限度是小于14开头的时间(1.4G秒)或者快要超出2048M秒的话就是非法了
	/////////////////////////////逐个元素进行HASH运算/////////////////////////////////////////////
	//首先处理固定字段的HASH值输入
	mySM3Update(&sm3, lock->AtmNo, sizeof(lock->AtmNo));
	mySM3Update(&sm3, lock->LockNo, sizeof(lock->LockNo));
	mySM3Update(&sm3, lock->PSK, sizeof(lock->PSK));

	//继续输入各个可变字段的HASH值
	mySM3Update(&sm3, l_datetime);
	mySM3Update(&sm3, l_validity);
	mySM3Update(&sm3, l_closecode);
	mySM3Update(&sm3, lock->CmdType);
	//////////////////////////////HASH运算结束////////////////////////////////////////////
	memset(outHmac, 0, ZWSM3_DGST_LEN);
	SM3_Final(&sm3, (char *)(outHmac));

#ifdef _DEBUG_20150309
	//我和孙玉龙，又是遇到ARM编译器优化级别0导致SM3算法结果错误的问题.20150309.1546
	printf("outHmac=\n");
	for (int i=0;i<ZWSM3_DGST_LEN;i++)
	{
		printf("%02X ",outHmac[i] & 0xFF);
	}
	printf("\n");
#endif // _DEBUG_20150309
	//把HASH结果转化为8位数字输出
	unsigned int res = zwBinString2Int32(outHmac, ZWSM3_DGST_LEN);
	ZWDBG_WARN("%s:dyCode=%d\n",__FUNCTION__,res);
	G_SM3DATA_TRACK=0;
	return res;
}

//离线模式匹配，时间点精度为取整到一个小时的零点，有效期精度为1小时起
//如果找到了，返回JCOFFLINE中是匹配的时间和有效期，否则其中的值都是0
JCMATCH JCLMSCCB2014_API JcLockReverseVerifyDynaCode(const int handle,
	const int dstCode)
{
	//zwTrace1027 tmr(__FUNCTION__"1");
	ZWDBG_WARN("%s dstCode=%d\n",__FUNCTION__,dstCode);
	JcLockDebugPrint(handle);
	zwJcLockDumpJCINPUT(handle);
	JCINPUT *jcp = (JCINPUT *) handle;
	const int MIN_OF_HOUR = 60;	//一小时的分钟数
	JCMATCH jcoff;
	//填入默认的失败返回值
	jcoff.s_datetime = 0;
	jcoff.s_validity = 0;

	//根据建行广开中心发现的问题，从“将来”几分钟的时间开始往过去方向
	//匹配，以防密码服务器和锁具之间有时间误差；
	int l_datetime = jcp->SearchTimeStart + JC_DCODE_MATCH_FUTURE_SEC;
	int l_closecode = jcp->CloseCode;
	int l_timestep = jcp->SearchTimeStep;
	if (JCCMD_CCB_CLOSECODE == jcp->CmdType) {
		int l_validity = jcp->Validity;	//此输入参数验证时无用，只是为了满足函数输入要求
		//如果是验证闭锁码，就换一套参数
		//验证闭锁码的时候，是否需要搜索更长时间呢？2014.0729.1509周伟
		myGetCloseCodeVarItem(&l_datetime, &l_validity, &l_closecode);
		l_timestep = ZW_CLOSECODE_STEP;
		assert(ZW_CLOSECODE_STEP > 0 && ZW_CLOSECODE_STEP < 60);
	}
	//搜索时间的起始点必须落在m_stepoftime的整倍数上，否则就无法匹配	
	//本来起始点在设置时已经规格化过了，但是为了防止之后又设置步长，
	//所以在此，用到的时候，再次根据步长规格化起始点时间
	l_datetime=myGetNormalTime(jcp->SearchTimeStart, jcp->SearchTimeStep);;
	l_datetime = myGetNormalTime(l_datetime, l_timestep);
	int tail = l_datetime % l_timestep;
	l_datetime -= tail;	//取整到数据结构中指定的步长

	//结束时间，往前推数据结构所指定的一段时间，几分钟到一整天不等
	int tend = l_datetime - jcp->SearchTimeLength;

	for (int tdate = l_datetime; tdate >= tend; tdate -= l_timestep) {			
		ZWDBG_INFO("%d\t",tdate);	
		for (int v = 0; v < NUM_VALIDITY; v++) {
			SM3 sm3;
			char outHmac[ZW_SM3_DGST_SIZE];

			SM3_Init(&sm3);
			/////////////////////////////逐个元素进行HASH运算/////////////////////////////////////////////
			mySM3Update(&sm3, jcp->AtmNo, sizeof(jcp->AtmNo));
			mySM3Update(&sm3, jcp->LockNo,
				sizeof(jcp->LockNo));
			mySM3Update(&sm3, jcp->PSK, sizeof(jcp->PSK));

			mySM3Update(&sm3, tdate);
			mySM3Update(&sm3, jcp->ValidityArray[v]);
			mySM3Update(&sm3, l_closecode);
			mySM3Update(&sm3, jcp->CmdType);
			//////////////////////////////HASH运算结束////////////////////////////////////////////
			memset(outHmac, 0, ZWSM3_DGST_LEN);
			SM3_Final(&sm3, (char *)(outHmac));
			unsigned int res =
				zwBinString2Int32(outHmac, ZWSM3_DGST_LEN);
			//ZWPRINTF("%d:%d\t",tdate,res);	
			//if (3==v)
			//{
			//	ZWPRINTF("\n");
			//}
			if (dstCode == res)	//发现了匹配的时间和有效期
			{
				//填写匹配的时间和有效期到结果
				ZWDBG_WARN("FOUND MATCH UTC SECONDS:%d\tMinites:%d\n", tdate,
					jcp->ValidityArray[v]);
				jcoff.s_datetime = tdate;
				jcoff.s_validity = jcp->ValidityArray[v];
				goto foundMatch;
			}			
		}		//END OF VALIDITY LOOP		
		//ZWPRINTF("\n");
	}			//END OF DATE LOOP
foundMatch:
	return jcoff;
}

int JCLMSCCB2014_API JcLockGetDynaCode(const int handle)
{
	return zwJcLockGetDynaCode(handle);
}

//生成第一，第二开锁码的共同函数，差异只在于CloseCode那个位置，在生成第一开锁码时
//填写的是前一次的闭锁码，生成验证码时填写的是第一开锁码，生成第二开锁码时填写的是验证码
//atm编号，锁编号都是不超过一定长度限度的随意的字符串，PSK是定长64字节HEX字符串相关长度限制请见头文件
//DyCodeUTCTime为指定动态码的时间UTC秒数，一般都是当前时间，但也可以为将来提前生成动态码而指定将来的时间
int embSrvGenDyCode(const JCCMD Pass,const time_t DyCodeUTCTime,const int CloseCode,
	const char *AtmNo,const char *LockNo,const char *PSK)
{
	int handle = JcLockNew();
	JcLockSetString(handle, JCI_ATMNO, AtmNo);
	JcLockSetString(handle, JCI_LOCKNO, LockNo);
	JcLockSetString(handle, JCI_PSK, PSK);
	const int JCMOD=6;
	int tail=DyCodeUTCTime % JCMOD;	//做6秒的时间规格化，使得时间协调一致
	JcLockSetInt(handle, JCI_DATETIME,static_cast < int >(DyCodeUTCTime-tail));
	JcLockSetCmdType(handle, JCI_CMDTYPE, Pass);
	JcLockSetInt(handle, JCI_CLOSECODE, CloseCode);
	int pass1DyCode = JcLockGetDynaCode(handle);	
	JcLockDelete(handle);
	return pass1DyCode;
}

//校验动态码，返回匹配的UTC时间秒数,需要的输入有：
//JCI_ATMNO,JCI_LOCKNO,JCI_PSK等3个基本条件
//以及CloseCode(此处指的是生成该动态码时填写的那个前一环节的输入条件)
//JCCMD指示校验的是哪一类的动态码 
//SearchStartTime指定搜索起始时间，一般情况下就是当前时间的UTC秒数
int embSrvReverseDyCode(const JCCMD Pass,const int dyCode, const int CloseCode,const time_t SearchStartTime,
	const char *AtmNo,const char *LockNo,const char *PSK)
{
	int handle = JcLockNew();
	JcLockSetString(handle, JCI_ATMNO, AtmNo);
	JcLockSetString(handle, JCI_LOCKNO, LockNo);
	JcLockSetString(handle, JCI_PSK, PSK);
	//生成动态码时不必设置搜索起始时间参数，反推时才需要
	//从将来3分钟开始往前搜索
	JcLockSetInt(handle,JCI_SEARCH_TIME_START,static_cast<int>(SearchStartTime+3*60));
		
	JcLockSetInt(handle, JCI_CLOSECODE, CloseCode);
	JcLockSetCmdType(handle, JCI_CMDTYPE, Pass);	
	//////////////////////////////////////////////////////////////////////////
	JCMATCH pass1Match =
		JcLockReverseVerifyDynaCode(handle, dyCode);
#ifdef WIN32	//避开ARM没有time函数的问题(板子没有RTC时钟无法提供时间)
	printf("current time=\t\t%d\n", time(NULL));
#endif // WIN32
	printf("pass1Match Time =\t%d\tValidity=%d\n",
		pass1Match.s_datetime, pass1Match.s_validity);
	JcLockDelete(handle);
	return pass1Match.s_datetime;
}

//从建行的2个输入因素生成PSK，结果是64字节HEX字符串；
const char * zwGenPSKFromCCB(const char * ccbFact1, const char * ccbFact2)
{
	char ccbIn[ZW_ECIES_HASH_LEN];
	memset(ccbIn,0,ZW_ECIES_HASH_LEN);
	strcpy(ccbIn,ccbFact1);
	strcat(ccbIn,ccbFact2);
	//从ccbInStr生成PSK
	const char *ccbPSK=zwMergePsk(ccbIn);
	return ccbPSK;
}


////////////////////////////////ECIES//////////////////////////////////////////
//从公钥，建行的2个输入因子字符串，输出激活信息字符串，输出缓冲区必须有头文件里面指定的足够大小
void zwGenActiveInfo(const char *pubkey,const char *ccbFact1,const char *ccbFact2,char *ccbActiveInfo)
{
	if (NULL==ccbFact1 ||NULL==ccbFact2 || NULL==ccbActiveInfo
		||0==strlen(ccbFact1) || 0==strlen(ccbFact2))
	{
		return;
	}
	const char * ccbPSK=zwGenPSKFromCCB(ccbFact1, ccbFact2);

#ifdef _DEBUG
	printf("ccbPSK=\t%s\n",ccbPSK);
#endif // _DEBUG
	//从PSK和公钥生成激活信息ccbActiveInfo，然后激活信息就可以通过网络传输出去了
	strcpy(ccbActiveInfo, EciesEncrypt(pubkey, ccbPSK));
}

//生成公钥私钥对,输入缓冲区必须有头文件里面宏定义值所指定的足够大小
void zwGenKeyPair(char *pubKey,char *priKey)
{
	if (NULL==pubKey || NULL==priKey)
	{
		return;
	}
	int hd=EciesGenKeyPair();
	strcpy(pubKey,EciesGetPubKey(hd));
	strcpy(priKey,EciesGetPriKey(hd));
	EciesDelete(hd);
}


//从私钥，激活信息，获取PSK，输出缓冲区必须有头文件里面指定的足够大小
void zwGetPSK(const char *priKey,const char *ccbActiveInfo,char *PSK)
{
	if (NULL==priKey || NULL==ccbActiveInfo || NULL==PSK
		||0==strlen(priKey) || 0==strlen(ccbActiveInfo))
	{
		return;
	}
	strcpy(PSK,EciesDecrypt(priKey,ccbActiveInfo));
}

////////////////////////////////3DES//////////////////////////////////////////
string zwCode8ToHex(int Code8)
{
	//8位动态码转换为字符串，然后字符串8字节转换为HEX，以便满足3DES的
	//64bit输入要求，估计这样就满足建行的要求可以被正确解密了；
	const int BUFLEN = 32;
	char buf[BUFLEN];
	memset(buf, 0, BUFLEN);
	sprintf(buf, "%08d", Code8);
	assert(strlen(buf) == 8);
	char hexbuf[BUFLEN];
	memset(hexbuf, 0, BUFLEN);
	for (int i = 0; i < 8; i++) {
		unsigned char ch = buf[i] % 256;
		sprintf(hexbuf + i * 2, "%02X", ch);
	}
	string retHexStr = hexbuf;
	return retHexStr;
}

