#include "stdafx.h"
#include "jcElockTestHdr.h"
#include "CCBelock.h"


#ifdef _DEBUG_ACTREQ

string zwCode8ToHex(int Code8);
string s_repActReqXML;
string s_repLockInitXML;

//���߼��������ĵ����߲���
TEST_F(ccbElockTest,LockActiveTest0000)
{
	printf("%d HEX=%s\n",67386086,zwCode8ToHex(67386086).c_str());
	printf("%s HEX=%016X\n","67386086","67386086");

	//Open(25);
	EXPECT_EQ(ELOCK_ERROR_SUCCESS,SetRecvMsgRotine(myATMCRecvMsgRotine));	
	const JC_MSG_TYPE msgType=JCMSG_LOCK_ACTIVE_REQUEST;	//�趨��Ϣ����
	//ATMC����XML��Ϣ
	string strLockActiveXML;	//�������ɵ���ϢXML
	//����������ϢXML
	ptree pt;
	jcAtmcMsg::zwAtmcTestMsgGen(msgType,strLockActiveXML);	
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
	//����һ����һ����ԭ��XML��������ҳ����ʹ�ã�û��ת���ַ�
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

	Sleep(500);
	EXPECT_LT(42,s_repActReqXML.length());

	string ccbop,ccbname;
	zwGetCCBMsgType(s_repActReqXML,ccbop,ccbname);
	EXPECT_EQ("0000",ccbop);
	EXPECT_EQ("CallForActInfo",ccbname);
	//Sleep(6*1000);
}
#endif // _DEBUG_ACTREQ

#ifdef _DEBUG_SEND_ACTINFO
//���߷��ͼ�����Ϣ(PSK)����(��ʼ��)�����߲���
TEST_F(ccbElockTest,LockSendActInfoTest0001)
{
	//Open(25);
	//Sleep(2000);
	EXPECT_EQ(ELOCK_ERROR_SUCCESS,SetRecvMsgRotine(myATMCRecvMsgRotine));	
	const JC_MSG_TYPE msgType=JCMSG_SEND_LOCK_ACTIVEINFO;	//�趨��Ϣ����
	//ATMC����XML��Ϣ
	string strSendLockActInfoXML;	//�������ɵ���ϢXML
	//����������ϢXML
	ptree pt;
	jcAtmcMsg::zwAtmcTestMsgGen(msgType,strSendLockActInfoXML);	

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
	EXPECT_LT(42,s_repLockInitXML.length());
	string ccbop,ccbname;
	zwGetCCBMsgType(s_repLockInitXML,ccbop,ccbname);
	EXPECT_EQ("0001",ccbop);
	EXPECT_EQ("SendActInfo",ccbname);

}
#endif // _DEBUG_SEND_ACTINFO

