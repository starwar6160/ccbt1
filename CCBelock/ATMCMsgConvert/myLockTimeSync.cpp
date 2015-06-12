#include "stdafx.h"
#include "zwCcbElockHdr.h"
#include "myConvIntHdr.h"

namespace jcAtmcConvertDLL {

	time_t myGetCcbUTC(const ptree & ptccb)
	{
		string ccbDate = ptccb.get < string > (CCBSTR_DATE);
		string ccbTime = ptccb.get < string > (CCBSTR_TIME);	
		time_t ccbUTCSec = 0;
		zwCCBDateTime2UTC(ccbDate.c_str(), ccbTime.c_str(),
			&ccbUTCSec);
		return ccbUTCSec;
	}

	//时间同步
	void zwconvTimeSyncDown(const ptree & ptccb, ptree & ptjc) {
		//ZWFUNCTRACE
		    ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE, JCSTR_TIME_SYNC);
		//建行字符串格式的日期和时间字段合成转换为UTC秒数.开始
		//time_t nowSec = time(NULL);
		time_t ccbUTCSec = myGetCcbUTC(ptccb);
		//string ccbDate = ptccb.get < string > (CCBSTR_DATE);
		//string ccbTime = ptccb.get < string > (CCBSTR_TIME);	
		// zwCCBDateTime2UTC(ccbDate.c_str(), ccbTime.c_str(),
		//		   &ccbUTCSec);
		 assert(ccbUTCSec > 1400 * 1000 * 1000);

		//建行字符串格式的日期和时间字段合成转换为UTC秒数.结束
#ifdef _DEBUG
		 //printf("当前机器时间time(NULL)=%u\n",time(NULL));
		 //printf("zwconvTimeSyncDown ccbUTCSec from ATMVH is %u\n",ccbUTCSec);
#endif // _DEBUG
		 ptjc.put < time_t > ("Lock_Time", ccbUTCSec);
		//ptjc.put < time_t > ("Lock_Time", nowSec);
	}
	//时间同步  
	void zwconvTimeSyncUp(const ptree & ptjc, ptree & ptccb) {
		//ZWFUNCTRACE
		    //无用的形式化部分
		    ptccb.put(CCBSTR_CODE, "0003");
		ptccb.put(CCBSTR_NAME, "TimeSync");	//使用缓存在内存中的值
		string zwDate, zwTime;
		zwGetLocalDateTimeString(ptjc.get < time_t > ("Lock_Time"),
					 zwDate, zwTime);
		ptccb.put(CCBSTR_DATE, zwDate);
		ptccb.put(CCBSTR_TIME, zwTime);
		//////////////////////////////////////////////////////////////////////////
		ptccb.put(CCBSTR_DEVCODE, ptjc.get < string > ("Atm_Serial"));	//使用缓存在内存中的值
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

	void zwconvLockReqTimeSyncDown(const ptree & ptccb, ptree & ptjc) {
		ZWFUNCTRACE
		    ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,
			     jcAtmcConvertDLL::JCSTR_REQUEST_TIME_SYNC);
		//建行字符串格式的日期和时间字段合成转换为UTC秒数.开始
		string ccbDate = ptccb.get < string > (CCBSTR_DATE);
		string ccbTime = ptccb.get < string > (CCBSTR_TIME);
		time_t ccbUTCSec = 0;
		zwCCBDateTime2UTC(ccbDate.c_str(), ccbTime.c_str(), &ccbUTCSec);
		assert(ccbUTCSec > 1400 * 1000 * 1000);
		//建行字符串格式的日期和时间字段合成转换为UTC秒数.结束

		ptjc.put("Lock_Time", ccbUTCSec);
	}

	void zwconvLockReqTimeSyncUp(const ptree & ptjc, ptree & ptccb) {
		ZWFUNCTRACE 
		ptccb.put(CCBSTR_CODE, "1003");
		ptccb.put(CCBSTR_NAME, "TimeSync");
		string zwDate, zwTime;
		zwGetLocalDateTimeString(time(NULL), zwDate, zwTime);
		ptccb.put(CCBSTR_DATE, zwDate);
		ptccb.put(CCBSTR_TIME, zwTime);
		//锁具发送初始闭锁码时，ATM编号应该已经在激活请求中获得，但是
		//1.1版本报文里面没有给出，所以此处可能会有问题
		ptccb.put(CCBSTR_DEVCODE, ptjc.get < string > ("Atm_Serial"));
		ptccb.put("root.LockMan", LOCKMAN_NAME);
		ptccb.put("root.LockId", ptjc.get < string > ("Lock_Serial"));
	}
//////////////////////////////////////////////////////////////////////////
//20141111万敏.温度振动传感器报文支持

	void zwconvTemptureSenseDown(const ptree & ptccb, ptree & ptjc) {
		ZWFUNCTRACE
		    ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,
			     jcAtmcConvertDLL::JCSTR_SENSE_TEMPTURE);
		ptjc.put("Temperature", ptccb.get < int >("root.Temperature"));
	}

	void zwconvTemptureSenseUp(const ptree & ptjc, ptree & ptccb) {
		ZWFUNCTRACE ptccb.put(CCBSTR_CODE, "5000");
		ptccb.put(CCBSTR_NAME, "Set_Senser_Temperature");
		string zwDate, zwTime;
		zwGetLocalDateTimeString(time(NULL), zwDate, zwTime);
		ptccb.put(CCBSTR_DATE, zwDate);
		ptccb.put(CCBSTR_TIME, zwTime);
		//实质性有用字段
		ptccb.put(CCBSTR_DEVCODE, ptjc.get < string > ("Atm_Serial"));
		ptccb.put("root.LockId", ptjc.get < string > ("Lock_Serial"));
		ptccb.put("root.Status", ptjc.get < int >("Status"));
	}

	void zwconvShockSenseDown(const ptree & ptccb, ptree & ptjc) {
		ZWFUNCTRACE
		    ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,
			     jcAtmcConvertDLL::JCSTR_SENSE_SHOCK);
		ptjc.put("Shock", ptccb.get < int >("root.Shock"));
	}

	void zwconvShockSenseUp(const ptree & ptjc, ptree & ptccb) {
		ZWFUNCTRACE ptccb.put(CCBSTR_CODE, "5001");
		ptccb.put(CCBSTR_NAME, "Set_Senser_Shock");
		string zwDate, zwTime;
		zwGetLocalDateTimeString(time(NULL), zwDate, zwTime);
		ptccb.put(CCBSTR_DATE, zwDate);
		ptccb.put(CCBSTR_TIME, zwTime);
		//实质性有用字段
		ptccb.put(CCBSTR_DEVCODE, ptjc.get < string > ("Atm_Serial"));
		ptccb.put("root.LockId", ptjc.get < string > ("Lock_Serial"));
		ptccb.put("root.Status", ptjc.get < int >("Status"));
	}
//////////////////////////////////////////////////////////////////////////
//20141125.1508.万敏新增的5002/3/4三条命令
	////ATM机设置上传的小循环次数命令
	void zwconvTemptureSetInsideLoopTimesDown(const ptree & ptccb, ptree & ptjc) {
		ZWFUNCTRACE
			ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,
			jcAtmcConvertDLL::JCSTR_SENSE_SET_INSIDE_LOOP_TIMES);
		ptjc.put("Times", ptccb.get < int >("root.Times"));
	}

	void zwconvTemptureSetInsideLoopTimesUp( const ptree & ptjc, ptree & ptccb )
	{
		ZWFUNCTRACE 
		ptccb.put(CCBSTR_CODE, "5002");
		ptccb.put(CCBSTR_NAME, jcAtmcConvertDLL::JCSTR_SENSE_SET_INSIDE_LOOP_TIMES);
		string zwDate, zwTime;
		zwGetLocalDateTimeString(time(NULL), zwDate, zwTime);
		ptccb.put(CCBSTR_DATE, zwDate);
		ptccb.put(CCBSTR_TIME, zwTime);
		//实质性有用字段
		ptccb.put(CCBSTR_DEVCODE, ptjc.get < string > ("Atm_Serial"));
		ptccb.put("root.LockId", ptjc.get < string > ("Lock_Serial"));
		ptccb.put("root.Status", ptjc.get < int >("Status"));
	}
	//ATM机设置上传的小循环周期(单位秒)命令
	void zwconvTemptureSetInsideLoopPeriodDown(const ptree & ptccb, ptree & ptjc) {
		ZWFUNCTRACE
			ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,
			jcAtmcConvertDLL::JCSTR_SENSE_SET_INSIDE_LOOP_PERIOD);
		ptjc.put("Period", ptccb.get < int >("root.Period"));
	}

	void zwconvTemptureSetInsideLoopPeriodUp( const ptree & ptjc, ptree & ptccb )
	{
		ZWFUNCTRACE ptccb.put(CCBSTR_CODE, "5003");
		ptccb.put(CCBSTR_NAME, jcAtmcConvertDLL::JCSTR_SENSE_SET_INSIDE_LOOP_PERIOD);
		string zwDate, zwTime;
		zwGetLocalDateTimeString(time(NULL), zwDate, zwTime);
		ptccb.put(CCBSTR_DATE, zwDate);
		ptccb.put(CCBSTR_TIME, zwTime);
		//实质性有用字段
		ptccb.put(CCBSTR_DEVCODE, ptjc.get < string > ("Atm_Serial"));
		ptccb.put("root.LockId", ptjc.get < string > ("Lock_Serial"));
		ptccb.put("root.Status", ptjc.get < int >("Status"));
	}
	//ATM机设置上传的大循环周期(单位分钟)命令
	void zwconvTemptureSetOutsideLoopPeriodDown(const ptree & ptccb, ptree & ptjc) {
		ZWFUNCTRACE
			ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,
			jcAtmcConvertDLL::JCSTR_SENSE_SET_OUTSIDE_LOOP_PERIOD);
		ptjc.put("Period", ptccb.get < int >("root.Period"));
	}

	void zwconvTemptureSetOutsideLoopPeriodUp( const ptree & ptjc, ptree & ptccb )
	{
		ZWFUNCTRACE ptccb.put(CCBSTR_CODE, "5004");
		ptccb.put(CCBSTR_NAME, jcAtmcConvertDLL::JCSTR_SENSE_SET_OUTSIDE_LOOP_PERIOD);
		string zwDate, zwTime;
		zwGetLocalDateTimeString(time(NULL), zwDate, zwTime);
		ptccb.put(CCBSTR_DATE, zwDate);
		ptccb.put(CCBSTR_TIME, zwTime);
		//实质性有用字段
		ptccb.put(CCBSTR_DEVCODE, ptjc.get < string > ("Atm_Serial"));
		ptccb.put("root.LockId", ptjc.get < string > ("Lock_Serial"));
		ptccb.put("root.Status", ptjc.get < int >("Status"));
	}

//////////////////////////////////////////////////////////////////////////
	//获取XML报文类型
	string zwGetJcxmlMsgType(const char *jcXML) 
	{
		ptree ptccb;
		std::stringstream ss;
		ss << jcXML;
		read_xml(ss, ptccb);
		string msgType=ptccb.get<string>("root.TransCode");		
		return msgType;
	}


	//获取JSON报文类型
	string zwGetJcJsonMsgType(const char *jcJson) 
	{
		ptree ptjc;
		std::stringstream ss;
		ss << jcJson;
		read_json(ss, ptjc);
		string msgType=ptjc.get<string>("Command");		
		return msgType;
	}



}				//namespace jcAtmcConvertDLL{
