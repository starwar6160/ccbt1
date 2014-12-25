#include "stdafx.h"
#include "zwCcbElockHdr.h"
#include "myConvIntHdr.h"

namespace jcAtmcConvertDLL {
	//��ѯ����״̬
	void zwconvQueryLockStatusDown(const ptree &, ptree & ptjc) {
		ZWFUNCTRACE
		    ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,
			     JCSTR_QUERY_LOCK_STATUS);
	} void zwconvCheckLockStatusUp(const ptree & ptjc, ptree & ptccb) {
		ZWFUNCTRACE
		    //"Command": "Lock_Now_Info",
		    //"Lock_Time": "1408434961",
		    //"Lock_Serial": "A3KE2OK256EO2SPE",
		    //"Lock_Status": "0,0,0,1,100,0,1,20,100,0,0"
		    //���õ���ʽ������
		    ptccb.put(CCBSTR_CODE, "0002");
		ptccb.put(CCBSTR_NAME, "QueryForLockStatus");
		string zwDate, zwTime;
		zwGetLocalDateTimeString(ptjc.get < time_t > ("Lock_Time"),
					 zwDate, zwTime);
		ptccb.put(CCBSTR_DATE, zwDate);
		ptccb.put(CCBSTR_TIME, zwTime);
		//////////////////////////////////////////////////////////////////////////
		ptccb.put(CCBSTR_DEVCODE, ptjc.get < string > ("Atm_Serial"));	//ʹ�û������ڴ��е�ֵ
		ptccb.put("root.LockMan", LOCKMAN_NAME);
		ptccb.put("root.LockId", ptjc.get < string > ("Lock_Serial"));
		string LockStatusStr = ptjc.get < string > ("Lock_Status");
		//��ʽ��ActiveStatus,EnableStatus,LockStatus,DoorStatus,
		//BatteryStatus,ShockAlert,TempAlert,PswTryAlert,LockOverTime
		//0,0,0,1,100,0,1,20,100,0,0
		JCLOCKSTATUS lockStatusStrings;
		zwLockStatusDataSplit(LockStatusStr.c_str(), lockStatusStrings);

		ptccb.put("root.ActiveStatus", lockStatusStrings.ActiveStatus);
		ptccb.put("root.EnableStatus", lockStatusStrings.EnableStatus);
		ptccb.put("root.LockStatus", lockStatusStrings.LockStatus);
		ptccb.put("root.DoorStatus", lockStatusStrings.DoorStatus);
		ptccb.put("root.BatteryStatus",
			  lockStatusStrings.BatteryStatus);
		ptccb.put("root.ShockAlert", lockStatusStrings.ShockAlert);
		ptccb.put("root.ShockValue", lockStatusStrings.ShockValue);
		ptccb.put("root.TempAlert", lockStatusStrings.TempAlert);
		ptccb.put("root.NodeTemp", lockStatusStrings.nodeTemp);
		ptccb.put("root.PswTryAlert", lockStatusStrings.PswTryAlert);
		ptccb.put("root.LockOverTime", lockStatusStrings.LockOverTime);
	}

	//��ȡ��־
	void zwconvGetLockLogDown(const ptree & ptccb, ptree & ptjc) {
		ZWFUNCTRACE
		    ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,
			     JCSTR_GET_LOCK_LOG);
		ptjc.put("Begin_No", ptccb.get < int >("root.BeginNo"));
		ptjc.put("End_No", ptccb.get < int >("root.EndNo"));
	}

	void zwconvGetLockLogUp(const ptree & ptjc, ptree & ptccb) {
		ZWFUNCTRACE ptccb.put(CCBSTR_CODE, "0005");
		ptccb.put(CCBSTR_NAME, "ReadLog");
		string zwDate, zwTime;
		zwGetLocalDateTimeString(ptjc.get < time_t > ("Lock_Time"),
					 zwDate, zwTime);
		ptccb.put(CCBSTR_DATE, zwDate);
		ptccb.put(CCBSTR_TIME, zwTime);
		ptccb.put("root.RevResult", ptjc.get < int >("State"));
		string LockStatus = ptjc.get < string > ("Journal");
		assert(LockStatus.size() > 0);

//////////////////////////////////////////////////////////////////////////
//20140916.1458.�������ҵĲ��Ե�·�����0005���ķ��ص���־���������⣬����
//ʹ�ø����߲���ʱ��Ҫ��һ��4�н�����־�Ĵ���ע�͵�
		JCLOCKSTATUS lst, ostr;
		zwLockStatusDataSplit(LockStatus.c_str(), lst);
		zwStatusData2String(lst, ostr);
		LockStatus = LockStatusStringMerge(ostr);
//////////////////////////////////////////////////////////////////////////
		ptccb.put("root.Log", LockStatus);
	}

	void zwconvLockPushWarnDown(const ptree & ptccb, ptree & ptjc) {
		ZWFUNCTRACE
		    ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,
			     jcAtmcConvertDLL::JCSTR_PUSH_WARNING);
		ptjc.put("State", ptccb.get < int >("root.RevResult"));
		ptjc.put("Atm_Serial", ptccb.get < string >(CCBSTR_DEVCODE));
	}

	void zwconvLockPushWarnUp(const ptree & ptjc, ptree & ptccb) {
		ZWFUNCTRACE ptccb.put(CCBSTR_CODE, "1001");
		ptccb.put(CCBSTR_NAME, "SendAlertStatus");
		string zwDate, zwTime;
		zwGetLocalDateTimeString(time(NULL), zwDate, zwTime);
		ptccb.put(CCBSTR_DATE, zwDate);
		ptccb.put(CCBSTR_TIME, zwTime);
		//���߷��ͳ�ʼ������ʱ��ATM���Ӧ���Ѿ��ڼ��������л�ã�����
		//1.1�汾��������û�и��������Դ˴����ܻ�������
		ptccb.put(CCBSTR_DEVCODE, ptjc.get < string > ("Atm_Serial"));
		ptccb.put("root.LockMan", LOCKMAN_NAME);
		ptccb.put("root.LockId", ptjc.get < string > ("Lock_Serial"));
		string LockStatusStr = ptjc.get < string > ("Lock_Status");
		//��ʽ��ActiveStatus,EnableStatus,LockStatus,DoorStatus,
		//BatteryStatus,ShockAlert,TempAlert,PswTryAlert,LockOverTime
		//0,0,0,1,100,0,1,20,100,0,0
		JCLOCKSTATUS lockStatusStrings;
		zwLockStatusDataSplit(LockStatusStr.c_str(), lockStatusStrings);

		ptccb.put("root.ActiveStatus", lockStatusStrings.ActiveStatus);
		ptccb.put("root.EnableStatus", lockStatusStrings.EnableStatus);
		ptccb.put("root.LockStatus", lockStatusStrings.LockStatus);
		ptccb.put("root.DoorStatus", lockStatusStrings.DoorStatus);
		ptccb.put("root.BatteryStatus",
			  lockStatusStrings.BatteryStatus);
		ptccb.put("root.ShockAlert", lockStatusStrings.ShockAlert);
		ptccb.put("root.ShockValue", lockStatusStrings.ShockValue);
		ptccb.put("root.TempAlert", lockStatusStrings.TempAlert);
		ptccb.put("root.NodeTemp", lockStatusStrings.nodeTemp);
		ptccb.put("root.PswTryAlert", lockStatusStrings.PswTryAlert);
		ptccb.put("root.LockOverTime", lockStatusStrings.LockOverTime);
	}

}				//namespace jcAtmcConvertDLL{
