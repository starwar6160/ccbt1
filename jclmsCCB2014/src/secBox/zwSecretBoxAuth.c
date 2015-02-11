#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "sha1.h"
#include "zwSecretBoxAuth.h"
#include "zwHidComm.h"
#include "zwHidSplitMsg.h"
#ifdef WIN32
#include <windows.h>
#endif // WIN32


#define ZWDBGOUT_SECBOX1226	//控制密盒这一部分的调试输出
//上下位机各自使用自己的PSK生成数据，验证对方发来的数据时，使用对方的PSK来验证；
//注意，这两个值属于机密；
//此外，这两个值最好改用char数组，避免大小端问题
//0x87,0x34,0xA1,0x5D,0x49,0x24,0x68,0xC9,0x7A,0xE0,0x00,0x00,0x30,0x32,0x00,0x4E,0x4C,0x21,0x72,0x6E,
//0x24,0x68,0xC9,0x7A,0xE0,0x00,0x00,0x30,0x32,0x00,0x4E,0x4C,0x21,0x72,0x6E,0x42,0x66,0x00,0x00,0x00,
const unsigned char SECBOXPSK2014D[SHA1DGSTLEN]=
{0x87,0x34,0xA1,0x5D,0x49,0x24,0x68,0xC9,0x7A,0xE0,0x00,0x00,0x30,0x32,0x00,0x4E,0x4C,0x21,0x72,0x6E};    
const unsigned char SECBOXPSK2014Z[SHA1DGSTLEN]=
{0x24,0x68,0xC9,0x7A,0xE0,0x00,0x00,0x30,0x32,0x00,0x4E,0x4C,0x21,0x72,0x6E,0x42,0x66,0x00,0x00,0x00};

void myDumpPsk1016(void)
{
	int i=0;
	for (i=0;i<20;i++)
	{
		printf("0x%02X,",SECBOXPSK2014D[i] & 0xFF);
	}
	printf("\n");
	for (i=0;i<20;i++)
	{
		printf("0x%02X,",SECBOXPSK2014Z[i] & 0xFF);
	}
	printf("\n");

}

#ifdef _DEBUG_1015
void zwSecretBoxAuthTest1015A(void)
{
	//void sha1(void *dest, const void* msg, uint32_t length_b);	
	const char *mystr="zhouwei test value and";
	int lenb=strlen(mystr)*sizeof(char)*8;
	char dest[20];
	memset(dest,0,20);
	//sha1(dest,mystr,lenb);
	sha1(dest,SECBOXPSK2014D,160);
	{
		int i=0;
		for (i=0;i<20;i++)
		{
			if (i>0 && i % 4 ==0)
			{
				printf(" ");
			}
			printf("%02X",dest[i] & 0xFF);
		}
		printf("\n");
	}
}
#endif // _DEBUG_1015

//利用硬件获得SHA1计算的20字节(160bit)随机值
//请保证输出缓冲区至少有20字节
void zwRandSeedWin32(char *randBuf)
{
	LARGE_INTEGER rnd;
	int pos=0;
	int i=0;
	assert(NULL!=randBuf);
	for (i=0;i<10;i++)
	{
		QueryPerformanceCounter(&rnd);
		sha1(randBuf,&rnd,sizeof(LARGE_INTEGER)*8);	
		Sleep(1);	//利用进程调度的不确定性增加随机性
	}
}

void myHexDump1016( int  outReqLen, char * outReq )
{
	int i=0;
	assert(outReqLen>0);
	assert(NULL!=outReq);
//#ifdef _DEBUG
	printf("\n");
	for (i=0;i<(outReqLen);i++)
	{
		if (i>0 && i % 2 ==0)
		{
			printf(" ");
		}
		printf("%02X",outReq[i] & 0xFF);
	}
	printf("\n");
//#endif // _DEBUG
}

//生成随机的认证请求,以及认证回复，输出到SECBOX_AUTH结构体，最长为HID帧长度64字节
//原理在于，上下位机都采用一个随机数+共同持有并保密的PSK生成一个SHA1结果，
//然后把随机数和SHA1结果发给对方作为验证的方式
void zwSecboxAuthDataGen(SECBOX_AUTH *req)
{
	char tmpBuf[JCHID_FRAME_LENGTH];
	char sha1dgst[SHA1DGSTLEN];
	char tmpRand[SHA1DGSTLEN];
	
	time_t now=0;
	assert(NULL!=req);
	if (NULL==req)
	{
		return;
	}

	
//////////////////////////////////////////////////////////////////////////
	memset(tmpBuf,0,JCHID_FRAME_LENGTH);
	memset(sha1dgst,0,SHA1DGSTLEN);
	memset(req,0,sizeof(SECBOX_AUTH));

	zwRandSeedWin32(tmpRand);
	memcpy(tmpBuf,tmpRand,SHA1DGSTLEN);	//当前时间rdtsc来源生成的HASH随机数复制到缓冲区
	memcpy(tmpBuf+SHA1DGSTLEN,SECBOXPSK2014D,SHA1DGSTLEN);	//PSK的二进制形式复制到缓冲区
	sha1(sha1dgst,tmpBuf,(2*SHA1DGSTLEN)*8);	//时间和PSK连起来做SHA1得出结果sha1dgst,作为“随机数”
	memcpy(req->rand_data,sha1dgst,SHA1DGSTLEN);	//把生成的“随机数”放到结构体里面对应字段；
//////////////////////////////////////////////////////////////////////////
	memset(tmpBuf,0,JCHID_FRAME_LENGTH);
	memset(sha1dgst,0,SHA1DGSTLEN);

	memcpy(tmpBuf,req->rand_data,SHA1DGSTLEN);	//前一步计算出来的“随机数”复制到缓冲区
	memcpy(tmpBuf+SHA1DGSTLEN,SECBOXPSK2014D,SHA1DGSTLEN);	//PSK的二进制形式复制到缓冲区
	sha1(sha1dgst,tmpBuf,(2*SHA1DGSTLEN)*8);	//"随机数”和PSK连起来做SHA1得出结果sha1dgst,作为请求中的SHA1结果字段
	memcpy(req->sha1_dgst,sha1dgst,SHA1DGSTLEN);	//把生成的SHA1结果字段放到结构体里面对应字段；
}

//生成包括前导的2字节“包类型”+2字节“包长度”在内的密盒授权请求和应答字节串
void zwSecboxAuthByteGen(char *outReq,int *outLen,JC_SECBOX_TYPE type)
{
	SECBOX_AUTH req;	
	short int msgType=type;
	int outReqLen=0;
	SECBOX_DATA_INFO info;	
	assert(NULL!=outReq);
	assert(type>=0 && type <=256*128);
//////////////////////////////////////////////////////////////////////////	
	info.msg_type=(unsigned char)msgType;
	info.data_index=0;
	info.data_len=HtoNs(sizeof(SECBOX_AUTH));	
	memcpy(outReq,&info,sizeof(SECBOX_DATA_INFO));
	zwSecboxAuthDataGen(&req);
	memcpy(outReq+sizeof(SECBOX_DATA_INFO),&req,sizeof(SECBOX_AUTH));
	outReqLen=sizeof(SECBOX_DATA_INFO)+sizeof(SECBOX_AUTH);
	*outLen=outReqLen;
	//myHexDump1016(outReqLen, outReq);
}


//////////////////////////////////////////////////////////////////////////

//生成随机的回复认证应答,输出到SECBOX_AUTH结构体，最长为HID帧长度64字节
//返回0代表认证成功，其他值代表失败；
//输入和应答都通过同一个结构体req输入输出
int zwSecboxAuthVerify(SECBOX_AUTH *req)
{
	char tmpBuf[JCHID_FRAME_LENGTH];
	char sha1dgst[SHA1DGSTLEN];
	int rcmp=0;	//保存memcpy比较结果
	time_t now=0;
	//////////////////////////////////////////////////////////////////////////
	assert(NULL!=req);
	if (NULL==req)
	{
		return 2;
	}
	
	//////////////////////////////////////////////////////////////////////////
	memset(tmpBuf,0,JCHID_FRAME_LENGTH);
	memset(sha1dgst,0,SHA1DGSTLEN);

	memcpy(tmpBuf,req->rand_data,SHA1DGSTLEN);	//输入进来的随机数(20字节的SHA1结果长度)复制到缓冲区
	memcpy(tmpBuf+SHA1DGSTLEN,SECBOXPSK2014Z,SHA1DGSTLEN);	//PSK的二进制形式复制到缓冲区
	sha1(sha1dgst,tmpBuf,(2*SHA1DGSTLEN)*8);	//时间和PSK连起来做SHA1得出结果sha1dgst,作为“随机数”
	rcmp=memcmp(sha1dgst,req->sha1_dgst,SHA1DGSTLEN);
	if (0!=rcmp)
	{
		//////////////////////////////////////////////////////////////////////////
		printf("req->randata is ");
		myHexDump1016(SHA1DGSTLEN,req->rand_data);
		printf("req->sha1_dgst is");
		myHexDump1016(SHA1DGSTLEN,req->sha1_dgst);
		printf("Expect SHA1DGST IS ");
		myHexDump1016(SHA1DGSTLEN,sha1dgst);
		myDumpPsk1016();
		//////////////////////////////////////////////////////////////////////////
		return rcmp;	//如果两个HASH值不等，也就是认证失败，返回memcpy的结果
	}
	//////////////////////////////////////////////////////////////////////////
	//如果认证通过了，就用上位机同样的算法生成随机应答

	return rcmp;
}

//////////////////////////////////数据读写函数////////////////////////////////////////

//获取消息类型以及长度；
void zwGetDataMsgTypeLen(const char *msg,JC_SECBOX_TYPE *type,int *msgLen)
{
	SECBOX_DATA_INFO *pInfo=(SECBOX_DATA_INFO *)msg;
	assert(NULL!=msg);
	assert(NULL!=type);
	assert(NULL!=msgLen);

	*type=pInfo->msg_type;
	*msgLen=NtoHs(pInfo->data_len);
}

//生成PC到密盒的数据读取请求，请保证输出缓冲区至少有SECBOX_DATA_INFO这么长
//index指定读取多少号数据
void pc2BoxDataReadRequest(const unsigned char index, char *outBuf,int *outLen)
{
	SECBOX_DATA_INFO info;	
	assert(NULL!=outBuf);
	assert(NULL!=outLen);
	info.msg_type=JC_SECBOX_DATA_READ_REQUEST;
	info.data_index=index;	//指定要读取的数据的索引号
	info.data_len=HtoNs(0);	//数据不用有任何内容，所以长度为0
	memcpy(outBuf,&info,sizeof(SECBOX_DATA_INFO));
	*outLen=sizeof(SECBOX_DATA_INFO);
};

//分析密盒发回的读取数据，并输出数据内容和长度
void box2PcDataParse(const char *inData,const int inLen,
						char *outBuf,int *outLen)
{
	SECBOX_DATA_INFO *info=(SECBOX_DATA_INFO *)inData;
	int inRealLen=NtoHs(info->data_len);
	assert(NULL!=inData && inLen>=sizeof(SECBOX_DATA_INFO));
	assert(NULL!=outBuf && NULL!=outLen);

	memcpy(outBuf,inData+sizeof(SECBOX_DATA_INFO),inRealLen);
	*outLen=inRealLen;
}

//生成PC到密盒的数据写入请求，请保证输出缓冲区至少有"SECBOX_DATA_INFO+实际数据长度"这么长
void pc2BoxDataWriteRequest(const unsigned char index,const char *inData,const int inLen,
						char *outBuf,int *outLen)
{
	SECBOX_DATA_INFO info;	
	assert(NULL!=inData && inLen>0);
	assert(NULL!=outBuf && NULL!=outLen);

	info.msg_type=JC_SECBOX_DATA_WRITE;
	info.data_index=index;
	info.data_len=HtoNs(inLen);	
	memcpy(outBuf,&info,sizeof(SECBOX_DATA_INFO));
	memcpy(outBuf+sizeof(SECBOX_DATA_INFO),inData,inLen);
	*outLen=sizeof(SECBOX_DATA_INFO)+inLen;
};

