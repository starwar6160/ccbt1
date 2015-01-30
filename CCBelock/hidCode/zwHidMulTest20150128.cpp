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


//测试套件初始化和结束事件
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
	//测试从外部字符串ID计算来的设备HASH是否正确
	jch::zwJcHidDbg15A hid1;
	//此处计算的是标准锁具的HASH
	hid1.SetElock(NULL);
	uint32_t hidHash=hid1.GetHash();
	cout<<"hidHash="<<hidHash<<endl;
	EXPECT_GT(hidHash,0);
	EXPECT_EQ(hidHash,2378562802);
	hid1.OpenHidDevice();
	//清理s_recvMsg，期待收到的长度大于0
	memset(s_recvMsg,0,G_BUFSIZE);
	hid1.PushJson(cmdBuf);
	Sleep(2000);
	EXPECT_GT(strlen(s_recvMsg),0);
}

TEST_F(ATMCDLLSelfTest, LockMultiOpen)
{
	//测试2个类对象打开同一个设备的情况
	jch::zwJcHidDbg15A hid1,hid2;
	//此处计算的是标准锁具的HASH
	hid1.SetElock(NULL);
	hid2.SetElock(NULL);
	cout<<"will SUCCESS Open HID Device 1"<<endl;
	EXPECT_EQ(jch::G_SUSSESS,hid1.OpenHidDevice());
	cout<<"will FAIL Open same HID Device 1"<<endl;
	EXPECT_EQ(jch::G_FAIL,hid2.OpenHidDevice());

}

TEST_F(ATMCDLLSelfTest, LockNotOpen)
{
	//测试没有执行OpenHidDevice的动作
	jch::zwJcHidDbg15A hdv;
	hdv.SetElock(NULL);
	EXPECT_EQ(jch::G_FAIL,hdv.PushJson(cmdBuf));
	Sleep(700);
}

TEST_F(ATMCDLLSelfTest, LockPushNULL)
{
	//测试发送空的命令字符串的操作
	jch::zwJcHidDbg15A hdv;
	hdv.SetElock(NULL);
	hdv.OpenHidDevice();
	//清理s_recvMsg，期待收到的长度等于0，因为发送空命令字符串
	memset(s_recvMsg,0,G_BUFSIZE);
	EXPECT_EQ(jch::G_FAIL,hdv.PushJson(NULL));
	EXPECT_EQ(jch::G_FAIL,hdv.PushJson(""));
	Sleep(700);
	EXPECT_EQ(strlen(s_recvMsg),0);
}

TEST_F(ATMCDLLSelfTest, LockNotSetPara)
{
	//测试没有设置参数就执行OpenHidDevice的动作
	jch::zwJcHidDbg15A hdv;
	hdv.OpenHidDevice();
	EXPECT_EQ(jch::G_FAIL,hdv.PushJson(cmdBuf));
	Sleep(700);
}

//////////////////////////////////////////////////////////////////////////
TEST_F(ATMCDLLSelfTest, jcHidDevEnumNormal)
{
	//枚举设备，正常情况测试
	memset(s_devList,0,G_BUFSIZE);
	SetReturnDrives(myHidListTest130);
	ListDrives("Lock");
	//"{"jcElockSerial": "a"}"这样的最低限度json是22字节长度
	EXPECT_GT(strlen(s_devList),22);	
}

TEST_F(ATMCDLLSelfTest, jcHidDevEnumBad1)
{
	//枚举设备，输入错误情况测试
	memset(s_devList,0,G_BUFSIZE);
	SetReturnDrives(myHidListTest130);
	ListDrives("Lock1");
	//"{"jcElockSerial": "a"}"这样的最低限度json是22字节长度
	EXPECT_EQ(strlen(s_devList),0);	
}

TEST_F(ATMCDLLSelfTest, jcHidDevEnumBad2)
{
	//枚举设备，无回调函数情况测试
	memset(s_devList,0,G_BUFSIZE);
	SetReturnDrives(NULL);
	ListDrives("Lock");
	//"{"jcElockSerial": "a"}"这样的最低限度json是22字节长度
	EXPECT_EQ(strlen(s_devList),0);	
}

TEST_F(ATMCDLLSelfTest, jcHidDevEnumBad3)
{
	//枚举设备，输入错误并且无回调函数情况测试
	memset(s_devList,0,G_BUFSIZE);
	SetReturnDrives(NULL);
	ListDrives("Lock1");
	//"{"jcElockSerial": "a"}"这样的最低限度json是22字节长度
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