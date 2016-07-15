#include "stdafx.h"
#include "jcElockTestHdr.h"
#include "CCBelock.h"

void zwSecboxWDXtest20141023(void);

string s_repActReqXML;	//�������յ��ļ�������ķ��ر���
string s_repLockInitXML;//�������յ��ĳ�ʼ������ķ��ر���
static const char *g_msg00="<?xml version='1.0' encoding='UTF-8'?><root><TransCode>0000</TransCode><TransName>CallForActInfo</TransName><TransDate>20160708</TransDate><TransTime>110900</TransTime><DevCode>515067001098</DevCode><SpareString1></SpareString1><SpareString2></SpareString2></root>";
static const char *g_msg01="<?xml version='1.0' encoding='UTF-8'?><root><TransCode>0001</TransCode><TransName>SendActInfo</TransName><TransDate>20160708</TransDate><TransTime>110900</TransTime><DevCode>515067001098</DevCode><LockMan>BeiJing.JinChu</LockMan><LockId>22222222</LockId><ActInfo>BFr4af4YvXxtsGmJnDCKsZ3OhmkZimSur0itl6fwuc3fqkiK6j05arPl2on3N4rfVLQkfo9GRceMmbXDebf7rdY=.h/UOVQjtcby5I3wQyUXDdB/uDTeUq1oW.vNDgbpmArJ2CTkBSAJ0NKDeZ6vUifPLKjbLZH3eiFE+QkEBOy5+r2ZJEYEgBmjD1KGIbVfridTa3sjuqaFo0lby7YpqBXTo56v5yIzyz28k=</ActInfo><SpareString1></SpareString1><SpareString2></SpareString2></root>";
static const char *g_msg02="<?xml version='1.0' encoding='UTF-8'?><root><TransCode>0002</TransCode><TransName>QueryForLockStatus</TransName><TransDate>20160708</TransDate><TransTime>110900</TransTime><DevCode>515067001098</DevCode><LockMan></LockMan><LockId></LockId><SpareString1></SpareString1><SpareString2></SpareString2></root>";
static const char *g_msg03="<?xml version='1.0' encoding='UTF-8'?><root><TransCode>0003</TransCode><TransName>TimeSync</TransName><TransDate>20160708</TransDate><TransTime>110900</TransTime></root>";
static const char *g_msg04="<?xml version='1.0' encoding='UTF-8'?><root><TransCode>0004</TransCode><TransName>ReadShutLockCode</TransName><TransDate>20160708</TransDate><TransTime>110900</TransTime><DevCode>515067001098</DevCode><SpareString1></SpareString1><SpareString2></SpareString2></root>"; 
//��0�ű��ķ���XML��ȡ��Կ������
string myGetPubKeyFromMsg0000Rep(const string msg0000RepXML)
{
	assert(msg0000RepXML.length()>0);
	ptree ptJC;
	std::stringstream ss;
	ss << msg0000RepXML;
	read_xml(ss, ptJC);
	string pubkeyLock = ptJC.get < string > ("root.LockPubKey");
	assert(pubkeyLock.length()>0);
	return pubkeyLock;
}


#ifdef _DEBUG_ACTREQ

string zwCode8ToHex(int Code8);


//���߼��������ĵ����߲���
TEST_F(ccbElockTest, LockActiveTest0000)
{
	printf("%d HEX=%s\n", 67386086, zwCode8ToHex(67386086).c_str());
	printf("%s HEX=%016X\n", "67386086", "67386086");

	//Open(25);
	EXPECT_EQ(ELOCK_ERROR_SUCCESS, SetRecvMsgRotine(myATMCRecvMsgRotine));
	const JC_MSG_TYPE msgType = JCMSG_LOCK_ACTIVE_REQUEST;	//�趨��Ϣ����
	//ATMC����XML��Ϣ
	string strLockActiveXML;	//�������ɵ���ϢXML
	//����������ϢXML
	ptree pt;
	jcAtmcMsg::zwAtmcTestMsgGen(msgType, strLockActiveXML);
	//strLockActiveXML=
	//"<?xml version=\"1.0\" encoding=\"utf-8\"?>"
	//"<root><TransCode>0000</TransCode>"
	//"<TransName>CallForActInfo</TransName>"
	//"<TransDate>20140807</TransDate>"
	//"<TransTime>153235</TransTime>"
	//"<DevCode>12345698</DevCode>"
	//"<SpareString1>NULL</SpareString1>"
	//"<SpareString2>NULL</SpareString2></root>";
	//"<?xml version=\"1.0\" encoding=\"utf-8\"?><root><TransCode>0000</TransCode><TransName>CallForActInfo</TransName><TransDate>20140807</TransDate><TransTime>173828</TransTime><DevCode>12345698Z</DevCode><SpareString1>NULL</SpareString1><SpareString2>NULL</SpareString2></root>";
	//����һ����һ����ԭ��XML��������ҳ����ʹ�ã�û��ת���ַ�
	// <?xml version="1.0" encoding="utf-8"?><root><TransCode>0000</TransCode><TransName>CallForActInfo</TransName><TransDate>20140807</TransDate><TransTime>173828</TransTime><DevCode>12345698Z</DevCode><SpareString1>NULL</SpareString1><SpareString2>NULL</SpareString2></root>
	//cout<<"Sleep 65 seconds,please wait"<<endl;

	if (ELOCK_ERROR_SUCCESS == m_connStatus) {
		EXPECT_EQ(ELOCK_ERROR_SUCCESS,
			  Notify(strLockActiveXML.c_str()));
	} else {
		EXPECT_EQ(ELOCK_ERROR_CONNECTLOST,
			  Notify(strLockActiveXML.c_str()));
		cout << "Server not Start!" << endl;
	}

	while (s_repActReqXML.length() == 0) {
		Sleep(200);
	}

	Sleep(500);
	EXPECT_LT(42, s_repActReqXML.length());

	string ccbop, ccbname;
	zwGetCCBMsgType(s_repActReqXML, ccbop, ccbname);
	EXPECT_EQ("0000", ccbop);
	EXPECT_EQ("CallForActInfo", ccbname);
	//Sleep(6*1000);
}
#endif // _DEBUG_ACTREQ

#ifdef _DEBUG_SEND_ACTINFO
//���߷��ͼ�����Ϣ(PSK)����(��ʼ��)�����߲���
TEST_F(ccbElockTest, LockSendActInfoTest0001)
{
	//Open(25);
	Sleep(2000);
	assert(s_repActReqXML.length()>0);
	cout<<"PubKey from Lock 20140911 is "<<myGetPubKeyFromMsg0000Rep(s_repActReqXML)<<endl;
	EXPECT_EQ(ELOCK_ERROR_SUCCESS, SetRecvMsgRotine(myATMCRecvMsgRotine));
	const JC_MSG_TYPE msgType = JCMSG_SEND_LOCK_ACTIVEINFO;	//�趨��Ϣ����
	//ATMC����XML��Ϣ
	string strSendLockActInfoXML;	//�������ɵ���ϢXML
	//����������ϢXML
	ptree pt;
	jcAtmcMsg::zwAtmcTestMsgGen(msgType, strSendLockActInfoXML);

	//cout<<"Sleep 65 seconds,please wait"<<endl;
	//Sleep(5*1000);
	if (ELOCK_ERROR_SUCCESS == m_connStatus) {
		EXPECT_EQ(ELOCK_ERROR_SUCCESS,
			  Notify(strSendLockActInfoXML.c_str()));
	} else {
		EXPECT_EQ(ELOCK_ERROR_CONNECTLOST,
			  Notify(strSendLockActInfoXML.c_str()));
		cout << "Server not Start!" << endl;
	}

	while (s_repLockInitXML.length() == 0) {
		Sleep(200);
	}
	EXPECT_LT(42, s_repLockInitXML.length());
	string ccbop, ccbname;
	zwGetCCBMsgType(s_repLockInitXML, ccbop, ccbname);
	EXPECT_EQ("0001", ccbop);
	EXPECT_EQ("SendActInfo", ccbname);

}
#endif // _DEBUG_SEND_ACTINFO


#ifdef _DEBUG_SECBOX_WDXTEST1
TEST_F(secBoxTest, WenDingXingTestZJY20141023)
{
	zwSecboxWDXtest20141023();
}
#endif // _DEBUG_SECBOX_WDXTEST1



void myATMCRecvMsgRotineT2(const char *pszMsg)
{
	myATMCRecvMsgRotine(pszMsg);
}


void zw1209SpeedTestThr2(void)
{
	//Sleep(200);
	//cout<<"["<<__FUNCTION__<<"] ThreadPID=["<<GetCurrentThreadId()<<"]\tSTART"<<endl;		
	SetRecvMsgRotine(myATMCRecvMsgRotine);	
	//EXPECT_EQ(ELOCK_ERROR_SUCCESS,Open(22));		
	//Sleep(5000);

	EXPECT_EQ(ELOCK_ERROR_SUCCESS,Notify(g_msg02));	
	EXPECT_EQ(ELOCK_ERROR_SUCCESS,Notify(g_msg03));	

	//���Դ�����һ��������Ա����ߺ��������������յ�
	//Sleep(1800);
	//EXPECT_EQ(ELOCK_ERROR_SUCCESS,Close());
	//cout<<"["<<__FUNCTION__<<"] ThreadPID=["<<GetCurrentThreadId()<<"]\tEND"<<endl;	
}


void zw1218OpenTimeTestThr(void)
{
	//cout<<"["<<__FUNCTION__<<"] ThreadPID=["<<GetCurrentThreadId()<<"]\tSTART"<<endl;		
	SetRecvMsgRotine(myATMCRecvMsgRotine);	
	int opMs=0;
	int opInv=800;
	for (int i=0;i<86400;i++)
	{
		int iErrCode=Open(-333);
		if (ELOCK_ERROR_SUCCESS==iErrCode)
		{
			cout<<"�����߳ɹ�"<<endl;
		}
		if (ELOCK_ERROR_SUCCESS!=iErrCode)
		{
			cout<<"������ʧ��,����ֵ="<<iErrCode<<endl;
		}

		Sleep(opInv);
		opMs=opMs+opInv;
		cout<<"�ӿ�ʼ��ȥ��"<<opMs/1000.0<<"��"<<endl;
	}
	
}


double zwGetUs(void)
{
	LARGE_INTEGER frq,cur;
	QueryPerformanceCounter(&cur);
	QueryPerformanceFrequency(&frq);
	return 1.0e6*cur.QuadPart/frq.QuadPart;
}

double zwGetMs(void)
{
	return zwGetUs()/1000.0;
}

	int G_TEST_UPNUM=0;	
	double G_MSGSTARTMS=0.0;

namespace testMatch1607{

	myMsgSts1607::myMsgSts1607()
	{

	}

	myMsgSts1607::~myMsgSts1607()
	{

	}

	string myMsgSts1607::statusToStr(enum matchStatus mMatch)
	{
		switch (mMatch)
		{
		case MATCH_TWOWAY_OK:
			return "MATCH_TWOWAY_OK";
			break;
		case MATCH_TWOWAY_ERROR:
			return "MATCH_TWOWAY_ERROR";
			break;
		case MATCH_SUP_OK:
			return "MATCH_SUP_OK";
			break;
		case MATCH_SUP_ERROR:
			return "MATCH_SUP_ERROR";
			break;
		}
		return "";
	};

	void myMsgSts1607::PushDownMsg(const string &downXML)
	{
		boost::mutex::scoped_lock lock(jcMatch_mutex);
		struct testMsgType *nItem=new struct testMsgType;
		zwGetCCBMsgType(downXML, nItem->msgCode,nItem->msgType);
		m_dqDown.push_back(*nItem);
	}

	void myMsgSts1607::PopDownMsgType(string &mCode,string &mType)
	{
		boost::mutex::scoped_lock lock(jcMatch_mutex);
		if (m_dqDown.size()>0)
		{
			struct testMsgType &nTop=m_dqDown.front();
			mCode=nTop.msgCode;
			mType=nTop.msgType;
			m_dqDown.pop_front();
		}
		else
		{
			mCode="";
			mType="";
		}
	}

	void myMsgSts1607::MatchDownMsgType(const string &upCode,const string &upType)
	{
		boost::mutex::scoped_lock lock(jcMatch_mutex);
		if (m_dqDown.size()>0)
		{
			//����ϴ���������ƥ���·����е�һ������ô�����·����е�һ��
			//�����������û��ƥ�䣬���������·�����,ͬʱ�ϴ����ļ���
			//��δƥ��ĵ����ϴ����Ķ��С�β��
			struct testMsgType &nTop=m_dqDown.front();
			if((upCode==nTop.msgCode) && (upType==nTop.msgType))
			{
				m_dqDown.pop_front();				
			}	
			else{
				struct testMsgType *nItem=new struct testMsgType;
				nItem->msgCode=upCode;
				nItem->msgType=upType;
				nItem->matchStatus=MATCH_TWOWAY_ERROR;
				m_dqUpNotMatch.push_back(*nItem);
			}
		}
		else
		{
			//�·������Ѿ�Ϊ�յĻ�����ô��������ȷ���ǵ����ϴ����ģ�
			//���롰δƥ��ĵ����ϴ����Ķ��С�β�����������dump�鿴��
			struct testMsgType *nItem=new struct testMsgType;
			nItem->msgCode=upCode;
			nItem->msgType=upType;
			nItem->matchStatus=MATCH_SUP_OK;
			m_dqUpNotMatch.push_back(*nItem);
		}

	}

	void myMsgSts1607::dumpDownDeque()
	{
		boost::mutex::scoped_lock lock(jcMatch_mutex);
		int downDqSize=m_dqDown.size();
		if (downDqSize>0)
		{
			cout<<"dumpDownDeque() �·���������δƥ����Ϣ��"<<downDqSize<<"��"<<endl;
			for (int i=0;i<downDqSize;i++)
			{
				const testMsgType &nItem=m_dqDown[i];
				cout<<nItem.msgCode<<" "<<nItem.msgType<<" "<<statusToStr(nItem.matchStatus)<<endl;
			}
		}

		int upDqSize=m_dqUpNotMatch.size();
		if (upDqSize>0)
		{
			cout<<"dumpDownDeque() �����ϴ���������δƥ����Ϣ��"<<upDqSize<<"��"<<endl;
			for (int i=0;i<upDqSize;i++)
			{
				const testMsgType &nItem=m_dqUpNotMatch[i];
				cout<<nItem.msgCode<<" "<<nItem.msgType<<" "<<statusToStr(nItem.matchStatus)<<endl;
			}
		}
		if (downDqSize==0 && upDqSize==0)
		{
			cout<<endl<<"[��������������Ϣ����ȷƥ����]"<<endl;
		}
	}
}	//namespace testMatch1607{

	
testMatch1607::myMsgSts1607 rdq;	

void zw1209SpeedTestThr1(void)
{
	//Sleep(100);
	//cout<<"["<<__FUNCTION__<<"] ThreadPID=["<<GetCurrentThreadId()<<"]\tSTART"<<endl;	
	SetRecvMsgRotine(myATMCRecvMsgRotine);	
	//EXPECT_EQ(ELOCK_ERROR_SUCCESS,Open(22));		

#ifdef _USE_FAKEHID_DEV20160705
	rdq.PushDownMsg(g_msg00);
	EXPECT_EQ(ELOCK_ERROR_SUCCESS,Notify(g_msg00));	

	rdq.PushDownMsg(g_msg01);
	EXPECT_EQ(ELOCK_ERROR_SUCCESS,Notify(g_msg01));	
	
	rdq.PushDownMsg(g_msg02);	
	EXPECT_EQ(ELOCK_ERROR_SUCCESS,Notify(g_msg02));	
	
	rdq.PushDownMsg(g_msg04);
	EXPECT_EQ(ELOCK_ERROR_SUCCESS,Notify(g_msg04));	

	rdq.PushDownMsg(g_msg03);
	EXPECT_EQ(ELOCK_ERROR_SUCCESS,Notify(g_msg03));	
#else
	rdq.PushDownMsg(g_msg02);	
	EXPECT_EQ(ELOCK_ERROR_SUCCESS,Notify(g_msg02));	

	rdq.PushDownMsg(g_msg03);	
	EXPECT_EQ(ELOCK_ERROR_SUCCESS,Notify(g_msg03));	

	/*rdq.PushDownMsg(g_msg04);
	EXPECT_EQ(ELOCK_ERROR_SUCCESS,Notify(g_msg04));	*/

	rdq.PushDownMsg(g_msg02);
	EXPECT_EQ(ELOCK_ERROR_SUCCESS,Notify(g_msg02));	

	rdq.PushDownMsg(g_msg03);	
	EXPECT_EQ(ELOCK_ERROR_SUCCESS,Notify(g_msg03));	

#endif // _USE_FAKEHID_DEV20160705	



	
	//���Դ�����һ��������Ա����ߺ��������������յ�	
		//while (G_TEST_UPNUM<7)
		//{
		//	printf("G_TEST_UPNUM=%d\n",G_TEST_UPNUM);
		//	Sleep(300);
		//}
	
	//Sleep(4000);
	//EXPECT_EQ(ELOCK_ERROR_SUCCESS,Close());
	//cout<<"["<<__FUNCTION__<<"] ThreadPID=["<<GetCurrentThreadId()<<"]\tEND"<<endl;	
	cout<<"zw1209SpeedTestThr1 ����"<<endl;
}


boost::mutex jctest1_mutex;	

void myTestPush712( const char * tMsg );
#include <cstdint>
using std::int64_t;
int g_totalRunCount=0;
void zw711SpeedTestThr1()
{
	assert(g_totalRunCount>0);
	const char *msgarr[]=
	{g_msg02,g_msg03,g_msg04,g_msg03,g_msg04,g_msg03,g_msg04,g_msg03,g_msg04};
	int aSize=sizeof(msgarr)/sizeof(char *);
	SetRecvMsgRotine(myATMCRecvMsgRotine);	
	int nCount=0;

	while(nCount++ <(g_totalRunCount/2))
	{		
		
		int idxMsg=static_cast<int64_t>(zwGetUs()) % aSize;
		assert(idxMsg>=0 && idxMsg <aSize);
		myTestPush712(msgarr[idxMsg]);
	}
	//cout<<"zw1209SpeedTestThr1 ����"<<endl;
}

void zw713SpeedTest1(void)
{
	SetRecvMsgRotine(myATMCRecvMsgRotine);	

	myTestPush712(g_msg00);
	myTestPush712(g_msg01);
}

void myTestPush712( const char * tMsg )
{
	{
		boost::mutex::scoped_lock lock(jctest1_mutex);
		rdq.PushDownMsg(tMsg);	
		EXPECT_EQ(ELOCK_ERROR_SUCCESS,Notify(tMsg));	
	}
}

int myGetRunCount(void)
{
	cout<<"��������Ҫ���е���Ϣ����"<<endl;
	int nCount=0;
	cin>>nCount;
	if (nCount<=0)
	{
		nCount=10;
	}
	return nCount;
}



TEST_F(ccbElockTest, jcHidDev20151207SpeedTestInATMCDLL)
{	
	//boost::thread *thr3=new boost::thread(zw1218OpenTimeTestThr);
	g_totalRunCount=myGetRunCount();
	boost::thread *thr1=new boost::thread(zw711SpeedTestThr1);	
	boost::thread *thr2=new boost::thread(zw711SpeedTestThr1);
	
	thr1->join();
	thr2->join();
	//thr3->join();
	//zw713SpeedTest1();
	Sleep(9000);
	//EXPECT_EQ(ELOCK_ERROR_SUCCESS,Close());
	//rdq.dumpDownDeque();
}

