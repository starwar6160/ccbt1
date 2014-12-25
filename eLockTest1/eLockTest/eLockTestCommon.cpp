#include "stdafx.h"
#include "jcElockTestHdr.h"
#include "CCBelock.h"

void ccbElockTest::SetUp()
{
	cout << __FUNCTION__ << endl;
	m_connStatus = ELOCK_ERROR_SUCCESS;
	m_connStatus = Open(25);
	const char *errmsg="Wait 9 Seconds for Plug Out/In Test 20141225\n";
	printf(errmsg);
	OutputDebugStringA(errmsg);
	Sleep(9000);
}

void ccbElockTest::TearDown()
{
	cout << __FUNCTION__ << endl;
	Close();
}

//��ʱ����ص�������ȡ���Ľ�����Ա���֤��
extern string s_repInitCloseCodeXML;
extern string s_repActReqXML;
extern string s_repLockInitXML;
extern string s_repReadCloseCodeXML;
extern string s_repVerifyCodeXML;

//���һ��XML���ĵĽ��״���ͽ�������
void zwGetCCBMsgType(const string & inXML, string & outOpCode,
		     string & outOpName)
{
	try {
		std::stringstream ss;
		ss << inXML;
		ptree pt;
		read_xml(ss, pt);
		outOpCode = pt.get < string > (CCBSTR_CODE);
		outOpName = pt.get < string > (CCBSTR_NAME);
	}
	catch(...) {
		outOpCode = "";
		outOpName = "";
	}
}

//�����Իص�����
void myATMCRecvMsgRotine(const char *jcLockResponseXML)
{
	assert(jcLockResponseXML != NULL && strlen(jcLockResponseXML) > 42);
	EXPECT_LT(42, strlen(jcLockResponseXML));
	string ccbop, ccbname;
	string rmsg = jcLockResponseXML;
	zwGetCCBMsgType(rmsg, ccbop, ccbname);
	cout << "*******************����ATMC�ص�������ʼ***************####\n";
	cout << jcLockResponseXML << endl;
	cout << "*******************����ATMC�ص���������***************####\n";
	OutputDebugStringA(__FUNCTION__);
	OutputDebugStringA(jcLockResponseXML);
	if ("0000" == ccbop) {
		s_repActReqXML = rmsg;
	}
	if ("0001" == ccbop) {
		s_repLockInitXML = rmsg;
	}
	if ("0004" == ccbop) {
		s_repReadCloseCodeXML = rmsg;
	}
	if ("1000" == ccbop) {
		s_repInitCloseCodeXML = rmsg;
	}
	if ("1002" == ccbop) {
		s_repVerifyCodeXML = rmsg;
	}
}

string zwCode8ToHex(int Code8)
{
	//8λ��̬��ת��Ϊ�ַ�����Ȼ���ַ���8�ֽ�ת��ΪHEX���Ա�����3DES��
	//64bit����Ҫ�󣬹������������㽨�е�Ҫ����Ա���ȷ�����ˣ�
	const int BUFLEN = 32;
	char buf[BUFLEN];
	memset(buf, 0, BUFLEN);
	sprintf(buf, "%08d", Code8);
	assert(strlen(buf) == 8);
	char hexbuf[BUFLEN];
	memset(hexbuf, 0, BUFLEN);
	for (int i = 0; i < 8; i++) {
		unsigned char ch = buf[i] % 256;
		sprintf(hexbuf + i * 2, "%02X", ch);
	}
	string retHexStr = hexbuf;
	return retHexStr;
}

namespace jcAtmcMsg {
	const char *ATMNO_CCBTEST = "CCBATMNO0922";
	const char *G_LOCKMAN_NAMEG = "BeiJing.JinChu";

	//����ģ���ATMC XML��Ϣ������ڣ�����ö��������Ӧ��һ����XML��Ϣ
	void zwAtmcTestMsgGen(const JC_MSG_TYPE type, string & strXML) {
		ptree pt;
		switch (type) {
		case JCMSG_LOCK_ACTIVE_REQUEST:
			strXML = myAtmcMsgLockActive(strXML, pt);
			assert(strXML.length() > 42);	//XML��ͷ�Ĺ̶�����38���ַ����������һ����ǩ�����Լ����źϼ�4���ַ�
			break;
			case JCMSG_SEND_LOCK_ACTIVEINFO:strXML =
			    myAtmcMsgSendActiveInfo(strXML, pt);
			assert(strXML.length() > 42);
			break;
			case JCMSG_GET_CLOSECODE:strXML =
			    myAtmcMsgReadCloseCodeInfo(strXML, pt);
			assert(strXML.length() > 42);
			break;
			//������������������ϴ���Ϣ�Ĳ������·���Ϣ,���в�����Щ�·�����
			case JCMSG_SEND_INITCLOSECODE:strXML =
			    myTestMsgRecvCloseCode(strXML, pt);
			assert(strXML.length() > 42);
			break;
			case JCMSG_SEND_UNLOCK_CERTCODE:strXML =
			    myTestMsgRecvVerifyCode(strXML, pt);
			assert(strXML.length() > 42);
			break;
			case JCMSG_TIME_SYNC:strXML =
			    myTestMsgTimeSync(strXML, pt);
			assert(strXML.length() > 42);
			break;
			case JCMSG_QUERY_LOCK_STATUS:strXML =
			    myTestMsgCheckLockStatus(strXML, pt);
			assert(strXML.length() > 42);
			break;
			case JCMSG_GET_LOCK_LOG:strXML =
			    myTestMsgGetLockLog(strXML, pt);
			assert(strXML.length() > 42);
			break;
}}}				//namespace jcAtmcMsg{

