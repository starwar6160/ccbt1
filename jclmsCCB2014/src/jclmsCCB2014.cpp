// jclmsCCB2014.cpp : 定义 DLL 应用程序的导出函数。
//
#include "stdafx.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "jclmsCCB2014.h"
#include "sm3.h"
#include "dCodeHdr.h"

const int ZW_SM3_DGST_SIZE = (256 / 8);
const int ZW_CLOSECODE_STEP = 12;	//闭锁码的计算步长时间精度
//从当前时间偏移到将来方向这么多秒，以防止生成密码的加密服务器时间比较快，结果下位机匹配
//的时候，从当前时间开始匹配，始终无法匹配到对应于“将来”某个时间点的动态码；
//这是20140821在建行广开中心发现的问题；
const int JC_DCODE_MATCH_FUTURE_SEC = 60 * 3;

void mySM3Update(SM3 * ctx, const char *data, const int len);
void mySM3Update(SM3 * ctx, const int data);
//从包含二进制数据的字符串输入，获得一个8位整数的输出
unsigned int zwBinString2Int32(const char *data, const int len);
//获取闭锁码的3个可变条件的“固定值”
void myGetCloseCodeVarItem(int *mdatetime, int *mvalidity, int *mclosecode);
//生成各种类型的动态码
int zwJcLockGetDynaCode(const int handle);



int JCLMSCCB2014_API JcLockGetDynaCode(const int handle)
{
	return zwJcLockGetDynaCode(handle);
}

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
	pjc->Validity = 5;	//用的最多的是5分钟有效期，所以直接初始化为5
	pjc->CloseCode = 0;	//防备初始闭锁码生成的时候此处未初始化
	pjc->CmdType = JCCMD_INIT_CLOSECODE;
	pjc->dbgSearchTimeStart=time(NULL);
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
	//把HASH结果转化为8位数字输出
	unsigned int res = zwBinString2Int32(outHmac, ZWSM3_DGST_LEN);
	return res;
}

	//离线模式匹配，时间点精度为取整到一个小时的零点，有效期精度为1小时起
	//如果找到了，返回JCOFFLINE中是匹配的时间和有效期，否则其中的值都是0
JCMATCH JCLMSCCB2014_API JcLockReverseVerifyDynaCode(const int handle,
						     const int dstCode)
{
	zwJcLockDumpJCINPUT(handle);
	JCINPUT *jcp = (JCINPUT *) handle;
	const int MIN_OF_HOUR = 60;	//一小时的分钟数
	JCMATCH jcoff;
	//填入默认的失败返回值
	jcoff.s_datetime = 0;
	jcoff.s_validity = 0;

	//根据建行广开中心发现的问题，从“将来”几分钟的时间开始往过去方向
	//匹配，以防密码服务器和锁具之间有时间误差；
	int l_datetime = time(NULL) + JC_DCODE_MATCH_FUTURE_SEC;
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
	l_datetime=myGetNormalTime(jcp->dbgSearchTimeStart, jcp->SearchTimeStep);;
	l_datetime = myGetNormalTime(l_datetime, l_timestep);
	int tail = l_datetime % l_timestep;
	l_datetime -= tail;	//取整到数据结构中指定的步长
	
	//结束时间，往前推数据结构所指定的一段时间，几分钟到一整天不等
	int tend = l_datetime - jcp->SearchTimeLength;

	for (int tdate = l_datetime; tdate >= tend; tdate -= l_timestep) {
		//printf("%d\t",tdate);
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
			if (dstCode == res)	//发现了匹配的时间和有效期
			{
				//填写匹配的时间和有效期到结果
				printf("FOUND MATCH UTC SECONDS:%d\tMinites:%d\n", tdate,
				       jcp->ValidityArray[v]);
				jcoff.s_datetime = tdate;
				jcoff.s_validity = jcp->ValidityArray[v];
				goto foundMatch;
			}
		}		//END OF VALIDITY LOOP
	}			//END OF DATE LOOP
      foundMatch:
	return jcoff;
}

//////////////////////////////////////////////////////////////////////////
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

//获取初始闭锁码的3个可变条件的“固定值”
void myGetInitCloseCodeVarItem(int *mdatetime, int *mvalidity, int *mclosecode)
{
	assert(NULL != mdatetime && NULL != mvalidity && NULL != mclosecode);
	if (NULL == mdatetime || NULL == mvalidity || NULL == mclosecode) {
		return;
	}
	//*mdatetime = myGetNormalTime(time(NULL), ZWMEGA);
	//20141113.1748.经过前两天的讨论，锁具初始闭锁码不能因为时间变化而变化
	//所以时间值定死为1400M秒，或者其实哪个过去的方便人识别的时间点都可以；
	//这些参数后续要改为可以配置的，起码要可以通过函数调用来配置，最好能
	//使用配置文件来配置
	*mdatetime = 1400*ZWMEGA;
	*mvalidity = 1000;
	*mclosecode = 10000000;
}

//获取闭锁码的3个可变条件的“固定值”
void myGetCloseCodeVarItem(int *mdatetime, int *mvalidity, int *mclosecode)
{
	const int ZW_CLOSECODE_BASEINPUT = 20000000;	//计算正常的闭锁码时，m_closecode字段的固定值
	assert(NULL != mdatetime && NULL != mvalidity && NULL != mclosecode);
	if (NULL == mdatetime || NULL == mvalidity || NULL == mclosecode) {
		return;
	}
	*mdatetime = myGetNormalTime(time(NULL), ZW_CLOSECODE_STEP);
	*mvalidity = 1440;
	*mclosecode = ZW_CLOSECODE_BASEINPUT;
}
