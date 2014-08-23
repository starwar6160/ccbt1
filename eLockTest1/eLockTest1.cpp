// eLockTest1.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "jcElockTestHdr.h"
#include "CCBelock.h"




#ifdef _DEBUG_ACTREQ
//锁具激活请求报文的在线测试
TEST_F(ccbElockTest,LockActiveTest0000)
{
	printf("%d HEX=%s\n",67386086,zwCode8ToHex(67386086).c_str());
	printf("%s HEX=%016X\n","67386086","67386086");

	//Open(25);
	EXPECT_EQ(ELOCK_ERROR_SUCCESS,SetRecvMsgRotine(myATMCRecvMsgRotine));	
	const JC_MSG_TYPE msgType=JCMSG_LOCK_ACTIVE_REQUEST;	//设定消息类型
	//ATMC生成XML消息
	string strLockActiveXML;	//容纳生成的消息XML
	//具体生成消息XML
	ptree pt;
	jcAtmcMsg::zwAtmcTestMsgGen(msgType,strLockActiveXML, pt);	
	//strLockActiveXML=
	//"<?xml version=\"1.0\" encoding=\"utf-8\"?>"
	//"<root><TransCode>0000</TransCode>"
	//"<TransName>CallForActInfo</TransName>"
	//"<TransDate>20140807</TransDate>"
	//"<TransTime>153235</TransTime>"
	//"<DevCode>12345698</DevCode>"
	//"<SpareString1>NULL</SpareString1>"
	//"<SpareString2>NULL</SpareString2></root>";
	//"<?xml version=\"1.0\" encoding=\"utf-8\"?><root><TransCode>0000</TransCode><TransName>CallForActInfo</TransName><TransDate>20140807</TransDate><TransTime>173828</TransTime><DevCode>12345698Z</DevCode><SpareString1>NULL</SpareString1><SpareString2>NULL</SpareString2></root>";
	//下面一行是一整行原生XML，方便网页测试使用，没有转义字符
	// <?xml version="1.0" encoding="utf-8"?><root><TransCode>0000</TransCode><TransName>CallForActInfo</TransName><TransDate>20140807</TransDate><TransTime>173828</TransTime><DevCode>12345698Z</DevCode><SpareString1>NULL</SpareString1><SpareString2>NULL</SpareString2></root>
	//cout<<"Sleep 65 seconds,please wait"<<endl;

	if (ELOCK_ERROR_SUCCESS==m_connStatus)
	{
		EXPECT_EQ(ELOCK_ERROR_SUCCESS,Notify(strLockActiveXML.c_str()));		
	}
	else
	{
		EXPECT_EQ(ELOCK_ERROR_CONNECTLOST,Notify(strLockActiveXML.c_str()));		
		cout<<"Server not Start!"<<endl;
	}

	while (s_repActReqXML.length()==0)
	{
		Sleep(200);
	}

	Sleep(ZW_END_WAIT);
	EXPECT_LT(42,s_repActReqXML.length());

	string ccbop,ccbname;
	zwGetCCBMsgType(s_repActReqXML,ccbop,ccbname);
	EXPECT_EQ("0000",ccbop);
	EXPECT_EQ("CallForActInfo",ccbname);
	//Sleep(6*1000);
}
#endif // _DEBUG_ACTREQ




#ifdef _DEBUG_SEND_ACTINFO
//锁具发送激活信息(PSK)报文(初始化)的在线测试
TEST_F(ccbElockTest,LockSendActInfoTest0001)
{
	//Open(25);
	//Sleep(2000);
	EXPECT_EQ(ELOCK_ERROR_SUCCESS,SetRecvMsgRotine(myATMCRecvMsgRotine));	
	const JC_MSG_TYPE msgType=JCMSG_SEND_LOCK_ACTIVEINFO;	//设定消息类型
	//ATMC生成XML消息
	string strSendLockActInfoXML;	//容纳生成的消息XML
	//具体生成消息XML
	ptree pt;
	jcAtmcMsg::zwAtmcTestMsgGen(msgType,strSendLockActInfoXML, pt);	

	//cout<<"Sleep 65 seconds,please wait"<<endl;
	//Sleep(5*1000);
	if (ELOCK_ERROR_SUCCESS==m_connStatus)
	{
		EXPECT_EQ(ELOCK_ERROR_SUCCESS,Notify(strSendLockActInfoXML.c_str()));		
	}
	else
	{
		EXPECT_EQ(ELOCK_ERROR_CONNECTLOST,Notify(strSendLockActInfoXML.c_str()));		
		cout<<"Server not Start!"<<endl;
	}

	while (s_repLockInitXML.length()==0)
	{
		Sleep(200);
	}
	//Sleep(ZW_END_WAIT);
	EXPECT_LT(42,s_repLockInitXML.length());
	string ccbop,ccbname;
	zwGetCCBMsgType(s_repLockInitXML,ccbop,ccbname);
	EXPECT_EQ("0001",ccbop);
	EXPECT_EQ("SendActInfo",ccbname);

}
#endif // _DEBUG_SEND_ACTINFO


#ifdef _DEBUG_QUERY_LOCK_STATUS
TEST_F(ccbElockTest,QueryLockStatusTest0002)
{
	EXPECT_EQ(ELOCK_ERROR_SUCCESS,SetRecvMsgRotine(myATMCRecvMsgRotine));	
	const JC_MSG_TYPE msgType=JCMSG_QUERY_LOCK_STATUS;	//设定消息类型
	//ATMC生成XML消息
	string strSendLockActInfoXML;	//容纳生成的消息XML
	//具体生成消息XML
	ptree pt;
	jcAtmcMsg::zwAtmcTestMsgGen(msgType,strSendLockActInfoXML, pt);	

	if (ELOCK_ERROR_SUCCESS==m_connStatus)
	{
		EXPECT_EQ(ELOCK_ERROR_SUCCESS,Notify(strSendLockActInfoXML.c_str()));	
	}
	else
	{
		EXPECT_EQ(ELOCK_ERROR_CONNECTLOST,Notify(strSendLockActInfoXML.c_str()));		
		cout<<"Server not Start!"<<endl;
	}
	Sleep(1*1000);
}
#endif // _DEBUG_QUERY_LOCK_STATUS

#ifdef _DEBUG_TIMESYNC
TEST_F(ccbElockTest,TimeSyncTest0003)
{
	EXPECT_EQ(ELOCK_ERROR_SUCCESS,SetRecvMsgRotine(myATMCRecvMsgRotine));	
	const JC_MSG_TYPE msgType=JCMSG_TIME_SYNC;	//设定消息类型
	//ATMC生成XML消息
	string strSendLockActInfoXML;	//容纳生成的消息XML
	//具体生成消息XML
	ptree pt;
	jcAtmcMsg::zwAtmcTestMsgGen(msgType,strSendLockActInfoXML, pt);	

	if (ELOCK_ERROR_SUCCESS==m_connStatus)
	{
		EXPECT_EQ(ELOCK_ERROR_SUCCESS,Notify(strSendLockActInfoXML.c_str()));	
		Sleep(90*1000);
		EXPECT_EQ(ELOCK_ERROR_SUCCESS,Notify(strSendLockActInfoXML.c_str()));	
	}
	else
	{
		EXPECT_EQ(ELOCK_ERROR_CONNECTLOST,Notify(strSendLockActInfoXML.c_str()));		
		cout<<"Server not Start!"<<endl;
	}
	Sleep(1*1000);
}
#endif // _DEBUG_TIMESYNC


#ifdef _DEBUG_GET_LOCK_LOG
TEST_F(ccbElockTest,GetLockLogTest0005)
{
	EXPECT_EQ(ELOCK_ERROR_SUCCESS,SetRecvMsgRotine(myATMCRecvMsgRotine));	
	const JC_MSG_TYPE msgType=JCMSG_GET_LOCK_LOG;	//设定消息类型
	//ATMC生成XML消息
	string strSendLockActInfoXML;	//容纳生成的消息XML
	//具体生成消息XML
	ptree pt;
	jcAtmcMsg::zwAtmcTestMsgGen(msgType,strSendLockActInfoXML, pt);	

	if (ELOCK_ERROR_SUCCESS==m_connStatus)
	{
		EXPECT_EQ(ELOCK_ERROR_SUCCESS,Notify(strSendLockActInfoXML.c_str()));	
		Sleep(20000);
		EXPECT_EQ(ELOCK_ERROR_SUCCESS,Notify(strSendLockActInfoXML.c_str()));	
	}
	else
	{
		EXPECT_EQ(ELOCK_ERROR_CONNECTLOST,Notify(strSendLockActInfoXML.c_str()));		
		cout<<"Server not Start!"<<endl;
	}
	Sleep(60*1000);
}
#endif // _DEBUG_GET_LOCK_LOG

#ifdef _DEBUG_RECV_INIT_CLOSECODE
//接收锁具主动发送的初始闭锁码测试
TEST_F(ccbElockTest,RecvInitClose1000)
{
	EXPECT_EQ(ELOCK_ERROR_SUCCESS,SetRecvMsgRotine(myATMCRecvMsgRotine));	
	const JC_MSG_TYPE msgType=JCMSG_SEND_INITCLOSECODE;	//设定消息类型
	//ATMC生成XML消息
	string strLockActiveXML;	//容纳生成的消息XML
	//具体生成消息XML
	ptree pt;
	jcAtmcMsg::zwAtmcTestMsgGen(msgType,strLockActiveXML, pt);	

	Notify(strLockActiveXML.c_str());

	while (s_repInitCloseCodeXML.length()==0)
	{
		Sleep(200);
	}
	//Sleep(ZW_END_WAIT);
	EXPECT_LT(42,s_repInitCloseCodeXML.length());
	string ccbop,ccbname;
	zwGetCCBMsgType(s_repInitCloseCodeXML,ccbop,ccbname);
	EXPECT_EQ("1000",ccbop);
	EXPECT_EQ("SendShutLockCode",ccbname);
}
#endif // _DEBUG_RECV_INIT_CLOSECODE

#ifdef _DEBUG_RECV_VERIFY_CODE
//接收锁具主动发送的验证码测试
TEST_F(ccbElockTest,RecvVerifyClose1002)
{
	EXPECT_EQ(ELOCK_ERROR_SUCCESS,SetRecvMsgRotine(myATMCRecvMsgRotine));	
	const JC_MSG_TYPE msgType=JCMSG_SEND_UNLOCK_CERTCODE;	//设定消息类型
	//ATMC生成XML消息
	string strLockActiveXML;	//容纳生成的消息XML
	//具体生成消息XML
	ptree pt;
	jcAtmcMsg::zwAtmcTestMsgGen(msgType,strLockActiveXML, pt);	

	Notify(strLockActiveXML.c_str());

	while (s_repVerifyCodeXML.length()==0)
	{
		Sleep(200);
	}
	//Sleep(ZW_END_WAIT);
	EXPECT_LT(42,s_repVerifyCodeXML.length());
	string ccbop,ccbname;
	zwGetCCBMsgType(s_repVerifyCodeXML,ccbop,ccbname);
	EXPECT_EQ("1002",ccbop);
	EXPECT_EQ("SendUnLockIdent",ccbname);
}
#endif // _DEBUG_RECV_VERIFY_CODE




int _tmain(int argc, _TCHAR* argv[])
{	
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

