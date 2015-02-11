#include "stdafx.h"
#include "sha1.h"
#include "zwSecretBoxAuth.h"
#include "zwHidComm.h"
#include "zwHidSplitMsg.h"
#include "base64arduino.h"
#include "zwTimerHdr.h"
#ifdef WIN32
#include <windows.h>
#endif // WIN32
using std::string;
const int MAXLEN=JCHID_FRAME_LENGTH*8;


////////////////////////////////便于C#等高层语言调用的简洁界面封装//////////////////////////////////////////
JCHID cs_hid;	
//打开密盒HID通道,返回0代表失败，其他代表成功
int zwSecboxHidOpen(void)
{		
	ZWTRC
	JCHID_STATUS status=JCHID_STATUS_OK;
	memset(&cs_hid,0,sizeof(JCHID));
	cs_hid.vid=JCHID_VID_2014;
	cs_hid.pid=JCHID_PID_SECBOX;
	status=jcHidOpen(&cs_hid);
	if (JCHID_STATUS_OK!=status)
	{
		return 0;
	}
	else
	{
		return (int)(&cs_hid);
	}
}

void zwSecboxHidClose(int handleHid)
{
	ZWTRC
	jcHidClose((JCHID *)handleHid);
}

//通过HID发送授权验证请求到密盒
void zwSendAuthReq2SecBox(int handleHid)
{	
	ZWTRC
	char secReq[JC_HIDMSG_PAYLOAD_LEN];
	int seqRealLen=0;
	assert(sizeof(SECBOX_AUTH)<=JC_HIDMSG_PAYLOAD_LEN);

	memset(secReq,0,JC_HIDMSG_PAYLOAD_LEN);
	zwSecboxAuthByteGen(secReq,&seqRealLen,JC_SECBOX_AUTH_REQUEST);
	JCHID_STATUS sts=jcHidSendData((JCHID *)handleHid,secReq,seqRealLen);
	//if (JCHID_STATUS_FAIL==sts)
	//{
	//	jcHidOpen(&cs_hid);
	//	sts=jcHidSendData(&cs_hid,secReq,seqRealLen);
	//	OutputDebugStringA("JCHID PLUGOUT/IN RECONNECT by Zhou Wei.20141023.0949");
	//}
	
}

//通过HID接收密盒反应并验证，成功返回0，其他值代表失败
int zwVerifyAuthRspFromSecBox(int handleHid)
{
	ZWTRC
	char recvBuf[JCHID_FRAME_LENGTH];
	int recvRealLen=0;
	int verf=0;
	memset(recvBuf,0,JCHID_FRAME_LENGTH);
	jcHidRecvData((JCHID *)handleHid,recvBuf,JCHID_FRAME_LENGTH,&recvRealLen,JCHID_RECV_TIMEOUT);
	//上位机对于收到的密盒应答进行验证，返回0为成功，其他值为失败
	verf=zwSecboxAuthVerify((SECBOX_AUTH *)(recvBuf+sizeof(SECBOX_DATA_INFO)));
	if (0==verf)
	{	
		//printf("Good Secbox !\n");
		return 0;
	}
	else
	{
		printf("FAKESecretBox\n");
		return 1;
	}
}

void zwWriteData2SecretBox(int handleHid,const int index,const char *dataB64)
{
	ZWTRC
	//参数检查			
	assert(NULL!=handleHid);
	assert(index>=0 && index<=10);
	assert(NULL!=dataB64);	
	int dataLen=strlen(dataB64);
	assert(dataLen>0 && dataLen<=MAXLEN);	
	if (dataLen>MAXLEN)
	{
		printf("incoming Data exceed max data len.20141017.1418");
		return;
	}
//////////////////////////////////////////////////////////////////////////
	char b64Dec [MAXLEN];
	char writeDataBuf[MAXLEN];
	memset(b64Dec,0,MAXLEN);
	memset(writeDataBuf,0,MAXLEN);
	int b64DecLen=base64_decode(b64Dec,const_cast<char *>(dataB64),dataLen);

//////////////////////////////////////////////////////////////////////////
	int outLen=0;
	pc2BoxDataWriteRequest(index,b64Dec,b64DecLen,writeDataBuf,&outLen);
	jcHidSendData((JCHID *)handleHid,writeDataBuf,outLen);
//写操作之后读取赵工原样返回的数据包，是必要的操作，否则会导致后续的读取，认证等操作受到错误的返回数据包
//20141021.1633.周伟
	char recvBuf[MAXLEN];
	memset(recvBuf,0,MAXLEN);
	jcHidRecvData((JCHID *)handleHid,recvBuf,MAXLEN,&outLen,JCHID_RECV_TIMEOUT);

}

//boost::mutex sboxB64_mutex;

const char * zwReadDataFromSecretBox(int handleHid,const int index)
{
	ZWTRC
	assert(NULL!=handleHid);
	assert(index>=0);
	//////////////////////////////////////////////////////////////////////////
	//生成并发送数据读取请求
	char readReqBuf[JCHID_FRAME_LENGTH];
	memset(readReqBuf,0,JCHID_FRAME_LENGTH);
	int readReqLen=0;
	pc2BoxDataReadRequest(index,readReqBuf,&readReqLen);
	assert(readReqLen>0);
	jcHidSendData((JCHID *)handleHid,readReqBuf,readReqLen);
	//接收锁具返回的数据包
	char dataRecvBuf[MAXLEN];
	char dataPayloadBuf[MAXLEN];
	memset(dataRecvBuf,0,MAXLEN);
	memset(dataPayloadBuf,0,MAXLEN);
	int dataRecvLen=0;
	int dataPayloadLen=0;
	jcHidRecvData((JCHID *)handleHid,dataRecvBuf,MAXLEN,&dataRecvLen,JCHID_RECV_TIMEOUT);
	assert(dataRecvLen>0);
	//解析出来锁具返回的数据的有效载荷
	box2PcDataParse(dataRecvBuf,dataRecvLen,dataPayloadBuf,&dataPayloadLen);
	
	assert(dataPayloadLen>0);
	{
		//boost::mutex::scoped_lock sb64(sboxB64_mutex);		
		static char b64Enc[MAXLEN];		
		memset(b64Enc,0,MAXLEN);
		int b64EncLen=base64_encode(b64Enc,dataPayloadBuf,dataPayloadLen);
		return b64Enc;
	}	
}