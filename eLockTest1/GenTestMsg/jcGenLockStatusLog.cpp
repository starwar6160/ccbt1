#include "stdafx.h"
#include "..\\eLockTest\\jcElockTestHdr.h"

namespace jcAtmcMsg {
	//生成测试用的0002查询锁具状态
	string & myTestMsgCheckLockStatus(string & strXML, ptree & pt) {
		//开始生成请求报文
		pt.put(CCBSTR_CODE, "0002");
		pt.put(CCBSTR_NAME, "QueryForLockStatus");
		string zwDate, zwTime;
		 zwGetLocalDateTimeString(time(NULL), zwDate, zwTime);
		 pt.put(CCBSTR_DATE, zwDate);
		 pt.put(CCBSTR_TIME, zwTime);
		 pt.put(CCBSTR_DEVCODE, ATMNO_CCBTEST);
		 pt.put("root.LockMan", jcAtmcMsg::G_LOCKMAN_NAMEG);
		 pt.put("root.LockId", "ZWFAKELOCKNO1548");
		 std::ostringstream ss;
		 write_xml(ss, pt);
		 strXML = ss.str();
		 return strXML;
	}
	//生成测试用的0005读取日志报文，目的在于模拟建行ATM机器的行为
	    string & myTestMsgGetLockLog(string & strXML, ptree & pt) {
		//开始生成请求报文
		pt.put(CCBSTR_CODE, "0005");
		pt.put(CCBSTR_NAME, "ReadLog");
		string zwDate, zwTime;
		zwGetLocalDateTimeString(time(NULL), zwDate, zwTime);
		pt.put(CCBSTR_DATE, zwDate);
		pt.put(CCBSTR_TIME, zwTime);
		pt.put("root.BeginNo", 0);
		pt.put("root.EndNo", 23);
		std::ostringstream ss;
		write_xml(ss, pt);
		strXML = ss.str();
		return strXML;
	}

}				//namespace jcAtmcMsg{
