// eLockTest1.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "CCBelock.h"
#include "zwCcbElockHdr.h"
//�����Ƿ�����������Ա�רһ����һ����
//#define _ZWTEST730
//��һ�׶ε�3�������Ƿ��
#define _USE_STAGE1_TEST805
using namespace boost::property_tree;
const JC_MSG_TYPE lockParseJson( const string & inJson, ptree &pt );
void LockOutJson( const ptree &pt, string &outJson );

const char *myLongMsg="0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
	"0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"
	"0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF";

const int ZW_END_WAIT=500;

namespace jcAtmcMsg{
	void zwAtmcMsgGen(const JC_MSG_TYPE type,string &strXML,ptree &pt);
}


string s_retstr;	//��ʱ����ص�������ȡ���Ľ�����Ա���֤��

//���һ��XML���ĵĽ��״���ͽ�������
void zwGetCCBMsgType(const string &inXML,string &outOpCode,string &outOpName)
{
	try{
		std::stringstream ss;
		ss<<inXML;
		ptree pt;
		read_xml(ss,pt);
		outOpCode=pt.get<string>("TransCode");
		outOpName=pt.get<string>("TransName");
	}
	catch(...)
	{
		outOpCode="";
		outOpName="";
	}
}


//�����Իص�����
void myATMCRecvMsgRotine(const char *pszMsg)
{
	assert(pszMsg!=NULL && strlen(pszMsg)>42);
	EXPECT_LT(42,strlen(pszMsg));
	cout<<"*******************����ATMC�ص�������ʼ*******************\n";	
	cout<<pszMsg<<endl;
	cout<<"*******************����ATMC�ص���������*******************\n";
	s_retstr=pszMsg;
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


TEST(ccbElockDeathTest,NotifyTestBad)
{//Debug�»ᴥ�����ԵķǷ�������ԣ��ŵ����������н���
	//EXPECT_DEBUG_DEATH(Notify(myLongMsg),"");
	EXPECT_DEBUG_DEATH(Notify(""),"");
	EXPECT_DEBUG_DEATH(Notify(NULL),"");
}


#ifdef _DEBUG_USE_UNITTEST804
//���߼���ĵĵ�Ԫ����
TEST_F(ccbElockTest,LockActiveTestUnit)
{
	const JC_MSG_TYPE msgType=JCMSG_LOCK_ACTIVE_REQUEST;	//�趨��Ϣ����
	EXPECT_EQ(ELOCK_ERROR_SUCCESS,SetRecvMsgRotine(myATMCRecvMsgRotine));	
	//////////////////////////////////////////////////////////////////////////
	//ATMC����XML��Ϣ
	string strLockActiveXML;	//�������ɵ���ϢXML
	//����������ϢXML
	ptree pt;
	jcAtmcMsg::zwAtmcMsgGen(msgType,strLockActiveXML, pt);	
	EXPECT_LT(42,strLockActiveXML.length());	//�������ɵ�XML����42�ֽ�����
	string strLockActiveJson;	//����XMLת���ɵ�JSON
	//////////////////////////////////////////////////////////////////////////
	//DLL��XMLת��ΪJSON
	//����XML��JSON�е���Ϣ�����ֶ�������ָ������Ϣ����
	JC_MSG_TYPE rType=jcAtmcConvertDLL::zwCCBxml2JCjson(strLockActiveXML,strLockActiveJson);
	EXPECT_EQ(msgType,rType);
	EXPECT_LT(9,strLockActiveJson.length());	//����ת��������JSON����9�ֽ�����
	//////////////////////////////////////////////////////////////////////////
	//�����ڲ�����JSON
	ptree pt2;
	JC_MSG_TYPE mtypeL=lockParseJson(strLockActiveJson, pt2);
	EXPECT_EQ(msgType,mtypeL);
	string outJson;
	//��pt���Ϊjson����
	LockOutJson(pt2, outJson);
	//DLL��JSONת��ΪXML���ظ�ATMC
	string outXML;
	jcAtmcConvertDLL::zwJCjson2CCBxml(outJson,outXML);
	EXPECT_LT(42,outXML.length());	//�������ɵ�XML����42�ֽ�����
	//////////////////////////////////////////////////////////////////////////
	//��������ڲ��������֮�󷵻ص������Ƿ���д�����ȷ�ķ���ֵ
	//����Ҳ��ֻ�ܴ��¼�鳤�ȶ���
	ptree ptc;
	std::stringstream ssc;
	ssc<<outXML;
	read_xml(ssc,ptc);
	string lockMan=ptc.get<string>("LockMan");
	string LockId=ptc.get<string>("LockId");
	string LockPubKey=ptc.get<string>("LockPubKey");
	EXPECT_LT(10,lockMan.length());
	EXPECT_GT(20,lockMan.length());
	EXPECT_LT(10,LockId.length());
	EXPECT_GT(20,LockId.length());
	EXPECT_LT(85,LockPubKey.length());
	EXPECT_GT(100,LockPubKey.length());
	//�õ�Ԫ�������������磬���Բ�������֤�ص������յ������ݳ��ȣ�
}
#endif // _DEBUG_USE_UNITTEST804


#endif // _ZWTEST730
//////////////////////////////////////////////////////////////////////////




#ifdef _USE_STAGE1_TEST805

//���ͳ�ʼ���������
TEST_F(ccbElockTest,SendInitCloseOnline)
{
	EXPECT_EQ(ELOCK_ERROR_SUCCESS,SetRecvMsgRotine(myATMCRecvMsgRotine));	
	Sleep(ZW_END_WAIT);
	EXPECT_LT(42,s_retstr.length());

	while (s_retstr.length()==0)
	{
		Sleep(200);
	}
	//string ccbop,ccbname;
	//zwGetCCBMsgType(s_retstr,ccbop,ccbname);
	//EXPECT_EQ("0000",ccbop);
	//EXPECT_EQ("CallForActInfo",ccbname);

}

//���߼��������ĵ����߲���
TEST_F(ccbElockTest,LockActiveTestOnline)
{
	//Open(25);
	EXPECT_EQ(ELOCK_ERROR_SUCCESS,SetRecvMsgRotine(myATMCRecvMsgRotine));	
	const JC_MSG_TYPE msgType=JCMSG_LOCK_ACTIVE_REQUEST;	//�趨��Ϣ����
	//ATMC����XML��Ϣ
	string strLockActiveXML;	//�������ɵ���ϢXML
	//����������ϢXML
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
	Sleep(ZW_END_WAIT);
	EXPECT_LT(42,s_retstr.length());

	while (s_retstr.length()==0)
	{
		Sleep(200);
	}
	string ccbop,ccbname;
	zwGetCCBMsgType(s_retstr,ccbop,ccbname);
	EXPECT_EQ("0000",ccbop);
	EXPECT_EQ("CallForActInfo",ccbname);

}


//���߷��ͼ�����Ϣ(PSK)����(��ʼ��)�����߲���
TEST_F(ccbElockTest,LockSendActInfoTestOnline)
{
	//Open(25);
	//Sleep(2000);
	EXPECT_EQ(ELOCK_ERROR_SUCCESS,SetRecvMsgRotine(myATMCRecvMsgRotine));	
	const JC_MSG_TYPE msgType=JCMSG_SEND_LOCK_ACTIVEINFO;	//�趨��Ϣ����
	//ATMC����XML��Ϣ
	string strSendLockActInfoXML;	//�������ɵ���ϢXML
	//����������ϢXML
	ptree pt;
	jcAtmcMsg::zwAtmcMsgGen(msgType,strSendLockActInfoXML, pt);	

	if (ELOCK_ERROR_SUCCESS==m_connStatus)
	{
		EXPECT_EQ(ELOCK_ERROR_SUCCESS,Notify(strSendLockActInfoXML.c_str()));		
	}
	else
	{
		EXPECT_EQ(ELOCK_ERROR_CONNECTLOST,Notify(strSendLockActInfoXML.c_str()));		
		cout<<"Server not Start!"<<endl;
	}

#ifdef NDEBUG
	EXPECT_EQ(ELOCK_ERROR_PARAMINVALID,SetRecvMsgRotine(NULL));
#endif // NDEBUG
	Sleep(ZW_END_WAIT);
	EXPECT_LT(42,s_retstr.length());
	while (s_retstr.length()==0)
	{
		Sleep(200);
	}
	string ccbop,ccbname;
	zwGetCCBMsgType(s_retstr,ccbop,ccbname);
	EXPECT_EQ("0001",ccbop);
	EXPECT_EQ("SendActInfo",ccbname);

}


//��ȡ�����뱨�ĵ����߲���
TEST_F(ccbElockTest,ReadCloseCodeTestOnline)
{
	//Open(25);
	EXPECT_EQ(ELOCK_ERROR_SUCCESS,SetRecvMsgRotine(myATMCRecvMsgRotine));	
	const JC_MSG_TYPE msgType=JCMSG_GET_CLOSECODE;	//�趨��Ϣ����
	//ATMC����XML��Ϣ
	string strSendLockActInfoXML;	//�������ɵ���ϢXML
	//����������ϢXML
	ptree pt;
	jcAtmcMsg::zwAtmcMsgGen(msgType,strSendLockActInfoXML, pt);	

	if (ELOCK_ERROR_SUCCESS==m_connStatus)
	{
		EXPECT_EQ(ELOCK_ERROR_SUCCESS,Notify(strSendLockActInfoXML.c_str()));		
	}
	else
	{
		EXPECT_EQ(ELOCK_ERROR_CONNECTLOST,Notify(strSendLockActInfoXML.c_str()));		
		cout<<"Server not Start!"<<endl;
	}

#ifdef NDEBUG
	EXPECT_EQ(ELOCK_ERROR_PARAMINVALID,SetRecvMsgRotine(NULL));
#endif // NDEBUG
	Sleep(ZW_END_WAIT);
	EXPECT_LT(42,s_retstr.length());

	while (s_retstr.length()==0)
	{
		Sleep(200);
	}
	string ccbop,ccbname;
	zwGetCCBMsgType(s_retstr,ccbop,ccbname);
	EXPECT_EQ("0004",ccbop);
	EXPECT_EQ("ReadShutLockCode",ccbname);
}
#endif // _USE_STAGE1_TEST805


int _tmain(int argc, _TCHAR* argv[])
{	
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

