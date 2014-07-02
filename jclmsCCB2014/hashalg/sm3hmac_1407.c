#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <memory.h>
#ifdef  __cplusplus
extern "C" {
#endif
//#include "sha2.h"
//#include "hmac_sha2.h"
#ifdef  __cplusplus
	extern "C" {
#endif

typedef unsigned char BYTE;
#include "sm3.h"


const uint8_t ZW_INPAD_FILL_CHAR=0x36;
const uint8_t ZW_OUTPAD_FILL_CHAR=0x5C;
void zwBinPrint2Hex(BYTE * src, int32_t srclen);

//把一串二进制数据输出为HEX格式
void zwBinPrint2Hex(BYTE * src, int32_t srclen)
{
	int32_t i = 0;

	assert(src != NULL);
	assert(srclen > 0);
	if (src==NULL || srclen==0)
	{
		return ;
	}

	for (i = 0; i < srclen; i++) {
		printf("%02X", src[i]);
	}
	printf("\n");
}

//6个参数实际上是3个，密钥，消息，输出的摘要
int32_t zwSm3Hmac(const char *psk,const int32_t pskLen,
				  const char *message,const int32_t msgLen,
				  char *outHmac,const int32_t outHmacLen)
{
	SM3 sm3p1,sm3p2;
	uint8_t sm3hash_t1[ZWSM3_DGST_LEN];	//中间HASH结果pass1保存的地方
	//uint8_t sm3hash_t2[ZWSM3_DGST_LEN];	//中间HASH结果pass2保存的地方
	//////////////////////////////////////////////////////////////////////////
	uint8_t inpad[ZWSM3_BLOCK_LEN];
	uint8_t outpad [ZWSM3_BLOCK_LEN];
	int32_t i=0;
#ifdef _DEBUG_SM3_DEBUGOUT20140328
	printf("*****************%s Start\n",__FUNCTION__);
	printf("pskLen=%d\tmessageLen=%d\toutHmacLen=%d\n",
		pskLen,msgLen,outHmacLen);
#endif // _DEBUG_SM3_DEBUGOUT20140328
	//参数检查断言
	assert(psk!=NULL);
	assert(pskLen<=ZWSM3_BLOCK_LEN);
	assert(message!=NULL);
	assert(msgLen>0);
	assert(outHmac!=NULL);
	//20140701,由于不明原因，暂且不用这个检查
	//assert(outHmac[0]==outHmac[1]);
	assert(outHmacLen==ZWSM3_DGST_LEN);
	//参数检查，如果不符合条件，就返回一个很大的负数
	if (psk==NULL || message==NULL ||outHmac==NULL)
	{
		return -1608;
	}
	if (pskLen>ZWSM3_BLOCK_LEN || pskLen<=0 ||
		msgLen<=0 || outHmacLen!=ZWSM3_DGST_LEN)
	{
		return -1609;
	}
	//////////////////////////////////////////////////////////////////////////
	//组合第一趟HASH所需的内容，psk放在开头，余下部分被ZW_INPAD_FILL_CHAR填充
	memset(inpad,ZW_INPAD_FILL_CHAR,ZWSM3_BLOCK_LEN);
	memcpy(inpad,psk,pskLen);
#ifdef _DEBUG_SM3_DEBUGOUT20140328
	printf("IN SM3 HMAC:inpad\n");
	zwBinPrint2Hex(inpad,ZWSM3_BLOCK_LEN);
#endif // _DEBUG_SM3_DEBUGOUT20140328
	memset(&sm3p1,0,sizeof(sm3p1));
	//计算第一趟HASH结果
	SM3_init(&sm3p1);
	for (i=0;i<ZWSM3_BLOCK_LEN;i++)
	{
		SM3_process(&sm3p1,inpad[i]);
	}
	for (i=0;i<msgLen;i++)
	{
		SM3_process(&sm3p1,message[i]);
	}
	memset(sm3hash_t1,0,ZWSM3_DGST_LEN);
	SM3_hash(&sm3p1,(char *)sm3hash_t1);
#ifdef _DEBUG_SM3_DEBUGOUT20140328
	printf("IN SM3 HMAC:sm3hash_t1\n");
	zwBinPrint2Hex(sm3hash_t1,ZWSM3_DGST_LEN);
#endif // _DEBUG_SM3_DEBUGOUT20140328
	//////////////////////////////////////////////////////////////////////////
	//计算第二趟HASH结果
	memset(outpad,ZW_OUTPAD_FILL_CHAR,ZWSM3_BLOCK_LEN);
	memcpy(outpad,psk,pskLen);
#ifdef _DEBUG_SM3_DEBUGOUT20140328
	printf("IN SM3 HMAC:outpad\n");
	zwBinPrint2Hex(outpad,ZWSM3_BLOCK_LEN);
#endif // _DEBUG_SM3_DEBUGOUT20140328
	//memset(sm3hash_t2,0,ZWSM3_BLOCK_LEN);
	memset(&sm3p2,0,sizeof(sm3p2));
	SM3_init(&sm3p2);
	for (i=0;i<ZWSM3_BLOCK_LEN;i++)
	{
		SM3_process(&sm3p2,outpad[i]);
	}
	for (i=0;i<ZWSM3_DGST_LEN;i++)
	{
		SM3_process(&sm3p2,sm3hash_t1[i]);
	}
	SM3_hash(&sm3p2,outHmac);
#ifdef _DEBUG_SM3_DEBUGOUT20140328
	printf("IN SM3 HMAC:sm3hash_outHmac\n");
	zwBinPrint2Hex((char *)outHmac,ZWSM3_DGST_LEN);
	printf("*****************%s End\n",__FUNCTION__);
#endif // _DEBUG_SM3_DEBUGOUT20140328
	return 0;
}

void zwsm3hmacTest1(void)
{
	const char *psk="hellopsk";
	const char *msg="mysm3testmessage";
	int32_t i=0;
	uint8_t outHmac[ZWSM3_DGST_LEN];
	memset(outHmac,0,ZWSM3_DGST_LEN);
	zwSm3Hmac(psk,strlen(psk),msg,strlen(msg),(char *)outHmac,ZWSM3_DGST_LEN);
	printf("T1537B %s:\n",__FUNCTION__);
	for (i=0;i<ZWSM3_DGST_LEN;i++)
	{
		printf("%02X",outHmac[i]);
	}
	printf("\n");
}

//做一个实际运行的机器上的SM3计算，针对一个已知值A，把结果与PC上的SM3(A)比对，
//返回值为0代表正确，非0代表错误，那么整个算法不用运行下去了；
int zwSM3SelfTest( void )
{
	const char *msg="mysm3testmessage";
	const int dstCorrectSM3Result=-1026675421;
	SM3 sm3;
	int i=0;
	uint8_t buf[ZWSM3_BLOCK_LEN];
	uint8_t outHmac[ZWSM3_DGST_LEN];
	int *correctSM3Result=0;
	memset(&sm3,0,sizeof(sm3));
	memset(buf,0,ZWSM3_BLOCK_LEN);
	memcpy(buf,msg,strlen(msg));
	//printf("[NOTICE] Base SM3 test %s:\n",__FUNCTION__);
	SM3_init(&sm3);
	for (i=0;i<ZWSM3_BLOCK_LEN;i++)
	{
		SM3_process(&sm3,buf[i]);
	}
	memset(outHmac,0,ZWSM3_DGST_LEN);
	SM3_hash(&sm3,(char *)(outHmac));
#ifdef _DEBUG_SM3_DEBUGOUT20140328
	for (i=0;i<ZWSM3_DGST_LEN;i++)
	{
		printf("%02X",outHmac[i]);
	}
#endif // _DEBUG_SM3_DEBUGOUT20140328
	//强制转换outHmac开头4字节为一个无符号整数，用作检验结果是否正确
	//的一个高概率方案，错误几率是2的32次方之一
	correctSM3Result=(int *)outHmac;
#ifdef _DEBUG_SM3_DEBUGOUT20140328
	printf("\n");
	printf("correctSM3Result=\t%d\n",*correctSM3Result);
	printf("dstCorrectSM3Result=\t%d\n",dstCorrectSM3Result);
#endif // _DEBUG_SM3_DEBUGOUT20140328

	//返回值是实际结果与正确值之差，如果非零，就说明SM3算法验证失败
	return (*correctSM3Result)-dstCorrectSM3Result;
}


