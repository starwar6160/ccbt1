#ifndef jcElockTestHdr_h__
#define jcElockTestHdr_h__
#include <gtest/gtest.h>
#include "CCBelock.h"
#include "zwCcbElockHdr.h"
using namespace boost::property_tree;
//�����Ƿ�����������Ա�רһ����һ����
//#define _ZWTEST730
//��һ�׶ε�3�������Ƿ��
//���߿�����ȷ��Ӧ��2������
#define _DEBUG_ACTREQ
#define _DEBUG_SEND_ACTINFO
//�����ڲ�����ȱ�����ݶ��޷�������Ӧ�Ĳ���
#define _DEBUG_RECV_INIT_CLOSECODE
#define _DEBUG_RECV_VERIFY_CODE
#define _DEBUG_READ_CLOSE_CODE

//#define _DEBUG_QUERY_LOCK_STATUS
//#define _DEBUG_TIMESYNC
//#define _DEBUG_GET_LOCK_LOG

void myATMCRecvMsgRotine(const char *jcLockResponseXML);
//���һ��XML���ĵĽ��״���ͽ�������
void zwGetCCBMsgType(const string &inXML,string &outOpCode,string &outOpName);
//��ʱ����ص�������ȡ���Ľ�����Ա���֤��
extern string s_repInitCloseCodeXML;
extern string s_repActReqXML;
extern string s_repLockInitXML;
extern string s_repReadCloseCodeXML;
extern string s_repVerifyCodeXML;

namespace jcAtmcMsg{
	void zwAtmcTestMsgGen(const JC_MSG_TYPE type,string &strXML,ptree &pt);
}

const JC_MSG_TYPE lockParseInJson( const string & inJson, ptree &pt );
void LockGenOutputJson( const ptree &pt, string &outJson );

extern const int ZW_END_WAIT;
extern const char *myLongMsg;

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



#endif // jcElockTestHdr_h__
