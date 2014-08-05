// eLockTest1.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "CCBelock.h"
#include "zwCcbElockHdr.h"
//看看是否打开其他测试以便专一测试一件事
//#define _ZWTEST730
//第一阶段的3条测试是否打开
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


string s_retstr;	//临时保存回调函数获取到的结果，以便验证；

//获得一条XML报文的交易代码和交易名字
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


//测试性回调函数
void myATMCRecvMsgRotine(const char *pszMsg)
{
	assert(pszMsg!=NULL && strlen(pszMsg)>42);
	EXPECT_LT(42,strlen(pszMsg));
	cout<<"*******************建行ATMC回调函数开始*******************\n";	
	cout<<pszMsg<<endl;
	cout<<"*******************建行ATMC回调函数结束*******************\n";
	s_retstr=pszMsg;
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




#ifdef _USE_STAGE1_TEST805

//发送初始闭锁码测试
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

//锁具激活请求报文的在线测试
TEST_F(ccbElockTest,LockActiveTestOnline)
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


//锁具发送激活信息(PSK)报文(初始化)的在线测试
TEST_F(ccbElockTest,LockSendActInfoTestOnline)
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


//读取闭锁码报文的在线测试
TEST_F(ccbElockTest,ReadCloseCodeTestOnline)
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

