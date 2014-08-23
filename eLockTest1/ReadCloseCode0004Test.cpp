#include "stdafx.h"
#include "jcElockTestHdr.h"
#include "CCBelock.h"

#ifdef _DEBUG_READ_CLOSE_CODE
//读取闭锁码报文的在线测试
TEST_F(ccbElockTest,ReadCloseCodeTest0004)
{
	//Open(25);
	EXPECT_EQ(ELOCK_ERROR_SUCCESS,SetRecvMsgRotine(myATMCRecvMsgRotine));	
	const JC_MSG_TYPE msgType=JCMSG_GET_CLOSECODE;	//设定消息类型
	//ATMC生成XML消息
	string strSendLockActInfoXML;	//容纳生成的消息XML
	//具体生成消息XML
	ptree pt;
	jcAtmcMsg::zwAtmcTestMsgGen(msgType,strSendLockActInfoXML, pt);	

	if (ELOCK_ERROR_SUCCESS==m_connStatus)
	{
		EXPECT_EQ(ELOCK_ERROR_SUCCESS,Notify(strSendLockActInfoXML.c_str()));
		Sleep(1000);
		EXPECT_EQ(ELOCK_ERROR_SUCCESS,Notify(strSendLockActInfoXML.c_str()));
	}
	else
	{
		EXPECT_EQ(ELOCK_ERROR_CONNECTLOST,Notify(strSendLockActInfoXML.c_str()));		
		cout<<"Server not Start!"<<endl;
	}
	while (s_repReadCloseCodeXML.length()==0)
	{
		Sleep(200);
	}
	//Sleep(ZW_END_WAIT);
	EXPECT_LT(42,s_repReadCloseCodeXML.length());

	string ccbop,ccbname;
	zwGetCCBMsgType(s_repReadCloseCodeXML,ccbop,ccbname);
	EXPECT_EQ("0004",ccbop);
	EXPECT_EQ("ReadShutLockCode",ccbname);
	Sleep(2*1000);
}
#endif // _DEBUG_READ_CLOSE_CODE

