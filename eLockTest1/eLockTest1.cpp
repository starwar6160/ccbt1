// eLockTest1.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "CCBelock.h"
#include "zwCcbElockHdr.h"
//看看是否打开其他测试以便专一测试一件事
//#define _ZWTEST730
//第一阶段的3条测试是否打开

#define _DEBUG_RECV_INIT_CLOSECODE
//#define _DEBUG_RECV_VERIFY_CODE

#define _DEBUG_ACTREQ
#define _DEBUG_SEND_ACTINFO
#define _DEBUG_READ_CLOSE_CODE



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


//临时保存回调函数获取到的结果，以便验证；
string s_retInitCloseCode;
string s_retActReq;
string s_retLockInit;
string s_retReadCloseCode;
string s_retVerifyCode;

//获得一条XML报文的交易代码和交易名字
void zwGetCCBMsgType(const string &inXML,string &outOpCode,string &outOpName)
{
	try{
		std::stringstream ss;
		ss<<inXML;
		ptree pt;
		read_xml(ss,pt);
		outOpCode=pt.get<string>("root.TransCode");
		outOpName=pt.get<string>("root.TransName");
	}
	catch(...)
	{
		outOpCode="";
		outOpName="";
	}
}


//测试性回调函数
void myATMCRecvMsgRotine(const char *pszMsg)
{
	assert(pszMsg!=NULL && strlen(pszMsg)>42);
	EXPECT_LT(42,strlen(pszMsg));
	string ccbop,ccbname;
	string rmsg=pszMsg;
	zwGetCCBMsgType(rmsg,ccbop,ccbname);
	cout<<"*******************建行ATMC回调函数开始***************####\n";	
	cout<<pszMsg<<endl;
	cout<<"*******************建行ATMC回调函数结束***************####\n";
	OutputDebugStringA(__FUNCTION__);
	OutputDebugStringA(pszMsg);
	if ("0000"==ccbop)
	{
		s_retActReq=rmsg;
	}
	if ("0001"==ccbop)
	{
		s_retLockInit=rmsg;
	}
	if ("0004"==ccbop)
	{
		s_retReadCloseCode=rmsg;
	}
	if ("1000"==ccbop)
	{
		s_retInitCloseCode=rmsg;
	}
	if ("1002"==ccbop)
	{
		s_retVerifyCode=rmsg;
	}
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
		Sleep(300);	//等待通讯线程把网络连接建立起来
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


TEST(ccbElockDeathTest,NotifyTestBad)
{//Debug下会触发断言的非法输入测试，放到死亡测试中进行
	//EXPECT_DEBUG_DEATH(Notify(myLongMsg),"");
	EXPECT_DEBUG_DEATH(Notify(""),"");
	EXPECT_DEBUG_DEATH(Notify(NULL),"");
}


#ifdef _DEBUG_USE_UNITTEST804
//锁具激活报文的单元测试
TEST_F(ccbElockTest,LockActiveTestUnit)
{
	const JC_MSG_TYPE msgType=JCMSG_LOCK_ACTIVE_REQUEST;	//设定消息类型
	EXPECT_EQ(ELOCK_ERROR_SUCCESS,SetRecvMsgRotine(myATMCRecvMsgRotine));	
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
	JC_MSG_TYPE rType=jcAtmcConvertDLL::zwCCBxml2JCjson(strLockActiveXML,strLockActiveJson);
	EXPECT_EQ(msgType,rType);
	EXPECT_LT(9,strLockActiveJson.length());	//期望转换出来的JSON至少9字节以上
	//////////////////////////////////////////////////////////////////////////
	//锁具内部处理JSON
	ptree pt2;
	JC_MSG_TYPE mtypeL=lockParseJson(strLockActiveJson, pt2);
	EXPECT_EQ(msgType,mtypeL);
	string outJson;
	//把pt输出为json返回
	LockOutJson(pt2, outJson);
	//DLL把JSON转换为XML返回给ATMC
	string outXML;
	jcAtmcConvertDLL::zwJCjson2CCBxml(outJson,outXML);
	EXPECT_LT(42,outXML.length());	//期望生成的XML至少42字节以上
	//////////////////////////////////////////////////////////////////////////
	//检查锁具内部处理完毕之后返回的内容是否具有大致正确的返回值
	//不过也就只能大致检查长度而已
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
	//该单元测试跳过了网络，所以不适用验证回调函数收到的内容长度；
}
#endif // _DEBUG_USE_UNITTEST804


#endif // _ZWTEST730
//////////////////////////////////////////////////////////////////////////
//TEST_F(ccbElockTest,zwWaitForCommThreadStart)
//{//该测试无意义,只是为了等待通讯线程启动的临时解决方法,20140805.1630.周伟
//	Sleep(1000);
//}


#ifdef _DEBUG_RECV_INIT_CLOSECODE
//接收锁具主动发送的初始闭锁码测试
TEST_F(ccbElockTest,RecvInitClose1000)
{
	EXPECT_EQ(ELOCK_ERROR_SUCCESS,SetRecvMsgRotine(myATMCRecvMsgRotine));	
	while (s_retInitCloseCode.length()==0)
	{
		Sleep(200);
	}
	//Sleep(ZW_END_WAIT);
	EXPECT_LT(42,s_retInitCloseCode.length());
	string ccbop,ccbname;
	zwGetCCBMsgType(s_retInitCloseCode,ccbop,ccbname);
	EXPECT_EQ("1000",ccbop);
	EXPECT_EQ("SendShutLockCode",ccbname);
}
#endif // _DEBUG_RECV_INIT_CLOSECODE

#ifdef _DEBUG_RECV_VERIFY_CODE
//接收锁具主动发送的验证码测试
TEST_F(ccbElockTest,RecvVerifyClose1002)
{
	EXPECT_EQ(ELOCK_ERROR_SUCCESS,SetRecvMsgRotine(myATMCRecvMsgRotine));	
	while (s_retVerifyCode.length()==0)
	{
		Sleep(200);
	}
	//Sleep(ZW_END_WAIT);
	EXPECT_LT(42,s_retVerifyCode.length());
	string ccbop,ccbname;
	zwGetCCBMsgType(s_retVerifyCode,ccbop,ccbname);
	EXPECT_EQ("1002",ccbop);
	EXPECT_EQ("SendUnLockIdent",ccbname);
}
#endif // _DEBUG_RECV_VERIFY_CODE



#ifdef _DEBUG_ACTREQ
//锁具激活请求报文的在线测试
TEST_F(ccbElockTest,LockActiveTest0000)
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
	while (s_retActReq.length()==0)
	{
		Sleep(200);
	}

	Sleep(ZW_END_WAIT);
	EXPECT_LT(42,s_retActReq.length());

	string ccbop,ccbname;
	zwGetCCBMsgType(s_retActReq,ccbop,ccbname);
	EXPECT_EQ("0000",ccbop);
	EXPECT_EQ("CallForActInfo",ccbname);

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
	while (s_retLockInit.length()==0)
	{
		Sleep(200);
	}
	//Sleep(ZW_END_WAIT);
	EXPECT_LT(42,s_retLockInit.length());
	string ccbop,ccbname;
	zwGetCCBMsgType(s_retLockInit,ccbop,ccbname);
	EXPECT_EQ("0001",ccbop);
	EXPECT_EQ("SendActInfo",ccbname);

}
#endif // _DEBUG_SEND_ACTINFO


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
	while (s_retReadCloseCode.length()==0)
	{
		Sleep(200);
	}
	//Sleep(ZW_END_WAIT);
	EXPECT_LT(42,s_retReadCloseCode.length());

	string ccbop,ccbname;
	zwGetCCBMsgType(s_retReadCloseCode,ccbop,ccbname);
	EXPECT_EQ("0004",ccbop);
	EXPECT_EQ("ReadShutLockCode",ccbname);
}
#endif // _DEBUG_READ_CLOSE_CODE





int _tmain(int argc, _TCHAR* argv[])
{	
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

