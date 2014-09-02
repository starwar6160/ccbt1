#include "stdafx.h"
#include "zwCcbElockHdr.h"
#include "myConvIntHdr.h"

namespace jcAtmcConvertDLL {
	//时间同步
	void zwconvTimeSyncDown(const ptree &, ptree & ptjc) {
		ZWFUNCTRACE
		    ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE, JCSTR_TIME_SYNC);
		ptjc.put < time_t > ("Lock_Time", time(NULL));
	}
	
	//时间同步 
	void zwconvTimeSyncUp(const ptree & ptjc, ptree & ptccb) {
		ZWFUNCTRACE
		    //无用的形式化部分
		    ptccb.put(CCBSTR_CODE, "0003");
		ptccb.put(CCBSTR_NAME, "TimeSync");	//使用缓存在内存中的值
		string zwDate, zwTime;
		zwGetLocalDateTimeString(ptjc.get < time_t > ("Lock_Time"),
					 zwDate, zwTime);
		ptccb.put(CCBSTR_DATE, zwDate);
		ptccb.put(CCBSTR_TIME, zwTime);
		//////////////////////////////////////////////////////////////////////////
		ptccb.put(CCBSTR_DEVCODE, ns_ccbAtmno);	//使用缓存在内存中的值
		ptccb.put("root.LockMan", LOCKMAN_NAME);
		ptccb.put("root.LockId", ptjc.get < string > ("Lock_Serial"));
		//////////////////////////////////////////////////////////////////////////
		//有用部分
		time_t exTimeValue = ptjc.get < time_t > ("Ex_Syn_Time");
		string exDate, exTime;
		zwGetLocalDateTimeString(exTimeValue, exDate, exTime);
		ptccb.put("root.ExSynDate", exDate);
		ptccb.put("root.ExSynTime", exTime);
	}

	//////////////////////////////////////////////////////////////////////////

	void zwconvLockReqTimeSyncDown(const ptree &, ptree & ptjc) {
		ZWFUNCTRACE
		    ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,
			     jcAtmcConvertDLL::JCSTR_REQUEST_TIME_SYNC);
		ptjc.put("Lock_Time", time(NULL));
	}

	void zwconvLockReqTimeSyncUp(const ptree & ptjc, ptree & ptccb) {
		ZWFUNCTRACE ptccb.put(CCBSTR_CODE, "1003");
		ptccb.put(CCBSTR_NAME, "TimeSync");
		string zwDate, zwTime;
		zwGetLocalDateTimeString(time(NULL), zwDate, zwTime);
		ptccb.put(CCBSTR_DATE, zwDate);
		ptccb.put(CCBSTR_TIME, zwTime);
		//锁具发送初始闭锁码时，ATM编号应该已经在激活请求中获得，但是
		//1.1版本报文里面没有给出，所以此处可能会有问题
		ptccb.put(CCBSTR_DEVCODE, ns_ccbAtmno);
		ptccb.put("root.LockMan", LOCKMAN_NAME);
		ptccb.put("root.LockId", ptjc.get < string > ("Lock_Serial"));
	}

}				//namespace jcAtmcConvertDLL{
