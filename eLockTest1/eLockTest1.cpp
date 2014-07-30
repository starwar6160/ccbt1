// eLockTest1.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
using namespace boost::property_tree;
#include "CCBelock.h"
const char *myLongMsg="0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
	"0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
	"0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF";

//测试性回调函数
void myATMCRecvMsgRotine(const char *pszMsg)
{
	cout<<__FUNCTION__<<"["<<pszMsg<<"]"<<endl;
}

//测试套件初始化和结束事件
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



//SetRecvMsgRotine测试
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
{//Debug下会触发断言的非法输入测试，放到死亡测试中进行
	EXPECT_DEBUG_DEATH(SetRecvMsgRotine(NULL),"");
}


//Open,Close测试
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
{//Debug下会触发断言的非法输入测试，放到死亡测试中进行
	EXPECT_DEBUG_DEATH(Open(0),"");
	EXPECT_DEBUG_DEATH(Open(-30),"");
	EXPECT_DEBUG_DEATH(Open(4000),"");
}

//Notify测试
TEST_F(ccbElockTest,NotifyTest)
{
	if (ELOCK_ERROR_SUCCESS==m_connStatus)
	{
	EXPECT_EQ(ELOCK_ERROR_SUCCESS,Notify("mytestNotify1014conn"));	
	}
	else
	{
		EXPECT_EQ(ELOCK_ERROR_CONNECTLOST,Notify("mytestNotify1014notConn"));	
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
{//Debug下会触发断言的非法输入测试，放到死亡测试中进行
	//EXPECT_DEBUG_DEATH(Notify(myLongMsg),"");
	EXPECT_DEBUG_DEATH(Notify(""),"");
	EXPECT_DEBUG_DEATH(Notify(NULL),"");
}

TEST_F(ccbElockTest,XMLTest730)
{
	ptree pt;
	pt.put("app.version", 101);
	pt.put("app.theme", "blue");
	pt.put("app.about.url", "http://www.jinchu.com.cn");
	pt.put("app.about.email", "jinchu_zhouwei@126.com");
	pt.put("app.about.content", "coryright (C) jinchu.com 2001-2014");
	std::ostringstream demoxml;
	write_xml(demoxml,pt);
	//zwTestXML730(demoxml.str().c_str());
	SetRecvMsgRotine(myATMCRecvMsgRotine);
	Notify(demoxml.str().c_str());
}



int _tmain(int argc, _TCHAR* argv[])
{	
	//testing::AddGlobalTestEnvironment(new ccbElockEnvironment);
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

