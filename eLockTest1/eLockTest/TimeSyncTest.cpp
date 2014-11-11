#include "stdafx.h"
#include "jcElockTestHdr.h"
#include "CCBelock.h"

#ifdef _DEBUG_TIMESYNC
TEST_F(ccbElockTest, TimeSyncTest0003)
{
	EXPECT_EQ(ELOCK_ERROR_SUCCESS, SetRecvMsgRotine(myATMCRecvMsgRotine));
	const JC_MSG_TYPE msgType = JCMSG_TIME_SYNC;	//设定消息类型
	//ATMC生成XML消息
	string strSendLockActInfoXML;	//容纳生成的消息XML
	//具体生成消息XML
	ptree pt;
	jcAtmcMsg::zwAtmcTestMsgGen(msgType, strSendLockActInfoXML);

	if (ELOCK_ERROR_SUCCESS == m_connStatus) {
		EXPECT_EQ(ELOCK_ERROR_SUCCESS,
			  Notify(strSendLockActInfoXML.c_str()));
		Sleep(1 * 1000);
		EXPECT_EQ(ELOCK_ERROR_SUCCESS,
			  Notify(strSendLockActInfoXML.c_str()));
	} else {
		EXPECT_EQ(ELOCK_ERROR_CONNECTLOST,
			  Notify(strSendLockActInfoXML.c_str()));
		cout << "Server not Start!" << endl;
	}
	Sleep(1 * 1000);
}
#endif // _DEBUG_TIMESYNC

TEST_F(ccbElockTest, WanMinTools_20141111)
{
	const char * zwInputFile="W:\\zwWorkSrc\\2014BinOut\\Debug\\jcinput.xml";
	//const char * zwInputFile="jcinput.xml";
	FILE *ifp=fopen(zwInputFile,"rb");
	assert(NULL!=ifp);
	const int BUFLEN=512;
	char buf[BUFLEN+1];
		memset(buf,0,BUFLEN+1);
		int frr=fread(buf,1,BUFLEN,ifp);
		assert(frr>=0 && frr <=BUFLEN);	
	fclose(ifp);
	SetRecvMsgRotine(myATMCRecvMsgRotine);
	Notify(buf);
}
