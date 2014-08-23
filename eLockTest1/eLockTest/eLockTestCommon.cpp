#include "stdafx.h"
#include "jcElockTestHdr.h"
#include "CCBelock.h"

void ccbElockTest::SetUp() {
	cout<<__FUNCTION__<<endl;
	m_connStatus=ELOCK_ERROR_SUCCESS;
	m_connStatus=Open(25);
}
void ccbElockTest::TearDown() {
	cout<<__FUNCTION__<<endl;
	Close();
}

//临时保存回调函数获取到的结果，以便验证；
extern string s_repInitCloseCodeXML;
extern string s_repActReqXML;
extern string s_repLockInitXML;
extern string s_repReadCloseCodeXML;
extern string s_repVerifyCodeXML;

//获得一条XML报文的交易代码和交易名字
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


//测试性回调函数
void myATMCRecvMsgRotine(const char *jcLockResponseXML)
{
	assert(jcLockResponseXML!=NULL && strlen(jcLockResponseXML)>42);
	EXPECT_LT(42,strlen(jcLockResponseXML));
	string ccbop,ccbname;
	string rmsg=jcLockResponseXML;
	zwGetCCBMsgType(rmsg,ccbop,ccbname);
	cout<<"*******************建行ATMC回调函数开始***************####\n";	
	cout<<jcLockResponseXML<<endl;
	cout<<"*******************建行ATMC回调函数结束***************####\n";
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


string zwCode8ToHex(int Code8)
{
	//8位动态码转换为字符串，然后字符串8字节转换为HEX，以便满足3DES的
	//64bit输入要求，估计这样就满足建行的要求可以被正确解密了；
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
