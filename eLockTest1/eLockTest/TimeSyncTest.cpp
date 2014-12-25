#include "stdafx.h"
#include "jcElockTestHdr.h"
#include "CCBelock.h"

#ifdef _DEBUG_TIMESYNC
TEST_F(ccbElockTest, TimeSyncTest0003)
{
	EXPECT_EQ(ELOCK_ERROR_SUCCESS, SetRecvMsgRotine(myATMCRecvMsgRotine));
	const JC_MSG_TYPE msgType = JCMSG_TIME_SYNC;	//�趨��Ϣ����
	//ATMC����XML��Ϣ
	string strSendLockActInfoXML;	//�������ɵ���ϢXML
	//����������ϢXML
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

//������β�Ļس������У��ո��Ʊ����ֱ���޸��ַ���
int zwCutLineEndSpace(char *str)
{
	int len=strlen(str);
	int spaceEnd=0;	//ȥ���ո���ĩβ
	if (NULL==str || 0==len)
	{
		return 0;
	}

	//��β����ǰ���ҵ���һ�����ǻس�/����/�ո�/�Ʊ��/NULL���ַ���Ȼ����ַ����濪ʼ�ض�
	for (int i=len;i>=0;i--)
	{
		if (0x20!=str[i] && '\n'!=str[i] && '\r'!=str[i] && '\t'!=str[i] && 0!=str[i])
		{
			spaceEnd=i+1;
			str[spaceEnd]=NULL;
			break;
		}
	}
	for (int i=spaceEnd;i<=len;i++)
	{
		str[i]=NULL;	//������հײ���ΪNULL���������
	}
	return spaceEnd;
}

#ifdef _DEBUG_WANMIN20141111
//20141111.1720.Ϊ�������Ĳ��Գ���
TEST_F(ccbElockTest, WanMinTools_20141111)
{
	const char * zwInputFile="jcinput.xml";
	FILE *ifp=fopen(zwInputFile,"r");
	assert(NULL!=ifp);
	if (NULL==ifp)
	{
		printf("error open input file %s\n",zwInputFile);
		return ;
	}
	string sbuf,sXML;
	const int BUFLEN=512;
	char lineBuf[BUFLEN];
	while (!feof(ifp))
	{
		memset(lineBuf,0,BUFLEN);
		fgets(lineBuf,BUFLEN,ifp);
		zwCutLineEndSpace(lineBuf);
		printf("%s",lineBuf);
		sXML+=lineBuf;
	}
	printf("\n");
	fclose(ifp);
	SetRecvMsgRotine(myATMCRecvMsgRotine);
	Notify(sXML.c_str());
	Sleep(3000);
}
#endif // _DEBUG_WANMIN20141111
