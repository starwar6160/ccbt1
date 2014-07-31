// eLockTest1.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "CCBelock.h"

//�����Ƿ�����������Ա�רһ����һ����
#define _ZWTEST730
const char *myLongMsg="0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
	"0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
	"0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF";

namespace jcAtmcMsg{
void zwAtmcMsgGen(const JC_MSG_TYPE type,string &strXML);
}




//�����Իص�����
void myATMCRecvMsgRotine(const char *pszMsg)
{
	cout<<"*******************EMU SRV RETURN XML	Start*****************\n";
	cout<<pszMsg<<endl;
	cout<<"*******************EMU SRV RETURN XML	End*******************\n";
}

//�����׼���ʼ���ͽ����¼�
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
//SetRecvMsgRotine����
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
{//Debug�»ᴥ�����ԵķǷ�������ԣ��ŵ����������н���
	EXPECT_DEBUG_DEATH(SetRecvMsgRotine(NULL),"");
}

TEST(ccbElockDeathTest,OpenTestBad)
{//Debug�»ᴥ�����ԵķǷ�������ԣ��ŵ����������н���
	EXPECT_DEBUG_DEATH(Open(0),"");
	EXPECT_DEBUG_DEATH(Open(-30),"");
	EXPECT_DEBUG_DEATH(Open(4000),"");
}

//Notify����
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
{//Debug�»ᴥ�����ԵķǷ�������ԣ��ŵ����������н���
	//EXPECT_DEBUG_DEATH(Notify(myLongMsg),"");
	EXPECT_DEBUG_DEATH(Notify(""),"");
	EXPECT_DEBUG_DEATH(Notify(NULL),"");
}
#endif // _ZWTEST730


TEST_F(ccbElockTest,XMLTestLockActive)
{
	const JC_MSG_TYPE msgType=JCMSG_LOCK_ACTIVE_REQUEST;	//�趨��Ϣ����
	string strLockActiveXML;	//�������ɵ���ϢXML
	//����������ϢXML
	jcAtmcMsg::zwAtmcMsgGen(msgType,strLockActiveXML);	
	EXPECT_LT(42,strLockActiveXML.length());	//�������ɵ�XML����42�ֽ�����
	string strLockActiveJson;	//����XMLת���ɵ�JSON
	//����XML��JSON�е���Ϣ�����ֶ�������ָ������Ϣ����
	JC_MSG_TYPE rType=zwXML2Json(strLockActiveXML,strLockActiveJson);
	EXPECT_EQ(msgType,rType);
	EXPECT_LT(9,strLockActiveJson.length());	//����ת��������JSON����9�ֽ�����
}



int _tmain(int argc, _TCHAR* argv[])
{	
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

