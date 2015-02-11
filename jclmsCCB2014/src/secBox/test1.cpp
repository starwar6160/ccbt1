#include "stdafx.h"
#include "zwHidComm.h"
#include "zwSecretBoxAuth.h"
#include <windows.h>
#include "zwBaseLib.h"
#include "zwTest201407.h"
#pragma comment(lib,"zwBaseLib.lib")

void myMallocTest1(int argc, _TCHAR** argv)
{
	int a=333;
	int b=555;
	cout<<argc<<" "<<argv[0]<<endl;
	cout<<a<<"+"<<b<<"="<<zwTest717::zwAddTest717(a,b)<<endl;
	zwTest717::zwMemAllocTest01();
}


void myDesEDE2EncTest1(void)
{
	//明文可以是任意长度
	string plain="zw20140718.0905";
	string encoded;	//加密结果密文
	//key必须是64bit的整倍数，也就是16个HEX字符的整倍数
	string key="446FA6E22A22F0A2";
	cout<<"plain text is \t"<<plain<<endl;
	//加密测试
	encoded= zwCrypt::zwDesEDE2EcbEncCCBv1(plain,key);
	cout<<__FUNCTION__<<" "<<encoded<<endl;
	//解密测试
	string dePlain=zwCrypt::zwDesEDE2EcbDecCCBv1(encoded,key);
	cout<<__FUNCTION__<<" "<<dePlain<<endl;
}



void zw901StaticLibTest1(void)
{
	cout<<"Result from zw Static Lib is "<<zwsTest1::zwsTestAdd(11,22)<<endl;
}


void zwTimeTest1008(void)
{
	std::string outDate,outTime;
	time_t now=time(NULL);
	time_t tail=now % 300;
	now=now-tail;
	zwTimeFunc::zwGetLocalDateTimeString(now,outDate,outTime);
	time_t outTimeSec=0;
	zwTimeFunc::zwCCBDateTime2UTC(outDate.c_str(),outTime.c_str(),&outTimeSec);
	std::string oDate2,oTime2;
	zwTimeFunc::zwGetLocalDateTimeString(outTimeSec,oDate2,oTime2);
	cout<<"now=\t\t\t\t"<<now<<endl;
	cout<<"Date=\t"<<outDate<<"\tTime=\t"<<outTime<<endl;
	cout<<"zwCCBDateTime2UTC Result=\t"<<outTimeSec<<endl;
	cout<<"zwCCBDateTime2UTC out\n";
	cout<<"Date=\t"<<oDate2<<"\tTime=\t"<<oTime2<<endl;

}

#ifdef _DEBUG_0120
//20141009.最简单的纯HID通信方式测试；
JCHID_STATUS zwHidTest1009Simple(void)
{
	JCHID hidHandle;
	JCHID_STATUS sts=JCHID_STATUS_OK;
	memset(&hidHandle,0,sizeof(JCHID));
	hidHandle.vid=JCHID_VID_2014;
	hidHandle.pid=JCHID_PID_SECBOX;		
	sts=jcHidOpen(&hidHandle);
	if (JCHID_STATUS_OK!=sts)
	{
		//cout<<"VID=\t"<<hidHandle.vid<<"\t"<<"PID="<<hidHandle.pid<<endl;
		//printf("VID=%04X\tPID=%04X\n",hidHandle.vid,hidHandle.pid);
		return sts;
	}
	//////////////////////////////////////////////////////////////////////////
	const char *str="ZhouWeiTestHidSimple20141009.1507"
		"ZhouWeiTestHidSimple20141010.1637ZhouWeiTestHidSimple20141010.1647"
		;
	jcHidSendData(&hidHandle,str,strlen(str));
	Sleep(500);
	unsigned char recvData[JC_HIDMSG_PAYLOAD_LEN];
	memset(recvData,0,JC_HIDMSG_PAYLOAD_LEN);
	int recvLen=0;
	jcHidRecvData(&hidHandle,(char *)recvData,JC_HIDMSG_PAYLOAD_LEN,&recvLen,JCHID_RECV_TIMEOUT);
	printf("Read Payload Data From JcLock is:\n");
	for (int i=0;i<JC_HIDMSG_PAYLOAD_LEN;i++)
	{
		if (i>0 && i % 16 ==0)
		{
			printf("\n");
		}
		printf("%02X ",recvData[i]);
	}
	printf("\n");


	//////////////////////////////////////////////////////////////////////////
	if (NULL!=hidHandle.vid && NULL!=hidHandle.pid)
	{
		jcHidClose(&hidHandle);
	}

	return sts;
};
#endif // _DEBUG_0120

void mySecBoxAuthTest1016()
{
	assert(sizeof(SECBOX_AUTH)<=(JCHID_FRAME_LENGTH-8));
	char secReq[JCHID_FRAME_LENGTH];
	memset(secReq,0,JCHID_FRAME_LENGTH);
	//printf("zwSecboxAuthRequest is\n");
	//////////////////////////////////上位机开始发送认证请求////////////////////////////////////////
	//上位机生成认证请求随机字节串，接下来可以作为HID的有效载荷发送了
	printf("PC Send to SecBox AuthRequest is\n");
	int seqLen1=0;
	zwSecboxAuthByteGen(secReq,&seqLen1,JC_SECBOX_AUTH_REQUEST);
	//////////////////////////////////上位机结束发送认证请求////////////////////////////////////////

	//////////////////////////////下位机开始验证上位机身份////////////////////////////////////////////
	//下位机对于收到的认证请求进行验证，返回0为成功，其他值为失败
	int verf=zwSecboxAuthVerify((SECBOX_AUTH *)(secReq+sizeof(SECBOX_DATA_INFO)));
	//如果验证成功，则生成随机应答，然后通过HID发送给上位机
	//由于现在取的秒数作为随机数种子，所以不够随机，所以需要过1秒以上，才能使得下位机应答不同，还需要改用真随机数源
	//Sleep(1200);	
	if (0==verf)
	{	
		printf("Good PC!\n");
		printf("SecBox Recv from PC Data is\n");
		int seqLen2=0;
		zwSecboxAuthByteGen(secReq,&seqLen2,JC_SECBOX_AUTH_RESPONE);
	}
	else
	{
		printf("FAKE PC!\n");
	}
	//////////////////////////////下位机结束验证上位机身份////////////////////////////////////////////
	//////////////////////////////上位机开始验证密盒身份////////////////////////////////////////////
	//上位机对于收到的密盒应答进行验证，返回0为成功，其他值为失败
	verf=zwSecboxAuthVerify((SECBOX_AUTH *)(secReq+sizeof(SECBOX_DATA_INFO)));
#ifdef _DEBUG_ZWHIDCOMM
	if (0==verf)
	{	
		printf("Good Secbox !\n");
	}
	else
	{
		printf("FAKE Seedbox Found!\n");
	}
#endif // _DEBUG_ZWHIDCOMM

	//////////////////////////////上位机结束验证密盒身份////////////////////////////////////////////

}

#ifdef _DEBUG_0120
void mySecBoxAuthCSTest1017()
{
	int hnd=zwSecboxHidOpen();
	zwSendAuthReq2SecBox(hnd);
	zwVerifyAuthRspFromSecBox(hnd);
	zwSecboxHidClose(hnd);
}

void mySecBoxWriteTest1017()
{
	int hnd=zwSecboxHidOpen();
	//mySecBoxAuthCSTest1017();

	const char *myLongB64Str1="emhvdXdlaXRlc3RPdXRwdXREZWJ1Z1N0cmluZ0FuZEppbkNodUVMb2NraW5kZXg9MFRvdGFsQmxvY2s9MkN1ckJsb2NrTGVuPTU4U2VkaW5nIERhdGEgQmxvY2sgIzBSZWNldmVkIERhdGEgRnJvbSBKQ0VMb2NrIGlzOg==";
	const char *myShortB64Str2="emhvdXdlaQ==";
	zwWriteData2SecretBox(hnd,1,myLongB64Str1);
	//zwReadDataFromSecretBox(33,1);
	exit(111);

	const char *data1="ZhouWeiTestWrite2SecretBoxDataJinChuElock20141017.1403.zwHidTest1009Simple.zwSecretBoxAuthTest";
	const int BUFLEN=JCHID_FRAME_LENGTH*2;
	char outBufW[BUFLEN];
	memset(outBufW,0,BUFLEN);
	int outLen=0;
	pc2BoxDataWriteRequest(1,data1,strlen(data1),outBufW,&outLen);
	jcHidSendData((JCHID *)hnd,outBufW,outLen);
	char recvBuf[BUFLEN];
	memset(recvBuf,0,BUFLEN);
	jcHidRecvData((JCHID *)hnd,recvBuf,BUFLEN,&outLen,JCHID_RECV_TIMEOUT);
	zwSecboxHidClose(hnd);
}

void mySecBoxReadTest1018()
{
	int hnd=zwSecboxHidOpen();
	zwReadDataFromSecretBox(hnd,1);
	zwSecboxHidClose(hnd);
}
#endif // _DEBUG_0120