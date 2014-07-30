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
public:
	int m_connStatus;
protected:
	virtual void SetUp() {
		//shared_resource_ = new ;
		//memset(s_priKey,0,sizeof(s_priKey));
		cout<<__FUNCTION__<<endl;
		m_connStatus=ELOCK_ERROR_SUCCESS;
		m_connStatus=Open(25);
	}
	virtual void TearDown() {
		cout<<__FUNCTION__<<endl;
		Close();
	}
};



//SetRecvMsgRotine����
TEST_F(ccbElockTest,SetRecvMsgRotineTest)
{
	//Open(25);
	EXPECT_EQ(ELOCK_ERROR_SUCCESS,SetRecvMsgRotine(myATMCRecvMsgRotine));	
	if (ELOCK_ERROR_SUCCESS==m_connStatus)
	{
		EXPECT_EQ(ELOCK_ERROR_SUCCESS,Notify("test20140725.1517forCallBack"));		
	}
	else
	{
		EXPECT_EQ(ELOCK_ERROR_CONNECTLOST,Notify("test20140725.1517forCallBack"));		
		cout<<"Server not Start!"<<endl;
	}
	
#ifdef NDEBUG
	EXPECT_EQ(ELOCK_ERROR_PARAMINVALID,SetRecvMsgRotine(NULL));
#endif // NDEBUG
	//Close();
}

TEST(ccbElockDeathTest,SetRecvMsgRotineTestBad)
{//Debug�»ᴥ�����ԵķǷ�������ԣ��ŵ����������н���
	EXPECT_DEBUG_DEATH(SetRecvMsgRotine(NULL),"");
}


//Open,Close����
TEST_F(ccbElockTest,OpenCloseTest)
{
	if (ELOCK_ERROR_SUCCESS==m_connStatus)
	{
	EXPECT_EQ(ELOCK_ERROR_SUCCESS,Open(25));
	EXPECT_EQ(ELOCK_ERROR_SUCCESS,Close());
	}
	else
	{
		EXPECT_EQ(ELOCK_ERROR_CONNECTLOST,Open(25));
		EXPECT_EQ(ELOCK_ERROR_SUCCESS,Close());
		cout<<"Server not Start!"<<endl;
	}
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
	if (ELOCK_ERROR_SUCCESS==m_connStatus)
	{
	EXPECT_EQ(ELOCK_ERROR_SUCCESS,Notify("mytestNotify"));	
	}
	else
	{
		EXPECT_EQ(ELOCK_ERROR_CONNECTLOST,Notify("mytestNotify"));	
		cout<<"Server not Start!"<<endl;
	}
	
#ifdef NDEBUG
	EXPECT_EQ(ELOCK_ERROR_PARAMINVALID,Notify(NULL));
	EXPECT_EQ(ELOCK_ERROR_PARAMINVALID,Notify(""));
	EXPECT_EQ(ELOCK_ERROR_PARAMINVALID,Notify(myLongMsg));
#endif // NDEBUG
	//Close();
}

TEST(ccbElockDeathTest,NotifyTestBad)
{//Debug�»ᴥ�����ԵķǷ�������ԣ��ŵ����������н���
	EXPECT_DEBUG_DEATH(Notify(myLongMsg),"");
	EXPECT_DEBUG_DEATH(Notify(""),"");
	EXPECT_DEBUG_DEATH(Notify(NULL),"");
}


int _tmain(int argc, _TCHAR* argv[])
{	
	//testing::AddGlobalTestEnvironment(new ccbElockEnvironment);
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

