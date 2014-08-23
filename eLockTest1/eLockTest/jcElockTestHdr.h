#ifndef jcElockTestHdr_h__
#define jcElockTestHdr_h__

using boost::property_tree::ptree;
#include <gtest/gtest.h>
#include "zwCcbElockHdr.h"
//看看是否打开其他测试以便专一测试一件事
#define _ZWTEST730
//第一阶段的3条测试是否打开
//锁具可以正确反应的2个测试
#define _DEBUG_ACTREQ
#define _DEBUG_SEND_ACTINFO
//锁具内部由于缺乏数据而无法做出反应的测试
#define _DEBUG_RECV_INIT_CLOSECODE
#define _DEBUG_RECV_VERIFY_CODE
#define _DEBUG_READ_CLOSE_CODE

//#define _DEBUG_QUERY_LOCK_STATUS
//#define _DEBUG_TIMESYNC
//#define _DEBUG_GET_LOCK_LOG


using jcAtmcConvertDLL::CCBSTR_CODE;
using jcAtmcConvertDLL::CCBSTR_NAME;

void myATMCRecvMsgRotine(const char *jcLockResponseXML);
//获得一条XML报文的交易代码和交易名字
void zwGetCCBMsgType(const string &inXML,string &outOpCode,string &outOpName);
//临时保存回调函数获取到的结果，以便验证；


namespace jcAtmcMsg{
	void zwAtmcTestMsgGen(const JC_MSG_TYPE type,string &strXML);
}

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
