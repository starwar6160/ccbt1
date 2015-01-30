#include "stdafx.h"
#include <gtest/gtest.h>
#include "CCBelock.h"
#include "zwHidMulHeader.h"
#include "zwHidGtest130.h"
#define ZWUSEGTEST

#ifdef ZWUSEGTEST

CCBELOCK_API int zwStartGtestInDLL(void)
{
	int argc=1;
	char *argv[1];
	argv[0]=NULL;
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

namespace zwHidGTest20150130{
	const int G_BUFSIZE=1024;
	char s_recvMsg[G_BUFSIZE];
	char s_devList[G_BUFSIZE];

void ZJY1501STD myReturnMessageTest130(const char* DrivesIdSN,char* DrivesMessageJson)
{
	int inJsonLen=strlen(DrivesMessageJson);
	assert(inJsonLen<G_BUFSIZE);
	memset(s_recvMsg,0,G_BUFSIZE);
	printf("Callback Function %s:\trecvDataLen=%d\t",__FUNCTION__,inJsonLen);
	printf("devSerial=%s\t devReturnJson is:\n%s\n",DrivesIdSN,DrivesMessageJson);
	strcpy(s_recvMsg,DrivesMessageJson);
}

void ZJY1501STD myHidListTest130(const char* DrivesType,const char * DrivesIDList)
{
	//printf("jcHidDevType=%s\n",DrivesType);
	printf("USB PlugInOut Callback 20150130.1445\n");
	printf("Json List of enum jcHidDev Type %s Serial is:\n%s\n",DrivesType,DrivesIDList);
	memset(s_devList,0,G_BUFSIZE);
	strcpy(s_devList,DrivesIDList);
}


//�����׼���ʼ���ͽ����¼�
class ATMCDLLSelfTest : public testing::Test
{
public:
char cmdBuf[128];	

protected:
	virtual void SetUp();
	virtual void TearDown();
	
};

void ATMCDLLSelfTest::SetUp()
{
	SetReturnMessage(myReturnMessageTest130);
	const char *msgT5a="{\"command\":\"Test_USB_HID\",\"cmd_id\":\"1234567890\",\"input\":\"TestAnyString";
	const char *msgT5b="\",\"output\":\"\"}";	
	memset(cmdBuf,0,128);
	sprintf(cmdBuf,"%s%d%s",msgT5a,7,msgT5b);
}

void ATMCDLLSelfTest::TearDown()
{

}


TEST_F(ATMCDLLSelfTest, LockNormalUse)
{
	//���Դ��ⲿ�ַ���ID���������豸HASH�Ƿ���ȷ
	jch::zwJcHidDbg15A hid1;
	//�˴�������Ǳ�׼���ߵ�HASH
	hid1.SetElock(NULL);
	uint32_t hidHash=hid1.GetHash();
	cout<<"hidHash="<<hidHash<<endl;
	EXPECT_GT(hidHash,0);
	EXPECT_EQ(hidHash,2378562802);
	hid1.OpenHidDevice();
	//����s_recvMsg���ڴ��յ��ĳ��ȴ���0
	memset(s_recvMsg,0,G_BUFSIZE);
	hid1.PushJson(cmdBuf);
	Sleep(2000);
	EXPECT_GT(strlen(s_recvMsg),0);
}

TEST_F(ATMCDLLSelfTest, LockMultiOpen)
{
	//����2��������ͬһ���豸�����
	jch::zwJcHidDbg15A hid1,hid2;
	//�˴�������Ǳ�׼���ߵ�HASH
	hid1.SetElock(NULL);
	hid2.SetElock(NULL);
	cout<<"will SUCCESS Open HID Device 1"<<endl;
	EXPECT_EQ(jch::G_SUSSESS,hid1.OpenHidDevice());
	cout<<"will FAIL Open same HID Device 1"<<endl;
	EXPECT_EQ(jch::G_FAIL,hid2.OpenHidDevice());

}

TEST_F(ATMCDLLSelfTest, LockNotOpen)
{
	//����û��ִ��OpenHidDevice�Ķ���
	jch::zwJcHidDbg15A hdv;
	hdv.SetElock(NULL);
	EXPECT_EQ(jch::G_FAIL,hdv.PushJson(cmdBuf));
	Sleep(700);
}

TEST_F(ATMCDLLSelfTest, LockPushNULL)
{
	//���Է��Ϳյ������ַ����Ĳ���
	jch::zwJcHidDbg15A hdv;
	hdv.SetElock(NULL);
	hdv.OpenHidDevice();
	//����s_recvMsg���ڴ��յ��ĳ��ȵ���0����Ϊ���Ϳ������ַ���
	memset(s_recvMsg,0,G_BUFSIZE);
	EXPECT_EQ(jch::G_FAIL,hdv.PushJson(NULL));
	EXPECT_EQ(jch::G_FAIL,hdv.PushJson(""));
	Sleep(700);
	EXPECT_EQ(strlen(s_recvMsg),0);
}

TEST_F(ATMCDLLSelfTest, LockNotSetPara)
{
	//����û�����ò�����ִ��OpenHidDevice�Ķ���
	jch::zwJcHidDbg15A hdv;
	hdv.OpenHidDevice();
	EXPECT_EQ(jch::G_FAIL,hdv.PushJson(cmdBuf));
	Sleep(700);
}

//////////////////////////////////////////////////////////////////////////
TEST_F(ATMCDLLSelfTest, jcHidDevEnumNormal)
{
	//ö���豸�������������
	memset(s_devList,0,G_BUFSIZE);
	SetReturnDrives(myHidListTest130);
	ListDrives("Lock");
	//"{"jcElockSerial": "a"}"����������޶�json��22�ֽڳ���
	EXPECT_GT(strlen(s_devList),22);	
}

TEST_F(ATMCDLLSelfTest, jcHidDevEnumBad1)
{
	//ö���豸����������������
	memset(s_devList,0,G_BUFSIZE);
	SetReturnDrives(myHidListTest130);
	ListDrives("Lock1");
	//"{"jcElockSerial": "a"}"����������޶�json��22�ֽڳ���
	EXPECT_EQ(strlen(s_devList),0);	
}

TEST_F(ATMCDLLSelfTest, jcHidDevEnumBad2)
{
	//ö���豸���޻ص������������
	memset(s_devList,0,G_BUFSIZE);
	SetReturnDrives(NULL);
	ListDrives("Lock");
	//"{"jcElockSerial": "a"}"����������޶�json��22�ֽڳ���
	EXPECT_EQ(strlen(s_devList),0);	
}

TEST_F(ATMCDLLSelfTest, jcHidDevEnumBad3)
{
	//ö���豸������������޻ص������������
	memset(s_devList,0,G_BUFSIZE);
	SetReturnDrives(NULL);
	ListDrives("Lock1");
	//"{"jcElockSerial": "a"}"����������޶�json��22�ֽڳ���
	EXPECT_EQ(strlen(s_devList),0);	
}
//////////////////////////////////////////////////////////////////////////

TEST_F(ATMCDLLSelfTest, zjydbgNormal)
{
	const char *devSN3="OQAiAACAAoQL1wAI";
	const char *hidType="Lock";
	OpenDrives(hidType,	devSN3);
	SetReturnMessage(myReturnMessageTest130);
	InputMessage(hidType,devSN3,cmdBuf);
	Sleep(5000);
	CloseDrives(hidType,devSN3);
}

}	//namespace zwHidGTest20150130{
#endif // ZWUSEGTEST