#include "zwHidSplitMsg.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

//计算一个简单的数据校验和
int zwDataSum26( const char *Data,const int DataLen )
{
	int sum=0;
	int i=0;
	const int SUMMOD=67108859;	//是precprime(2^26)的结果，2^26以下的最大质数
	unsigned char *du=(unsigned char *)Data;
	assert(Data!=NULL && DataLen>0);
	if (Data==NULL || DataLen<=0)
	{
		return 0;
	}	
	for (i=0;i<DataLen;i++)
	{		
		int t=du[i];
		assert(t>=0 && t<256);
		sum*=31;
		sum+=t;
		if (sum>SUMMOD)
		{
			sum%=SUMMOD;
		}		
	}
	return sum;
}

//以自定义的特殊模式填充内存，以便查找问题
void zwDebugFillMemory(unsigned char *data,const int dataLen)
{
	int i=0;
	for (i=3*sizeof(short int);i<dataLen;i++)
	{
		data[i]=(i % 256);
	}
}

//////////////////////////////////////////////////////////////////////////
//给定输入数据指针inData,长度inDataLen，
//输出结构体数组首个成员指针outMsg，结构体数组成员个数
//把输入切分为一个个输出结构体，正好符合HID要求的64字节大小，可以直接发送
JC_SPLIT_STATUS jcMsgMulPartSplit(const char *inData,const int inLen,JC_MSG_MULPART outMsg[],const int outCount)
{
	int i=0;
	int inBlocks=inLen/JC_HIDMSG_PAYLOAD_LEN;
	int inTail=inLen % JC_HIDMSG_PAYLOAD_LEN;
	assert(NULL!=inData);
	assert(inLen>0);
	assert(NULL!=outMsg);
	assert(outCount>0 && outCount<256);
	if (NULL==inData || inLen<=0 || NULL==outMsg || outCount<=0 || outCount>256)
	{
		return JC_SP_INPUT_NULL;
	}
	if (inTail>0)
	{
		inBlocks=inBlocks+1;	//如果末尾一块不是完整块，则加上一块
	}
	assert(inBlocks<=outCount);	//不能超出输出缓冲区容量
	if (inBlocks>outCount)
	{
		return JC_SP_OUTBUF_TOO_SHORT;
	}
	memset(outMsg,0,sizeof(JC_MSG_MULPART)*outCount);
	//printf("INFO:HidSplit Block:nIndex:nTotalBlock:nDataLength\n");
	for (i=0;i<inBlocks;i++)
	{
		int nStart=i*JC_HIDMSG_PAYLOAD_LEN;
		int nEnd=(i+1)*JC_HIDMSG_PAYLOAD_LEN;
		//最后一块不完整块处理
		if (nEnd>inLen)
		{
			nEnd=nStart+(inLen % JC_HIDMSG_PAYLOAD_LEN);
		}
		//zwDebugFillMemory((unsigned char *)(outMsg+i),sizeof(outMsg[i]));
		outMsg[i].nIndex=i;
		outMsg[i].nTotalBlock=inBlocks;
		outMsg[i].nDataLength=nEnd-nStart;
		//数字字段转换为网络字节序
		outMsg[i].nIndex		=HtoNs(outMsg[i].nIndex);
		outMsg[i].nTotalBlock	=HtoNs(outMsg[i].nTotalBlock);
		outMsg[i].nDataLength	=HtoNs(outMsg[i].nDataLength);		
		//printf("HidSplit Block HEX:nIndex=%04X\tnTotalBlock=%04X\tnDataLength=%04X\n",
		//	outMsg[i].nIndex,outMsg[i].nTotalBlock,outMsg[i].nDataLength);

		memcpy(outMsg[i].Data,inData+nStart,nEnd-nStart);
	}
	return JC_SP_OK;
}

JC_SPLIT_STATUS jcMsgMulPartMerge(const JC_MSG_MULPART inMsg[],const int inCount,char *outData,int outLen)
{
	int i=0;
	assert(NULL!=inMsg);
	assert(inCount>0);
	assert(NULL!=outData);
	assert(outLen>0);
	if (NULL==inMsg || inCount<=0 || NULL==outData || outLen<=0)
	{
		return JC_SP_INPUT_NULL;
	}
	//printf("\n");
	for (i=0;i<inCount;i++)
	{
		int nStart=i*JC_HIDMSG_PAYLOAD_LEN;
		int nSize=0;
		JC_MSG_MULPART tMsg;
		memcpy(&tMsg,inMsg+i,sizeof(JC_MSG_MULPART));
		nSize=NtoHs(tMsg.nDataLength);
		if (nStart+nSize>outLen)
		{
			return JC_SP_OUTBUF_TOO_SHORT;
		}
		
		//数字字段恢复为主机字节序
		tMsg.nIndex			=NtoHs(tMsg.nIndex);
		tMsg.nTotalBlock	=NtoHs(tMsg.nTotalBlock);
		tMsg.nDataLength	=NtoHs(tMsg.nDataLength);
#ifdef _DEBUG_ZWHIDCOMM
		printf("index=%d\tTotalBlock=%d\tCurBlockLen=%d\n",
			tMsg.nIndex,tMsg.nTotalBlock,tMsg.nDataLength);
#endif // _DEBUG_ZWHIDCOMM
		memcpy(outData+nStart,tMsg.Data,nSize);
	}
	return JC_SP_OK;
}

void test919(void)
{
	int i=0;
	int outCount=0;
	JC_MSG_MULPART s_mpSplit[JC_HIDMSG_SPLIT_NUM];
	const char *inData=
		"0123456789012345678901234567890123456789"
		"012345678901234567"
		"8901234567890123456789ABCDEFG"
		;
	char outBuf[JC_MSG_MAXSIZE];
	const int inLen=strlen(inData);
	jcMsgMulPartSplit(inData,inLen,s_mpSplit,JC_HIDMSG_SPLIT_NUM);
	outCount=NtoHs(s_mpSplit[0].nTotalBlock);
	for (i=0;i<outCount;i++)
	{
		char buf[JC_HIDMSG_PAYLOAD_LEN+1];
		memset(buf,0,JC_HIDMSG_PAYLOAD_LEN+1);
		memcpy(buf,s_mpSplit[i].Data,NtoHs(s_mpSplit[i].nDataLength));
#ifdef _DEBUG_ZWHIDCOMM
		printf("%s\n",buf);
#endif // _DEBUG_ZWHIDCOMM
	}

	memset(outBuf,0,JC_MSG_MAXSIZE);
	jcMsgMulPartMerge(s_mpSplit,NtoHs(s_mpSplit[0].nTotalBlock),outBuf,JC_MSG_MAXSIZE);
#ifdef _DEBUG_ZWHIDCOMM
	printf("outbuf is\n%s\n",outBuf);
#endif // _DEBUG_ZWHIDCOMM
}