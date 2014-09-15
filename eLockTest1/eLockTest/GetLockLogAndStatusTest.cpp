#include "stdafx.h"
#include "jcElockTestHdr.h"
#include "CCBelock.h"

#ifdef _DEBUG_QUERY_LOCK_STATUS
TEST_F(ccbElockTest, QueryLockStatusTest0002)
{
	EXPECT_EQ(ELOCK_ERROR_SUCCESS, SetRecvMsgRotine(myATMCRecvMsgRotine));
	const JC_MSG_TYPE msgType = JCMSG_QUERY_LOCK_STATUS;	//设定消息类型
	//ATMC生成XML消息
	string strSendLockActInfoXML;	//容纳生成的消息XML
	//具体生成消息XML
	ptree pt;
	jcAtmcMsg::zwAtmcTestMsgGen(msgType, strSendLockActInfoXML);

	if (ELOCK_ERROR_SUCCESS == m_connStatus) {
		EXPECT_EQ(ELOCK_ERROR_SUCCESS,
			  Notify(strSendLockActInfoXML.c_str()));
	} else {
		EXPECT_EQ(ELOCK_ERROR_CONNECTLOST,
			  Notify(strSendLockActInfoXML.c_str()));
		cout << "Server not Start!" << endl;
	}
	Sleep(1 * 1000);
}
#endif // _DEBUG_QUERY_LOCK_STATUS

#ifdef _DEBUG_GET_LOCK_LOG
TEST_F(ccbElockTest, GetLockLogTest0005)
{
	EXPECT_EQ(ELOCK_ERROR_SUCCESS, SetRecvMsgRotine(myATMCRecvMsgRotine));
	const JC_MSG_TYPE msgType = JCMSG_GET_LOCK_LOG;	//设定消息类型
	//ATMC生成XML消息
	string strSendLockActInfoXML;	//容纳生成的消息XML
	//具体生成消息XML
	ptree pt;
	jcAtmcMsg::zwAtmcTestMsgGen(msgType, strSendLockActInfoXML);
	for (int i=0;i<9;i++)
	{
		if (ELOCK_ERROR_SUCCESS == m_connStatus) {
			EXPECT_EQ(ELOCK_ERROR_SUCCESS,
				Notify(strSendLockActInfoXML.c_str()));
			Sleep(1000);
			EXPECT_EQ(ELOCK_ERROR_SUCCESS,
				Notify(strSendLockActInfoXML.c_str()));
		} else {
			EXPECT_EQ(ELOCK_ERROR_CONNECTLOST,
				Notify(strSendLockActInfoXML.c_str()));
			cout << "Server not Start!" << endl;
		}
		Sleep(2 * 1000);
	}
}
#endif // _DEBUG_GET_LOCK_LOG
