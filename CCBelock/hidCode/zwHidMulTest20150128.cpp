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
//�����׼���ʼ���ͽ����¼�
class ATMCDLLSelfTest : public testing::Test
{
public:
	

protected:
	virtual void SetUp();
	virtual void TearDown();
};

void ATMCDLLSelfTest::SetUp()
{


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
	//���Դ��ⲿ�ַ���ID���������豸HASH�Ƿ���ȷ
	int age=1;
	EXPECT_EQ(1,age);
	jch::zwJcHidDbg15A hid1;
	//�˴�������Ǳ�׼���ߵ�HASH
	hid1.SetElock(NULL);
	uint32_t hidHash=hid1.GetHash();
	cout<<"hidHash="<<hidHash<<endl;
	EXPECT_GT(hidHash,0);
	EXPECT_EQ(hidHash,2378562802);
	hid1.OpenHidDevice();
	const char *msgT5a="{\"command\":\"Test_USB_HID\",\"cmd_id\":\"1234567890\",\"input\":\"TestAnyString";
	const char *msgT5b="\",\"output\":\"\"}";
	char buf[128];
	memset(buf,0,128);
	sprintf(buf,"%s%d%s",msgT5a,7,msgT5b);
	SetReturnMessage(myReturnMessageTest130);
	hid1.PushJson(buf);
	Sleep(2000);
}
#endif // ZWUSEGTEST