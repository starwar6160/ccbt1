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
