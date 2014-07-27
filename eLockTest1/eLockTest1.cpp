// eLockTest1.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "CCBelock.h"
const char *myLongMsg="0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
	"0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
	"0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF";

//测试性回调函数
void myATMCRecvMsgRotine(const char *pszMsg)
{
	cout<<__FUNCTION__<<"["<<pszMsg<<"]"<<endl;
}

//Open,Close测试
TEST(ccbElockTest,OpenCloseTest)
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
{//Debug下会触发断言的非法输入测试，放到死亡测试中进行
	EXPECT_DEBUG_DEATH(Open(0),"");
	EXPECT_DEBUG_DEATH(Open(-30),"");
	EXPECT_DEBUG_DEATH(Open(4000),"");
}

//Notify测试
TEST(ccbElockTest,NotifyTest)
{
	//zwThrTest1(33);
	EXPECT_EQ(ELOCK_ERROR_SUCCESS,Notify("mytestNotify"));	
#ifdef NDEBUG
	EXPECT_EQ(ELOCK_ERROR_PARAMINVALID,Notify(NULL));
	EXPECT_EQ(ELOCK_ERROR_PARAMINVALID,Notify(""));
	EXPECT_EQ(ELOCK_ERROR_PARAMINVALID,Notify(myLongMsg));
#endif // NDEBUG
}

TEST(ccbElockDeathTest,NotifyTestBad)
{//Debug下会触发断言的非法输入测试，放到死亡测试中进行
	EXPECT_DEBUG_DEATH(Notify(myLongMsg),"");
	EXPECT_DEBUG_DEATH(Notify(""),"");
	EXPECT_DEBUG_DEATH(Notify(NULL),"");
}

//SetRecvMsgRotine测试
TEST(ccbElockTest,SetRecvMsgRotineTest)
{
	EXPECT_EQ(ELOCK_ERROR_SUCCESS,SetRecvMsgRotine(myATMCRecvMsgRotine));	
	Notify("test20140725.1517forCallBack");
#ifdef NDEBUG
	EXPECT_EQ(ELOCK_ERROR_PARAMINVALID,SetRecvMsgRotine(NULL));
#endif // NDEBUG
}

TEST(ccbElockDeathTest,SetRecvMsgRotineTestBad)
{//Debug下会触发断言的非法输入测试，放到死亡测试中进行
	EXPECT_DEBUG_DEATH(SetRecvMsgRotine(NULL),"");
}


//程序全局初始化和结束事件
class ccbElockEnvironment : public testing::Environment
{
public:
	virtual void SetUp()
	{
		std::cout << "ccbElockEnv SetUP" << std::endl;		
	}
	virtual void TearDown()
	{
		std::cout << "ccbElockEnv TearDown" << std::endl;
		zwTestWebSocket();
	}
};

int _tmain(int argc, _TCHAR* argv[])
{	
	testing::AddGlobalTestEnvironment(new ccbElockEnvironment);
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}