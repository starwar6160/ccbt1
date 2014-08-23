#include "stdafx.h"
#include "jcElockTestHdr.h"
#include "CCBelock.h"

#ifdef _ZWTEST730

const char *myLongMsg="0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
	"0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
	"0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF";

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
	EXPECT_DEBUG_DEATH(Open(90000),"");
}


TEST(ccbElockDeathTest,NotifyTestBad)
{//Debug下会触发断言的非法输入测试，放到死亡测试中进行
	//EXPECT_DEBUG_DEATH(Notify(myLongMsg),"");
	EXPECT_DEBUG_DEATH(Notify(""),"");
	EXPECT_DEBUG_DEATH(Notify(NULL),"");
}

#endif // _ZWTEST730

