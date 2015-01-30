#include "stdafx.h"
#include <gtest/gtest.h>
#include "zwHidGtest130.h"
#define ZWUSEGTEST

void ZJY1501STD myReturnMessageTest130(const char* DrivesIdSN,char* DrivesMessageJson)
{
	printf("Callback Function %s:\trecvDataLen=%d\t",__FUNCTION__,strlen(DrivesMessageJson));
	printf("devSerial=%s\t devReturnJson is:\n%s\n",DrivesIdSN,DrivesMessageJson);
}


#ifdef ZWUSEGTEST
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

CCBELOCK_API int zwStartGtestInDLL(void)
{
	int argc=1;
	char *argv[1];
	argv[0]=NULL;
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
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
	hid1.PushJson(cmdBuf);
	Sleep(2000);
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
	EXPECT_EQ(jch::G_FAIL,hdv.PushJson(NULL));
	EXPECT_EQ(jch::G_FAIL,hdv.PushJson(""));
	Sleep(700);
}

TEST_F(ATMCDLLSelfTest, LockNotSetPara)
{
	//测试没有设置参数就执行OpenHidDevice的动作
	jch::zwJcHidDbg15A hdv;
	hdv.OpenHidDevice();
	EXPECT_EQ(jch::G_FAIL,hdv.PushJson(cmdBuf));
	Sleep(700);
}

#endif // ZWUSEGTEST