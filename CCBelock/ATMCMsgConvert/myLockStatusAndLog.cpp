#include "stdafx.h"
#include "zwCcbElockHdr.h"
#include "myConvIntHdr.h"

namespace jcAtmcConvertDLL {
	//查询锁具状态
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
		    //无用的形式化部分
		    ptccb.put(CCBSTR_CODE, "0002");
		ptccb.put(CCBSTR_NAME, "QueryForLockStatus");
		string zwDate, zwTime;
		zwGetLocalDateTimeString(ptjc.get < time_t > ("Lock_Time"),
					 zwDate, zwTime);
		ptccb.put(CCBSTR_DATE, zwDate);
		ptccb.put(CCBSTR_TIME, zwTime);
		//////////////////////////////////////////////////////////////////////////
		ptccb.put(CCBSTR_DEVCODE, ptjc.get < string > ("Atm_Serial"));	//使用缓存在内存中的值
		ptccb.put("root.LockMan", LOCKMAN_NAME);
		ptccb.put("root.LockId", ptjc.get < string > ("Lock_Serial"));
		string LockStatusStr = ptjc.get < string > ("Lock_Status");
		//格式：ActiveStatus,EnableStatus,LockStatus,DoorStatus,
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

	//读取日志
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
//20140916.1458.万敏给我的测试电路板对于0005报文返回的日志内容有问题，所以
//使用该锁具测试时需要把一下4行解析日志的代码注释掉
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
		//锁具发送初始闭锁码时，ATM编号应该已经在激活请求中获得，但是
		//1.1版本报文里面没有给出，所以此处可能会有问题
		ptccb.put(CCBSTR_DEVCODE, ptjc.get < string > ("Atm_Serial"));
		ptccb.put("root.LockMan", LOCKMAN_NAME);
		ptccb.put("root.LockId", ptjc.get < string > ("Lock_Serial"));
		string LockStatusStr = ptjc.get < string > ("Lock_Status");
		//格式：ActiveStatus,EnableStatus,LockStatus,DoorStatus,
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
