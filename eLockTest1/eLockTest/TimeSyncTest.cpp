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

//20141111.1720.为万敏做的测试程序
TEST_F(ccbElockTest, WanMinTools_20141111)
{
	//const char * zwInputFile="W:\\zwWorkSrc\\2014BinOut\\Debug\\jcinput.xml";
	const char * zwInputFile="jcinput.xml";
	FILE *ifp=fopen(zwInputFile,"rb");
	assert(NULL!=ifp);
	if (NULL==ifp)
	{
		printf("error open input file %s\n",zwInputFile);
		return ;
	}
	const int BUFLEN=512;
	const int CMDLEN=BUFLEN*4;
	char buf[BUFLEN+1];	
	char cmdXML[CMDLEN+1];
	memset(cmdXML,0,CMDLEN+1);
	while(!feof(ifp))
	{
		memset(buf,0,BUFLEN+1);
		fgets(buf,BUFLEN,ifp);
		//过滤掉回车换行
		char *strend=strstr(buf,"\r\n");
		if (NULL!=strend)
		{
			*strend=NULL;			
		}				
		strcat(cmdXML,buf);
	}
	fclose(ifp);
	SetRecvMsgRotine(myATMCRecvMsgRotine);
	Notify(cmdXML);
	Sleep(3000);
}
