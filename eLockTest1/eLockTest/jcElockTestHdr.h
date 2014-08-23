#ifndef jcElockTestHdr_h__
#define jcElockTestHdr_h__

using boost::property_tree::ptree;
#include <gtest/gtest.h>
#include "zwCcbElockHdr.h"
//�����Ƿ�����������Ա�רһ����һ����
#define _ZWTEST730
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


using jcAtmcConvertDLL::CCBSTR_CODE;
using jcAtmcConvertDLL::CCBSTR_NAME;

void myATMCRecvMsgRotine(const char *jcLockResponseXML);
//���һ��XML���ĵĽ��״���ͽ�������
void zwGetCCBMsgType(const string &inXML,string &outOpCode,string &outOpName);
//��ʱ����ص�������ȡ���Ľ�����Ա���֤��


namespace jcAtmcMsg{
	void zwAtmcTestMsgGen(const JC_MSG_TYPE type,string &strXML);
}

//�����׼���ʼ���ͽ����¼�
class ccbElockTest : public testing::Test
{
public:
	int m_connStatus;

protected:
	virtual void SetUp();
	virtual void TearDown();
};

#endif // jcElockTestHdr_h__
