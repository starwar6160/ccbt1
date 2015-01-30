#include "stdafx.h"
#include <gtest/gtest.h>
#include "zwHidGtest130.h"
#define ZWUSEGTEST

#ifdef ZWUSEGTEST
//测试套件初始化和结束事件
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

TEST_F(ATMCDLLSelfTest, LockGetHashTest)
{
	//测试从外部字符串ID计算来的设备HASH是否正确
	int age=1;
	EXPECT_EQ(1,age);
	jch::zwJcHidDbg15A hid1;
	//此处计算的是标准锁具的HASH
	hid1.SetElock(NULL);
	uint32_t hidHash=hid1.GetHash();
	cout<<"hidHash="<<hidHash<<endl;
	EXPECT_GT(hidHash,0);
	EXPECT_EQ(hidHash,2378562802);
}
#endif // ZWUSEGTEST