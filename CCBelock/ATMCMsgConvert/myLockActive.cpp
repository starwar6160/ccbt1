#include "stdafx.h"
#include "zwCcbElockHdr.h"
#include "myConvIntHdr.h"

namespace jcAtmcConvertDLL {
	time_t myGetCcbUTC(const ptree & ptccb);
//发送锁具激活请求
	void zwconvLockActiveDown(const ptree & ptccb, ptree & ptjc) {
		    ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,
			     JCSTR_LOCK_ACTIVE_REQUEST);
		ptjc.put("Atm_Serial", ptccb.get < string > (CCBSTR_DEVCODE));
	} 
	
	void zwconvLockActiveUp(const ptree & ptjc, ptree & ptccb) {
		    //无用的形式化部分
		    ptccb.put(CCBSTR_CODE, "0000");
		assert("CallForActInfo" == ns_ActReqName);
		ptccb.put(CCBSTR_NAME, ns_ActReqName);
		ptccb.put(CCBSTR_DEVCODE, ptjc.get < string > ("Atm_Serial"));
		//////////////////////////////////////////////////////////////////////////
		string zwDate, zwTime;
		zwGetLocalDateTimeString(ptjc.get < time_t > ("Lock_Time"),
					 zwDate, zwTime);
		ptccb.put(CCBSTR_DATE, zwDate);
		ptccb.put(CCBSTR_TIME, zwTime);

		//////////////////////////////////////////////////////////////////////////
		//有用部分

		ptccb.put("root.LockId", ptjc.get < string > ("Lock_Serial"));
		ptccb.put("root.LockPubKey",
			  ptjc.get < string > ("Lock_Public_Key"));
		ptccb.put("root.LockMan", LOCKMAN_NAME);
	}
//////////////////////////////////////////////////////////////////////////

//发送锁具激活信息(锁具初始化)
	void zwconvLockInitDown(const ptree & ptccb, ptree & ptjc) {
		    //锁具初始化
		    //>> 上位机下发
		    //      "command": JCSTR_LOCK_INIT,
		    //              "Lock_Time"
		    //              "Atm_Serial"
		    //              "Lock_Init_Info":"ECIES加密过的PSK"
		    //>> 锁具正确接收后，返回
		    //      "command": JCSTR_LOCK_INIT,
		    //      "Lock_Time"
		    //      "Lock_Serial"
		    //      "State"
		    //      "Lock_Init_Info":"锁具ECIES解密出来的PSK明文,用于ATMVH存档"
		    ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE, JCSTR_LOCK_INIT);
		//以下字段(LMS时间)是JC特有，CCB没有,时间或许还应该用建行报文中的时间来转换
		//建行字符串格式的日期和时间字段合成转换为UTC秒数.开始
		//string ccbDate = ptccb.get < string > (CCBSTR_DATE);
		//string ccbTime = ptccb.get < string > (CCBSTR_TIME);
		time_t ccbUTCSec = myGetCcbUTC(ptccb);
		//zwCCBDateTime2UTC(ccbDate.c_str(), ccbTime.c_str(), &ccbUTCSec);
		assert(ccbUTCSec > 1400 * 1000 * 1000);
		//建行字符串格式的日期和时间字段合成转换为UTC秒数.结束

		ptjc.put("Lock_Time", ccbUTCSec);
		ptjc.put("Atm_Serial", ptccb.get < string > (CCBSTR_DEVCODE));
		//此处1.1版本已经支持ECIES加密过的PSK输入了，使用第一套密文
		ptjc.put("Lock_Init_Info",
			 ptccb.get < string > ("root.ActInfo"));
	}

	void zwconvLockInitUp(const ptree & ptjc, ptree & ptccb) {
		    //无用的形式化部分
		    ptccb.put(CCBSTR_CODE, "0001");
		ptccb.put(CCBSTR_NAME, ns_LockInitName);	//使用缓存在内存中的值
		assert("SendActInfo" == ns_LockInitName);
		string zwDate, zwTime;
		zwGetLocalDateTimeString(ptjc.get < time_t > ("Lock_Time"),
					 zwDate, zwTime);
		ptccb.put(CCBSTR_DATE, zwDate);
		ptccb.put(CCBSTR_TIME, zwTime);
		ptccb.put(CCBSTR_DEVCODE, ptjc.get < string > ("Atm_Serial"));	//使用缓存在内存中的值
		ptccb.put("root.LockMan", LOCKMAN_NAME);
		ptccb.put("root.LockId", ptjc.get < string > ("Lock_Serial"));
		//有用部分
		int ActiveResult = 0;	//建行定义该字段0为成功，1为失败；
		ActiveResult = ptjc.get < int >("State");
		ptccb.put("root.ActiveResult", ActiveResult);
		//1.1版本里面下位机解密了ECIES加密的PSK并在Lock_Init_Info字段返回
		string lActInfo=ptjc.get < string > ("Lock_Init_Info");
		//20151214.1152.潘飞说Lock_System_Init命令的Lock_Init_Info字段有时候
		//错误的返回空格内容造成上层应用base64编码后成为无用的多余乱码
		// 所以在此提前过滤掉； 周伟
		lActInfo=zwtrim(lActInfo);
		ptccb.put("root.ActInfo",lActInfo);
	}

}				//namespace jcAtmcConvertDLL{
