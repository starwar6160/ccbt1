// eLockTest1.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "CCBelock.h"
using namespace boost::property_tree;

const JC_MSG_TYPE lockParseJson( const string & inJson, ptree &pt );
void LockOutJson( const ptree &pt, string &outJson );


//看看是否打开其他测试以便专一测试一件事
#define _ZWTEST730
const char *myLongMsg="0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
	"0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
	"0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF";

namespace jcAtmcMsg{
	void zwAtmcMsgGen(const JC_MSG_TYPE type,string &strXML,ptree &pt);
}




//测试性回调函数
void myATMCRecvMsgRotine(const char *pszMsg)
{
	cout<<"*******************EMU SRV RETURN XML	Start*****************\n";
	cout<<pszMsg<<endl;
	cout<<"*******************EMU SRV RETURN XML	End*******************\n";
}

//测试套件初始化和结束事件
class ccbElockTest : public testing::Test
{
public:
	int m_connStatus;
protected:
	virtual void SetUp() {
		cout<<__FUNCTION__<<endl;
		m_connStatus=ELOCK_ERROR_SUCCESS;
		m_connStatus=Open(25);
	}
	virtual void TearDown() {
		cout<<__FUNCTION__<<endl;
		Close();
	}
};



#ifdef _ZWTEST730
//SetRecvMsgRotine测试
TEST_F(ccbElockTest,SetRecvMsgRotineTestInvalidXML)
{
	//Open(25);
	EXPECT_EQ(ELOCK_ERROR_SUCCESS,SetRecvMsgRotine(myATMCRecvMsgRotine));	
	if (ELOCK_ERROR_SUCCESS==m_connStatus)
	{
		EXPECT_EQ(ELOCK_ERROR_CONNECTLOST,Notify("test20140730.1558forCallBackTESTFORCHAR42AND"));		
	}
	else
	{
		EXPECT_EQ(ELOCK_ERROR_CONNECTLOST,Notify("test20140730.1558forCallBackTESTFORCHAR42ANDNoConnforCallBack"));		
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

TEST(ccbElockDeathTest,OpenTestBad)
{//Debug下会触发断言的非法输入测试，放到死亡测试中进行
	EXPECT_DEBUG_DEATH(Open(0),"");
	EXPECT_DEBUG_DEATH(Open(-30),"");
	EXPECT_DEBUG_DEATH(Open(4000),"");
}

//Notify测试
TEST_F(ccbElockTest,NotifyTestInvalidXML)
{
	if (ELOCK_ERROR_SUCCESS==m_connStatus)
	{
	EXPECT_EQ(ELOCK_ERROR_CONNECTLOST,Notify("mytestNotify1014connmytestNotify1014connmytestNotify1014conn"));	
	}
	else
	{
		EXPECT_EQ(ELOCK_ERROR_CONNECTLOST,Notify("mytestNotify1014notConnmytestNotify1014connmytestNotify1014conn"));	
		cout<<"Server not Start!"<<endl;
	}
	
#ifdef NDEBUG
	EXPECT_EQ(ELOCK_ERROR_PARAMINVALID,Notify(NULL));
	EXPECT_EQ(ELOCK_ERROR_PARAMINVALID,Notify(""));
	EXPECT_EQ(ELOCK_ERROR_CONNECTLOST,Notify(myLongMsg));
#endif // NDEBUG
	//Close();
}

TEST(ccbElockDeathTest,NotifyTestBad)
{//Debug下会触发断言的非法输入测试，放到死亡测试中进行
	//EXPECT_DEBUG_DEATH(Notify(myLongMsg),"");
	EXPECT_DEBUG_DEATH(Notify(""),"");
	EXPECT_DEBUG_DEATH(Notify(NULL),"");
}
#endif // _ZWTEST730

//锁具激活报文的在线测试
TEST_F(ccbElockTest,LockActiveTestOnline)
{
	//Open(25);
	EXPECT_EQ(ELOCK_ERROR_SUCCESS,SetRecvMsgRotine(myATMCRecvMsgRotine));	
	const JC_MSG_TYPE msgType=JCMSG_LOCK_ACTIVE_REQUEST;	//设定消息类型
	//ATMC生成XML消息
	string strLockActiveXML;	//容纳生成的消息XML
	//具体生成消息XML
	ptree pt;
	jcAtmcMsg::zwAtmcMsgGen(msgType,strLockActiveXML, pt);	

	if (ELOCK_ERROR_SUCCESS==m_connStatus)
	{
		EXPECT_EQ(ELOCK_ERROR_SUCCESS,Notify(strLockActiveXML.c_str()));		
	}
	else
	{
		EXPECT_EQ(ELOCK_ERROR_CONNECTLOST,Notify(strLockActiveXML.c_str()));		
		cout<<"Server not Start!"<<endl;
	}

#ifdef NDEBUG
	EXPECT_EQ(ELOCK_ERROR_PARAMINVALID,SetRecvMsgRotine(NULL));
#endif // NDEBUG
	//Close();
}


//锁具激活报文的单元测试
TEST_F(ccbElockTest,LockActiveTestUnit)
{
	const JC_MSG_TYPE msgType=JCMSG_LOCK_ACTIVE_REQUEST;	//设定消息类型
	//////////////////////////////////////////////////////////////////////////
	//ATMC生成XML消息
	string strLockActiveXML;	//容纳生成的消息XML
	//具体生成消息XML
	ptree pt;
	jcAtmcMsg::zwAtmcMsgGen(msgType,strLockActiveXML, pt);	
	EXPECT_LT(42,strLockActiveXML.length());	//期望生成的XML至少42字节以上
	string strLockActiveJson;	//容纳XML转换成的JSON
	//////////////////////////////////////////////////////////////////////////
	//DLL把XML转换为JSON
	//期望XML和JSON中的消息类型字段是我们指定的消息类型
	JC_MSG_TYPE rType=zwXML2Json(strLockActiveXML,strLockActiveJson);
	EXPECT_EQ(msgType,rType);
	EXPECT_LT(9,strLockActiveJson.length());	//期望转换出来的JSON至少9字节以上
	//////////////////////////////////////////////////////////////////////////
	//锁具内部处理JSON
	JC_MSG_TYPE mtypeL=lockParseJson(strLockActiveJson, pt);
	EXPECT_EQ(msgType,mtypeL);

}


int _tmain(int argc, _TCHAR* argv[])
{	
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

