// eLockTest1.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <gtest/gtest.h>
#include "CCBelock.h"
#include "zwCcbElockHdr.h"
using jcAtmcConvertDLL::CCBSTR_CODE;
using jcAtmcConvertDLL::CCBSTR_NAME;
//�����Ƿ�����������Ա�רһ����һ����
//#define _ZWTEST730
//��һ�׶ε�3�������Ƿ��
//���߿�����ȷ��Ӧ��2������
//#define _DEBUG_ACTREQ
//#define _DEBUG_SEND_ACTINFO
//�����ڲ�����ȱ�����ݶ��޷�������Ӧ�Ĳ���
//#define _DEBUG_RECV_INIT_CLOSECODE
//#define _DEBUG_RECV_VERIFY_CODE
#define _DEBUG_READ_CLOSE_CODE
//#define _DEBUG_TIMESYNC



using namespace boost::property_tree;
const JC_MSG_TYPE lockParseInJson( const string & inJson, ptree &pt );
void LockGenOutputJson( const ptree &pt, string &outJson );

const char *myLongMsg="0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
	"0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
	"0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF";

const int ZW_END_WAIT=500;

namespace jcAtmcMsg{
	void zwAtmcTestMsgGen(const JC_MSG_TYPE type,string &strXML,ptree &pt);
}


//��ʱ����ص�������ȡ���Ľ�����Ա���֤��
string s_repInitCloseCodeXML;
string s_repActReqXML;
string s_repLockInitXML;
string s_repReadCloseCodeXML;
string s_repVerifyCodeXML;

//���һ��XML���ĵĽ��״���ͽ�������
void zwGetCCBMsgType(const string &inXML,string &outOpCode,string &outOpName)
{
	try{
		std::stringstream ss;
		ss<<inXML;
		ptree pt;
		read_xml(ss,pt);
		outOpCode=pt.get<string>(CCBSTR_CODE);
		outOpName=pt.get<string>(CCBSTR_NAME);
	}
	catch(...)
	{
		outOpCode="";
		outOpName="";
	}
}


//�����Իص�����
void myATMCRecvMsgRotine(const char *jcLockResponseXML)
{
	assert(jcLockResponseXML!=NULL && strlen(jcLockResponseXML)>42);
	EXPECT_LT(42,strlen(jcLockResponseXML));
	string ccbop,ccbname;
	string rmsg=jcLockResponseXML;
	zwGetCCBMsgType(rmsg,ccbop,ccbname);
	cout<<"*******************����ATMC�ص�������ʼ***************####\n";	
	cout<<jcLockResponseXML<<endl;
	cout<<"*******************����ATMC�ص���������***************####\n";
	OutputDebugStringA(__FUNCTION__);
	OutputDebugStringA(jcLockResponseXML);
	if ("0000"==ccbop)
	{
		s_repActReqXML=rmsg;
	}
	if ("0001"==ccbop)
	{
		s_repLockInitXML=rmsg;
	}
	if ("0004"==ccbop)
	{
		s_repReadCloseCodeXML=rmsg;
	}
	if ("1000"==ccbop)
	{
		s_repInitCloseCodeXML=rmsg;
	}
	if ("1002"==ccbop)
	{
		s_repVerifyCodeXML=rmsg;
	}
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
	EXPECT_DEBUG_DEATH(Open(90000),"");
}


TEST(ccbElockDeathTest,NotifyTestBad)
{//Debug�»ᴥ�����ԵķǷ�������ԣ��ŵ����������н���
	//EXPECT_DEBUG_DEATH(Notify(myLongMsg),"");
	EXPECT_DEBUG_DEATH(Notify(""),"");
	EXPECT_DEBUG_DEATH(Notify(NULL),"");
}

#endif // _ZWTEST730
//////////////////////////////////////////////////////////////////////////
//TEST_F(ccbElockTest,zwWaitForCommThreadStart)
//{//�ò���������,ֻ��Ϊ�˵ȴ�ͨѶ�߳���������ʱ�������,20140805.1630.��ΰ
//	Sleep(1000);
//}


string zwCode8ToHex(int Code8)
{
	//8λ��̬��ת��Ϊ�ַ�����Ȼ���ַ���8�ֽ�ת��ΪHEX���Ա�����3DES��
	//64bit����Ҫ�󣬹������������㽨�е�Ҫ����Ա���ȷ�����ˣ�
	const int BUFLEN=32;
	char buf[BUFLEN];
	memset(buf,0,BUFLEN);
	sprintf(buf,"%08d",Code8);
	assert(strlen(buf)==8);
	char hexbuf[BUFLEN];
	memset(hexbuf,0,BUFLEN);
	for (int i=0;i<8;i++)
	{
		unsigned char ch=buf[i] % 256;
		sprintf(hexbuf+i*2,"%02X",ch);
	}
	string retHexStr=hexbuf;
	return retHexStr;
}


#ifdef _DEBUG_ACTREQ
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
	}
	else
	{
		EXPECT_EQ(ELOCK_ERROR_CONNECTLOST,Notify(strSendLockActInfoXML.c_str()));		
		cout<<"Server not Start!"<<endl;
	}
	Sleep(63*1000);
}
#endif // _DEBUG_TIMESYNC

#ifdef _DEBUG_READ_CLOSE_CODE
//��ȡ�����뱨�ĵ����߲���
TEST_F(ccbElockTest,ReadCloseCodeTest0004)
{
	//Open(25);
	EXPECT_EQ(ELOCK_ERROR_SUCCESS,SetRecvMsgRotine(myATMCRecvMsgRotine));	
	const JC_MSG_TYPE msgType=JCMSG_GET_CLOSECODE;	//�趨��Ϣ����
	//ATMC����XML��Ϣ
	string strSendLockActInfoXML;	//�������ɵ���ϢXML
	//����������ϢXML
	ptree pt;
	jcAtmcMsg::zwAtmcTestMsgGen(msgType,strSendLockActInfoXML, pt);	

	if (ELOCK_ERROR_SUCCESS==m_connStatus)
	{
		EXPECT_EQ(ELOCK_ERROR_SUCCESS,Notify(strSendLockActInfoXML.c_str()));
		EXPECT_EQ(ELOCK_ERROR_SUCCESS,Notify(strSendLockActInfoXML.c_str()));
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
	Sleep(63*1000);
}
#endif // _DEBUG_READ_CLOSE_CODE

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

