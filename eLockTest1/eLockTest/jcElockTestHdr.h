#ifndef jcElockTestHdr_h__
#define jcElockTestHdr_h__

using boost::property_tree::ptree;
#include <gtest/gtest.h>
#include "zwCcbElockHdr.h"
//看看是否打开其他测试以便专一测试一件事
//#define _ZWTEST730
//第一阶段的3条测试是否打开
//锁具可以正确反应的2个测试
//#define _DEBUG_ACTREQ		//0000
//#define _DEBUG_SEND_ACTINFO	//0001
////
//////锁具内部由于缺乏数据而无法做出反应的测试
//#define _DEBUG_READ_CLOSE_CODE		//0004
//#define _DEBUG_TIMESYNC				//0003

#define _DEBUG_QUERY_LOCK_STATUS	//0002
//#define _DEBUG_GET_LOCK_LOG		//0005

//#define _DEBUG_RECV_INIT_CLOSECODE	//1000
//#define _DEBUG_RECV_VERIFY_CODE		//1002
//


using jcAtmcConvertDLL::CCBSTR_CODE;
using jcAtmcConvertDLL::CCBSTR_NAME;

void myATMCRecvMsgRotine(const char *jcLockResponseXML);
//获得一条XML报文的交易代码和交易名字
void zwGetCCBMsgType(const string &inXML,string &outOpCode,string &outOpName);
//临时保存回调函数获取到的结果，以便验证；


namespace jcAtmcMsg{
	extern const char *ATMNO_CCBTEST;
	extern const char *G_LOCKMAN_NAMEG;
	void zwAtmcTestMsgGen(const JC_MSG_TYPE type,string &strXML);
	string & myAtmcMsgLockActive( string & strXML ,ptree &pt);
	string & myAtmcMsgSendActiveInfo( string & strXML ,ptree &pt );
	string & myAtmcMsgReadCloseCodeInfo( string & strXML ,ptree &pt );
	string & myTestMsgRecvCloseCode( string & strXML ,ptree &pt );
	string & myTestMsgRecvVerifyCode( string & strXML ,ptree &pt );
	string & myTestMsgTimeSync( string & strXML ,ptree &pt );
	string & myTestMsgCheckLockStatus( string & strXML ,ptree &pt );
	string & myTestMsgGetLockLog( string & strXML ,ptree &pt );

}
using jcAtmcConvertDLL::CCBSTR_CODE;
using jcAtmcConvertDLL::CCBSTR_NAME;
using jcAtmcConvertDLL::CCBSTR_DATE;
using jcAtmcConvertDLL::CCBSTR_TIME;
using jcAtmcConvertDLL::CCBSTR_DEVCODE;
using jcAtmcMsg::ATMNO_CCBTEST;

//测试套件初始化和结束事件
class ccbElockTest : public testing::Test
{
public:
	int m_connStatus;

protected:
	virtual void SetUp();
	virtual void TearDown();
};

#endif // jcElockTestHdr_h__
