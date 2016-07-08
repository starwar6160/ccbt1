#ifndef jcElockTestHdr_h__
#define jcElockTestHdr_h__

using boost::property_tree::ptree;
#include <gtest/gtest.h>
#include "zwCcbElockHdr.h"
//�����Ƿ�����������Ա�רһ����һ����
//#define _ZWTEST730
//��һ�׶ε�3�������Ƿ��
//���߿�����ȷ��Ӧ��2������
//#define _DEBUG_ACTREQ		//0000
//#define _DEBUG_SEND_ACTINFO	//0001
//
////�����ڲ�����ȱ�����ݶ��޷�������Ӧ�Ĳ���
//#define _DEBUG_READ_CLOSE_CODE		//0004
//#define _DEBUG_TIMESYNC				//0003

//#define _DEBUG_QUERY_LOCK_STATUS	//0002
//#define _DEBUG_GET_LOCK_LOG		//0005
//
//#define _DEBUG_RECV_INIT_CLOSECODE	//1000
//#define _DEBUG_RECV_VERIFY_CODE		//1002
//#define _DEBUG_WANMIN20141111



using jcAtmcConvertDLL::CCBSTR_CODE;
using jcAtmcConvertDLL::CCBSTR_NAME;

void myATMCRecvMsgRotine(const char *jcLockResponseXML);
//���һ��XML���ĵĽ��״���ͽ�������
void zwGetCCBMsgType(const string &inXML,string &outOpCode,string &outOpName);
//��ʱ����ص�������ȡ���Ľ�����Ա���֤��


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

//�����׼���ʼ���ͽ����¼�
class ccbElockTest : public testing::Test
{
public:
	int m_connStatus;

protected:
	virtual void SetUp();
	virtual void TearDown();
};

class secBoxTest : public testing::Test
{
public:
	//int m_connStatus;

protected:
	//virtual void SetUp();
	//virtual void TearDown();
};


namespace testMatch1607{
	using std::string;
	using std::deque;

	enum matchStatus{
		MATCH_TWOWAY_OK=11,
		MATCH_TWOWAY_ERROR=12,
		MATCH_SUP_OK=13,
		MATCH_SUP_ERROR=14
	};

	struct testMsgType{
		string msgCode;
		string msgType;
		enum matchStatus matchStatus;	//ƥ��״̬
	};

	class myMsgSts1607
	{
	public:
		myMsgSts1607();
		~myMsgSts1607();
		void PushDownMsg(const string &downXML);
		void PopDownMsgType(string &mCode,string &mType);
		void MatchDownMsgType(const string &upCode,const string &upType);
		void dumpDownDeque(void);

	private:
		deque<testMsgType> m_dqDown;	//�·�����
		deque<testMsgType> m_dqUpNotMatch;	//�������б�����δƥ���		
		string statusToStr(enum matchStatus mMatch);

	};
} //namespace testMatch1607{

#endif // jcElockTestHdr_h__
