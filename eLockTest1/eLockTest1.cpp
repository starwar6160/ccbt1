// eLockTest1.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "CCBelock.h"
const char *myLongMsg="0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
	"0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
	"0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF";

//�����Իص�����
void myATMCRecvMsgRotine(const char *pszMsg)
{
	cout<<__FUNCTION__<<"["<<pszMsg<<"]"<<endl;
}

//�����׼���ʼ���ͽ����¼�
class ccbElockTest : public testing::Test
{

protected:
	virtual void SetUp() {
		cout<<__FUNCTION__<<endl;
		Open(25);
	}
	virtual void TearDown() {
		cout<<__FUNCTION__<<endl;
		Close();
	}
};


//Open,Close����
TEST_F(ccbElockTest,OpenCloseTest)
{
	EXPECT_EQ(ELOCK_ERROR_SUCCESS,Open(25));
	EXPECT_EQ(ELOCK_ERROR_SUCCESS,Close());
#ifdef NDEBUG
	EXPECT_EQ(ELOCK_ERROR_PARAMINVALID,Open(0));
	EXPECT_EQ(ELOCK_ERROR_PARAMINVALID,Open(-30));
	EXPECT_EQ(ELOCK_ERROR_PARAMINVALID,Open(4000));
#endif // NDEBUG
}

TEST(ccbElockDeathTest,OpenTestBad)
{//Debug�»ᴥ�����ԵķǷ�������ԣ��ŵ����������н���
	EXPECT_DEBUG_DEATH(Open(0),"");
	EXPECT_DEBUG_DEATH(Open(-30),"");
	EXPECT_DEBUG_DEATH(Open(4000),"");
}

//Notify����
TEST_F(ccbElockTest,NotifyTest)
{
	EXPECT_EQ(ELOCK_ERROR_SUCCESS,Notify("mytestNotify"));	
	
#ifdef NDEBUG
	EXPECT_EQ(ELOCK_ERROR_PARAMINVALID,Notify(NULL));
	EXPECT_EQ(ELOCK_ERROR_PARAMINVALID,Notify(""));
	EXPECT_EQ(ELOCK_ERROR_PARAMINVALID,Notify(myLongMsg));
#endif // NDEBUG
}

TEST(ccbElockDeathTest,NotifyTestBad)
{//Debug�»ᴥ�����ԵķǷ�������ԣ��ŵ����������н���
	EXPECT_DEBUG_DEATH(Notify(myLongMsg),"");
	EXPECT_DEBUG_DEATH(Notify(""),"");
	EXPECT_DEBUG_DEATH(Notify(NULL),"");
}

//SetRecvMsgRotine����
TEST_F(ccbElockTest,SetRecvMsgRotineTest)
{
	EXPECT_EQ(ELOCK_ERROR_SUCCESS,SetRecvMsgRotine(myATMCRecvMsgRotine));	
	Notify("test20140725.1517forCallBack");
#ifdef NDEBUG
	EXPECT_EQ(ELOCK_ERROR_PARAMINVALID,SetRecvMsgRotine(NULL));
#endif // NDEBUG
}

TEST(ccbElockDeathTest,SetRecvMsgRotineTestBad)
{//Debug�»ᴥ�����ԵķǷ�������ԣ��ŵ����������н���
	EXPECT_DEBUG_DEATH(SetRecvMsgRotine(NULL),"");
}



int _tmain(int argc, _TCHAR* argv[])
{	
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}