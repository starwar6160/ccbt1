#include "stdafx.h"
#include <gtest/gtest.h>
#include "zwHidGtest130.h"
#define ZWUSEGTEST

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

TEST_F(ATMCDLLSelfTest, LockGetHashTest)
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
}
#endif // ZWUSEGTEST