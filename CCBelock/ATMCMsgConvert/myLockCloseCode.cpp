#include "stdafx.h"
#include "zwCcbElockHdr.h"
#include "myConvIntHdr.h"
extern Poco::LogStream * pocoLog;
namespace jcAtmcConvertDLL {
	//读取闭锁码
	void zwconvReadCloseCodeDown(const ptree & ptccb, ptree & ptjc) {
		ZWFUNCTRACE
		    //>> 读取闭锁码
		    //请求
		    //      "command": JCSTR_READ_CLOSECODE,
		    //      "Lock_Time"     
		    //应答
		    //"Command":JCSTR_READ_CLOSECODE
		    ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,
			     JCSTR_READ_CLOSECODE);
		//建行字符串格式的日期和时间字段合成转换为UTC秒数.开始
		string ccbDate = ptccb.get < string > (CCBSTR_DATE);
		string ccbTime = ptccb.get < string > (CCBSTR_TIME);
		time_t ccbUTCSec = 0;
		zwCCBDateTime2UTC(ccbDate.c_str(), ccbTime.c_str(), &ccbUTCSec);
		assert(ccbUTCSec > 1400 * 1000 * 1000);
		//建行字符串格式的日期和时间字段合成转换为UTC秒数.结束

		ptjc.put("Lock_Time", ccbUTCSec);
	} void zwconvReadCloseCodeUp(const ptree & ptjc, ptree & ptccb) {
		ZWFUNCTRACE
		    //读取闭锁码 应答
		    //      交易代码        TransCode       是      值：0004
		    //      交易名称        TransName       是      值：ReadShutLockCode
		    //      交易日期        TransDate       是      值：YYYYMMDD，如20140401
		    //      交易时间        TransTime       是      值：hhmmss，如134050
		    //      ATM设备编号     DevCode 是      值：我行12位设备编号
		    //      锁具厂商        LockMan 是      值：厂商自定与其他厂商不同的名称
		    //      锁具编号        LockId  是      值：厂商自定的锁具唯一编号
		    //      闭锁码  ShutLockcode    是      值：闭锁码
		    //无用的形式化部分
		    ptccb.put(CCBSTR_CODE, "0004");
		assert("ReadShutLockCode" == ns_ReadCloseCodeName);
		ptccb.put(CCBSTR_NAME, ns_ReadCloseCodeName);
		string zwDate, zwTime;
		zwGetLocalDateTimeString(time(NULL), zwDate, zwTime);
		ptccb.put(CCBSTR_DATE, zwDate);
		ptccb.put(CCBSTR_TIME, zwTime);

		ptccb.put(CCBSTR_DEVCODE, ptjc.get < string > ("Atm_Serial"));
		ptccb.put("root.LockMan", LOCKMAN_NAME);
		ptccb.put("root.LockId", ptjc.get < string > ("Lock_Serial"));
		ptccb.put("root.ShutLockcode", ptjc.get < int >("Code"));
	}

	void zwconvRecvInitCloseCodeDown(const ptree &, ptree & ptjc) {
		ZWFUNCTRACE
		    ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,
			     "Lock_Close_Code_Lock");
	}

	void zwconvRecvInitCloseCodeUp(const ptree & ptjc, ptree & ptccb) {
		ZWFUNCTRACE ptccb.put(CCBSTR_CODE, "1000");
		ptccb.put(CCBSTR_NAME, "SendShutLockCode");
		string zwDate, zwTime;
		zwGetLocalDateTimeString(time(NULL), zwDate, zwTime);
		ptccb.put(CCBSTR_DATE, zwDate);
		ptccb.put(CCBSTR_TIME, zwTime);
		//锁具发送初始闭锁码时，ATM编号应该已经在激活请求中获得，但是
		//1.1版本报文里面没有给出，所以此处可能会有问题
		ptccb.put(CCBSTR_DEVCODE, ptjc.get < string > ("Atm_Serial"));
		ptccb.put("root.LockMan", LOCKMAN_NAME);
		ptccb.put("root.ShutLockcode", ptjc.get < int >("Code"));
	}

	void zwconvRecvVerifyCodeDown(const ptree &, ptree & ptjc) {
		ZWFUNCTRACE
		    ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,
			     "Lock_Open_Ident");
	}

	void zwconvRecvVerifyCodeUp(const ptree & ptjc, ptree & ptccb) {
		try {
			OutputDebugStringA
			    ("20141017.1116.zwconvRecvVerifyCodeUp.MaHaoTest1");
			ZWFUNCTRACE ptccb.put(CCBSTR_CODE, "1002");
			OutputDebugStringA
			    ("20141017.1116.zwconvRecvVerifyCodeUp.MaHaoTest2");
			ptccb.put(CCBSTR_NAME, "SendUnLockIdent");
			OutputDebugStringA
			    ("20141017.1116.zwconvRecvVerifyCodeUp.MaHaoTest3");
			string zwDate, zwTime;
			zwGetLocalDateTimeString(time(NULL), zwDate, zwTime);
			ptccb.put(CCBSTR_DATE, zwDate);
			ptccb.put(CCBSTR_TIME, zwTime);
			//锁具发送初始闭锁码时，ATM编号应该已经在激活请求中获得，但是
			//1.1版本报文里面没有给出，所以此处可能会有问题
			OutputDebugStringA
			    ("20141017.1116.zwconvRecvVerifyCodeUp.MaHaoTest4");
			ptccb.put(CCBSTR_DEVCODE,
				  ptjc.get < string > ("Atm_Serial"));
			ptccb.put("root.LockMan", LOCKMAN_NAME);
			OutputDebugStringA
			    ("20141017.1116.zwconvRecvVerifyCodeUp.MaHaoTest5");
			ptccb.put("root.LockId",
				  ptjc.get < string > ("Lock_Serial"));
			OutputDebugStringA
			    ("20141017.1116.zwconvRecvVerifyCodeUp.MaHaoTest6");
			//关键的验证码本体
			ptccb.put("root.UnLockIdentInfo",
				  ptjc.get < int >("Lock_Ident_Info"));
			OutputDebugStringA
			    ("20141017.1116.zwconvRecvVerifyCodeUp.MaHaoTest7");
		}
		catch(...) {
			string errmsg =
			    "zwconvRecvVerifyCodeUp发生了异常，多半原因是JcEloc返回的Json中缺少某些关键字段比如Atm_Serial，Lock_Serial，Lock_Ident_Info";
			OutputDebugStringA(errmsg.c_str());
			pocoLog->error() << errmsg << endl;
		}
	}

}				//namespace jcAtmcConvertDLL{
