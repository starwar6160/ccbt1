// eLockTest1.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "jcElockTestHdr.h"
#include "CCBelock.h"






#ifdef _DEBUG_QUERY_LOCK_STATUS
TEST_F(ccbElockTest,QueryLockStatusTest0002)
{
	EXPECT_EQ(ELOCK_ERROR_SUCCESS,SetRecvMsgRotine(myATMCRecvMsgRotine));	
	const JC_MSG_TYPE msgType=JCMSG_QUERY_LOCK_STATUS;	//�趨��Ϣ����
	//ATMC����XML��Ϣ
	string strSendLockActInfoXML;	//�������ɵ���ϢXML
	//����������ϢXML
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
	const JC_MSG_TYPE msgType=JCMSG_TIME_SYNC;	//�趨��Ϣ����
	//ATMC����XML��Ϣ
	string strSendLockActInfoXML;	//�������ɵ���ϢXML
	//����������ϢXML
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
	const JC_MSG_TYPE msgType=JCMSG_GET_LOCK_LOG;	//�趨��Ϣ����
	//ATMC����XML��Ϣ
	string strSendLockActInfoXML;	//�������ɵ���ϢXML
	//����������ϢXML
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
//���������������͵ĳ�ʼ���������
TEST_F(ccbElockTest,RecvInitClose1000)
{
	EXPECT_EQ(ELOCK_ERROR_SUCCESS,SetRecvMsgRotine(myATMCRecvMsgRotine));	
	const JC_MSG_TYPE msgType=JCMSG_SEND_INITCLOSECODE;	//�趨��Ϣ����
	//ATMC����XML��Ϣ
	string strLockActiveXML;	//�������ɵ���ϢXML
	//����������ϢXML
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
//���������������͵���֤�����
TEST_F(ccbElockTest,RecvVerifyClose1002)
{
	EXPECT_EQ(ELOCK_ERROR_SUCCESS,SetRecvMsgRotine(myATMCRecvMsgRotine));	
	const JC_MSG_TYPE msgType=JCMSG_SEND_UNLOCK_CERTCODE;	//�趨��Ϣ����
	//ATMC����XML��Ϣ
	string strLockActiveXML;	//�������ɵ���ϢXML
	//����������ϢXML
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

