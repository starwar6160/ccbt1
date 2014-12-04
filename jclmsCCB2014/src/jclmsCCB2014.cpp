// jclmsCCB2014.cpp : 定义 DLL 应用程序的导出函数。
//
//#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "jclmsCCB2014.h"
#include "sm3.h"
#include "dCodeHdr.h"
#include "zwhidComm.h"
#include "zwHidSplitMsg.h"
#include "zwSecretBoxAuth.h"

void myCjsonTest1(void);
void zwJclmsReq2Json(const JCINPUT *p,char *outJson,const int outBufLen);

#define _DEBUG_USE_LMS_FUNC_CALL_20141202
extern "C"
{
//void	WINAPI	Sleep(uint32_t dwMilliseconds	);
	int crc32testmain1127();
};

//为了修补密盒没有RTC时钟做的临时性措施，从上位机传递时间下去
//保存在全局变量g_armEmuTime里面
////密盒没有RTC时钟的临时修补，20141128.1358.周伟
//#ifndef _WIN32
//#define time	zwArmEmuTime	
//static time_t g_armEmuTime=0;
//
//time_t zwArmEmuTime(time_t *inputTime)
//{
//	printf("DEBUG1128 %s g_armEmuTime=%d\n",__FUNCTION__,g_armEmuTime);
//	if (NULL!=inputTime)
//	{
//		return 0;
//	}
//	return g_armEmuTime;
//}
//#endif // _WIN32

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
	//myCjsonTest1();
	JCINPUT *pjc = new JCINPUT;
	assert(pjc != NULL);
	memset(pjc, 0, sizeof(JCINPUT));
	memset(pjc->AtmNo, 0, JC_ATMNO_MAXLEN + 1);
	memset(pjc->LockNo, 0, JC_LOCKNO_MAXLEN + 1);
	memset(pjc->PSK, 0, JC_PSK_LEN + 1);
//#ifdef _DEBUG
//	printf("sizeof JCINPUT=%d\n",sizeof(JCINPUT));
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
	printf("%s\n",__FUNCTION__);
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
	printf("%s:dyCode=%d\n",__FUNCTION__,res);
	return res;
}

	//离线模式匹配，时间点精度为取整到一个小时的零点，有效期精度为1小时起
	//如果找到了，返回JCOFFLINE中是匹配的时间和有效期，否则其中的值都是0
JCMATCH JCLMSCCB2014_API JcLockReverseVerifyDynaCode(const int handle,
						     const int dstCode)
{
	printf("%s dstCode=%d\n",__FUNCTION__,dstCode);
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
		printf("%d\t",tdate);	
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
			//printf("%d:%d\t",tdate,res);	
			//if (3==v)
			//{
			//	printf("\n");
			//}
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
		//printf("\n");
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
	*mdatetime = myGetNormalTime(*mdatetime, ZW_CLOSECODE_STEP);
	*mvalidity = 1440;
	*mclosecode = ZW_CLOSECODE_BASEINPUT;
}


//////////////////////////////////////////////////////////////////////////


void JCLMSCCB2014_API zwJclmsRsp( void * inLmsReq,const int inLmsReqLen,JCRESULT *lmsResult );

void myJcInputHton(JCINPUT *p)
{
	p->CodeGenDateTime=HtoNl(p->CodeGenDateTime);
	p->Validity=HtoNl(p->Validity);
	p->CloseCode=HtoNl(p->CloseCode);
	p->CmdType=static_cast<JCCMD>(HtoNl(p->CmdType));

	p->SearchTimeStart=HtoNl(p->SearchTimeStart);
	p->SearchTimeStep=HtoNl(p->SearchTimeStep);
	p->SearchTimeLength=HtoNl(p->SearchTimeLength);
	for (int i=0;i<NUM_VALIDITY;i++)
	{
		p->ValidityArray[i]=HtoNl(p->ValidityArray[i]);
	}
}

void myJcInputNtoh(JCINPUT *p)
{
	p->CodeGenDateTime=NtoHl(p->CodeGenDateTime);
	p->Validity=NtoHl(p->Validity);
	p->CloseCode=NtoHl(p->CloseCode);
	p->CmdType=static_cast<JCCMD>(NtoHl(p->CmdType));

	p->SearchTimeStart=NtoHl(p->SearchTimeStart);
	p->SearchTimeStep=NtoHl(p->SearchTimeStep);
	p->SearchTimeLength=NtoHl(p->SearchTimeLength);
	for (int i=0;i<NUM_VALIDITY;i++)
	{
		p->ValidityArray[i]=NtoHl(p->ValidityArray[i]);
	}
}

void myLmsReqZHton(JCLMSREQ *req)
{
	myJcInputHton(&req->inputData);
	req->op=static_cast<JCLMSOP>(HtoNl(req->op));
	req->dstCode=HtoNl(req->dstCode);
	req->timeNow=HtoNl(req->timeNow);
}

void myLmsReqZNtoh(JCLMSREQ *req)
{
	myJcInputNtoh(&req->inputData);
	req->op=static_cast<JCLMSOP>(NtoHl(req->op));
	req->dstCode=NtoHl(req->dstCode);
	req->timeNow=NtoHl(req->timeNow);
}


#ifdef _WIN32
void myHexDump(const void * hidSendBuf,const int outLen );
const int ZWHIDBUFLEN=512;
char g_dbg_hid_common1202[ZWHIDBUFLEN];
//两个zwJclmsReq函数是上位机专用

//填写完毕handle里面的数据结构以后，调用该函数生成动态码，该函数在底层将请求
//通过HID等通信线路发送到密盒，然后阻塞接收密盒返回结果，通过出参返回；
int JCLMSCCB2014_API zwJclmsReqGenDyCode( int lmsHandle,int *dyCode )
{
	JCLMSREQ req;
	memset(&req,0,sizeof(JCLMSREQ));
	req.op=JCLMS_CCB_CODEGEN;
	memcpy((void *)&req.inputData,(void *)lmsHandle,sizeof(JCINPUT));
	//////////////////////////////////模拟发送数据////////////////////////////////////////
	//此处由于是模拟，时序不好控制，为了便于调试，在此直接调用密盒端的函数zwJclmsRsp来做处理
	printf("%s Send Data to Secbox for Gen DynaCode:\n",__FUNCTION__);
	zwJcLockDumpJCINPUT(lmsHandle);	
	//////////////////////////////////////////////////////////////////////////
	//构建整个HID发送数据包，给下层HID函数去切分和发送
	char hidSendBuf[ZWHIDBUFLEN];
	memset(hidSendBuf,0,ZWHIDBUFLEN);
	//HID有效载荷的头部
	const int outLen=sizeof(SECBOX_DATA_INFO)+sizeof(JCLMSREQ);
	SECBOX_DATA_INFO hidPayloadHeader;
	memset(&hidPayloadHeader,0,sizeof(SECBOX_DATA_INFO));
	hidPayloadHeader.data_index=1;
	hidPayloadHeader.msg_type=JC_SECBOX_LMS_GENDYCODE;
	hidPayloadHeader.data_len=sizeof(JCLMSREQ);
	hidPayloadHeader.data_len=HtoNs(hidPayloadHeader.data_len);
	//先加入头部
	memcpy(hidSendBuf,&hidPayloadHeader,sizeof(hidPayloadHeader));
	//然后加入实际的请求部分
	myLmsReqZHton(&req);
	memcpy(hidSendBuf+sizeof(hidPayloadHeader),&req,sizeof(req));
	printf("HidSend Data is(Net ByteOrder)\n");
	myHexDump(hidSendBuf, outLen);
	//////////////////////////////////////////////////////////////////////////	
	JCRESULT rsp;
	memset(&rsp,0,sizeof(rsp));

#ifdef _DEBUG_USE_LMS_FUNC_CALL_20141202
	//调试状态，直接调用下位机函数即可
	zwJclmsRsp(hidSendBuf,outLen,&rsp);
#else
	JCHID hidHandle;
	memset(&hidHandle,0,sizeof(JCHID));
	hidHandle.vid=0x0483;
	hidHandle.pid=0x5710;

	if (JCHID_STATUS_OK != jcHidOpen(&hidHandle)) {
		return -1118;
	}
	jcHidSendData(&hidHandle,hidSendBuf,outLen);


	printf("GenWait To SecBox Return Result now..\n");
	int rspRealLen=0;
	jcHidRecvData(&hidHandle,(char *)&rsp,sizeof(rsp),&rspRealLen);
	printf("HidRecv Data is\n");
	myHexDump(&rsp, rspRealLen);

	assert(sizeof(rsp)==rspRealLen);
	if (sizeof(rsp)!=rspRealLen)
	{
		printf("Secbox Return of LMS result size not match JCRESULT!\n");
	}
	jcHidClose(&hidHandle);
#endif // _DEBUG_USE_LMS_FUNC_CALL_20141202
	//zwJclmsRsp(&req,sizeof(JCLMSREQ),&rsp);
	assert(0!=rsp.dynaCode);
	*dyCode=rsp.dynaCode;
	printf("%s Return dynaCode=%d\n",__FUNCTION__,rsp.dynaCode);

	return 0;
}


//填写完毕handle里面的数据结构以后，调用该函数验证动态码（第一和第二动态码中间，锁具生成的校验码
//也是使用其他两个动态码的同样算法生成的，所以也算一种动态码，该函数在底层将验证请求通过HID等
//通信线路发送到密盒，然后阻塞接收密盒返回结果，通过出参返回；
int JCLMSCCB2014_API zwJclmsReqVerifyDyCode( int lmsHandle,int dstCode,JCMATCH *match )
{
	JCLMSREQ req;
	memset(&req,0,sizeof(JCLMSREQ));
	req.op=JCLMS_CCB_CODEVERIFY;
	req.dstCode=dstCode;
	memcpy((void *)&req.inputData,(void *)lmsHandle,sizeof(JCINPUT));
	//////////////////////////////////模拟发送数据////////////////////////////////////////
	//此处由于是模拟，时序不好控制，为了便于调试，在此直接调用密盒端的函数zwJclmsRsp来做处理
	printf("%s Send Data to Secbox with Wait To Verify DestCode %d\n",__FUNCTION__,dstCode);
	zwJcLockDumpJCINPUT(lmsHandle);
	//////////////////////////////////////////////////////////////////////////
	//HID有效载荷的头部
	const int outLen=sizeof(SECBOX_DATA_INFO)+sizeof(JCLMSREQ);
	SECBOX_DATA_INFO hidPayloadHeader;
	memset(&hidPayloadHeader,0,sizeof(SECBOX_DATA_INFO));
	hidPayloadHeader.data_index=1;
	hidPayloadHeader.msg_type=JC_SECBOX_LMS_VERDYCODE;
	hidPayloadHeader.data_len=sizeof(JCLMSREQ);
	hidPayloadHeader.data_len=HtoNs(hidPayloadHeader.data_len);
	//构建整个HID发送数据包，给下层HID函数去切分和发送
	char hidSendBuf[ZWHIDBUFLEN];
	memset(hidSendBuf,0,ZWHIDBUFLEN);
	//先加入头部
	memcpy(hidSendBuf,&hidPayloadHeader,sizeof(hidPayloadHeader));
	//然后加入实际的请求部分
	myLmsReqZHton(&req);
	memcpy(hidSendBuf+sizeof(hidPayloadHeader),&req,sizeof(req));
	printf("HidSend Data is(Net ByteOrder)\n");
	myHexDump(hidSendBuf, outLen);

	//////////////////////////////////////////////////////////////////////////
	JCRESULT rsp;
	memset(&rsp,0,sizeof(rsp));

#ifdef _DEBUG_USE_LMS_FUNC_CALL_20141202
	//调试状态，直接调用下位机函数即可
	zwJclmsRsp(hidSendBuf,outLen,&rsp);
#else
	JCHID hidHandle;
	hidHandle.vid=0x0483;
	hidHandle.pid=0x5710;
	if (JCHID_STATUS_OK != jcHidOpen(&hidHandle)) {
		return -1118;
	}
	jcHidSendData(&hidHandle,hidSendBuf,outLen);
	printf("VerWait To SecBox Return Result now..\n");
	int rspRealLen=0;
	jcHidRecvData(&hidHandle,(char *)&rsp,sizeof(rsp),&rspRealLen);
	printf("HidRecv Data is\n");
	myHexDump(&rsp, rspRealLen);
	assert(rsp.verCodeMatch.s_datetime>1400*ZWMEGA);
	assert(sizeof(rsp)==rspRealLen);
	if (sizeof(rsp)!=rspRealLen)
	{
		printf("Secbox Return of LMS result size not match JCRESULT!\n");
	}
	jcHidClose(&hidHandle);
#endif // _DEBUG_USE_LMS_FUNC_CALL_20141202
	memcpy((void *)match,(void *)&rsp.verCodeMatch,sizeof(JCMATCH));
	printf("%s Match DateTime=%d\tValidity=%d\n",__FUNCTION__,match->s_datetime,match->s_validity);	
	return 0;
}
#endif // _WIN32


//该函数是下位机专用
//Input:void * inLmsReq:pointer of a JCLMSREQ struct
//input:const int inLmsReqLen:sizeof(JCLMSREQ)
//output:JCRESULT
void JCLMSCCB2014_API zwJclmsRsp( void * inLmsReq,const int inLmsReqLen,JCRESULT *lmsResult )
{
	//从外部接收数据
	JCLMSREQ lmsReq;

	assert(NULL!=inLmsReq);
	assert(NULL!=lmsResult);
	if (NULL==inLmsReq || NULL==lmsResult)
	{
		return;
	}
#ifdef _DEBUG_USE_LMS_FUNC_CALL_20141202
	//PC调试时输入大小必须是HID有效载荷头部+JCLMSREQ的大小
	assert(sizeof(SECBOX_DATA_INFO)+sizeof(JCLMSREQ)==inLmsReqLen);
	//跳过HID有效载荷头部
	memcpy((void *)&lmsReq,(char *)inLmsReq+sizeof(SECBOX_DATA_INFO),inLmsReqLen-sizeof(SECBOX_DATA_INFO));
#else
	//在ARM上输入大小必须是JCLMSREQ大小
	assert(sizeof(JCLMSREQ)==inLmsReqLen);
	//跳过HID有效载荷头部
	memcpy((void *)&lmsReq,(char *)inLmsReq,inLmsReqLen);
#endif // _DEBUG_USE_LMS_FUNC_CALL_20141202

	myLmsReqZNtoh(&lmsReq);
	zwJcLockDumpJCINPUT((int)(&lmsReq));

	//通过出参结构体返回计算结果给外部
	int dyCode=0;
	if (JCLMS_CCB_CODEGEN==lmsReq.op)
	{
		dyCode=zwJcLockGetDynaCode((int)(&lmsReq.inputData));
		assert(dyCode>10*ZWMEGA);
		lmsResult->dynaCode=dyCode;
	}
	if (JCLMS_CCB_CODEVERIFY==lmsReq.op)
	{
		JCMATCH jm=JcLockReverseVerifyDynaCode((int)(&lmsReq.inputData),lmsReq.dstCode);
		assert(jm.s_datetime>1400*ZWMEGA);
		assert(jm.s_validity>0 && jm.s_validity<=1440);
		memcpy((void *)&(lmsResult->verCodeMatch),(void *)&jm,sizeof(JCMATCH));
	}	
}


void myHexDump( const void * hidSendBuf,const int outLen )
{
	const char *pt=static_cast<const char *>(hidSendBuf);
	for (int i=0;i<outLen;i++)
	{
		if (i>0)
		{
			if(i%8==0 && i%16!=0)printf("\t");
			if(i%16==0)printf("\n");
		}
		unsigned char c=pt[i] & 0xFF;
		printf("%02X ",c);
	}
	printf("\n");
}

//一个纯算法层面的标准测试，测试了动态码生成和验证两个环节，用于ARM校验自己是否有编译器优化问题等等；
//20141203.1001.周伟
int zwLmsAlgStandTest20141203(void)
{
	int handle=0;
	int pass1DyCode=0;
	handle = JcLockNew();
	JcLockSetString(handle, JCI_ATMNO, "atm10455761");
	JcLockSetString(handle, JCI_LOCKNO, "lock14771509");
	JcLockSetString(handle, JCI_PSK, "PSKDEMO728");
	//////////////////////////////////////////////////////////////////////////
	//固定开锁时间,应该出来固定的结果
	const int ZWFIX_STARTTIME=1416*ZWMEGA;
	JcLockSetInt(handle,JCI_TIMESTEP,6);
	JcLockSetInt(handle,JCI_SEARCH_TIME_START,1416*ZWMEGA+127);
	JcLockSetCmdType(handle, JCI_CMDTYPE, JCCMD_INIT_CLOSECODE);
	//////////////////////////////////////////////////////////////////////////
	JCRESULT lmsRsp;
	//printf("zwJclmsReqGenDyCode initCloseCode\n");
	int initCloseCode=0;
	initCloseCode=JcLockGetDynaCode(handle);
	//这里是一个自检测试，如果失败，就说明有比较大的问题了，比如类似发生过的
	//ARM编译器优化级别问题导致的生成错误的二进制代码等等
	if(38149728!=initCloseCode)
	{
		printf("initCloseCode Gen Error! JCLMS Algorithm GenDynaCode Self Check Fail! 20141203\n");
		return -1;
	}
	JcLockSetInt(handle, JCI_CLOSECODE, initCloseCode);
	JcLockSetInt(handle,JCI_DATETIME,1416*ZWMEGA);
	JcLockSetInt(handle,JCI_SEARCH_TIME_START,1416*ZWMEGA+127);
	JcLockSetCmdType(handle, JCI_CMDTYPE, JCCMD_CCB_DYPASS1);
	pass1DyCode=JcLockGetDynaCode(handle);
	assert(57174184==pass1DyCode);
	JCMATCH pass1Match =
		JcLockReverseVerifyDynaCode(handle,pass1DyCode);
	if(ZWFIX_STARTTIME!=pass1Match.s_datetime)
	{
		printf("JcLockReverseVerifyDynaCode Error! JCLMS Algorithm Reverse DynaCode Self Check Fail! 20141203\n");
		return -2;
	}	
	//////////////////////////////////////////////////////////////////////////
	JcLockDelete(handle);
	return 0;
}
