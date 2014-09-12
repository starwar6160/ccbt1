#include "stdafx.h"
#include "..\\eLockTest\\jcElockTestHdr.h"

namespace jcAtmcMsg {
//生成读取闭锁码报文
	string & myAtmcMsgReadCloseCodeInfo(string & strXML, ptree & pt) {
		//读取闭锁码 请求：
		//      交易代码        TransCode       是      值：0004
		//      交易名称        TransName       是      值：ReadShutLockCode
		//      交易日期        TransDate       是      值：YYYYMMDD，如20140401
		//      交易时间        TransTime       是      值：hhmmss，如134050
		//开始生成请求报文
		pt.put(CCBSTR_CODE, "0004");
		pt.put(CCBSTR_NAME, "ReadShutLockCode");
		string zwDate, zwTime;
		zwGetLocalDateTimeString(time(NULL),zwDate, zwTime);
		pt.put(CCBSTR_DATE, zwDate);
		pt.put(CCBSTR_TIME, zwTime);
		//建行给出的报文里面没有这个字段，但是会导致后续流程很难处理
		pt.put(CCBSTR_DEVCODE, ATMNO_CCBTEST);
		pt.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,
		       jcAtmcConvertDLL::JCSTR_READ_CLOSECODE);
		std::ostringstream ss;
		write_xml(ss, pt);
		strXML = ss.str();
		assert(strXML.length() > 42);	//XML开头的固定内容38个字符，外加起码一个标签的两对尖括号合计4个字符
		return strXML;

		//推送闭锁码 

		//读取闭锁码 应答
		//      交易代码        TransCode       是      值：0004
		//      交易名称        TransName       是      值：ReadShutLockCode
		//      交易日期        TransDate       是      值：YYYYMMDD，如20140401
		//      交易时间        TransTime       是      值：hhmmss，如134050
		//      ATM设备编号     DevCode 是      值：我行12位设备编号
		//      锁具厂商        LockMan 是      值：厂商自定与其他厂商不同的名称
		//      锁具编号        LockId  是      值：厂商自定的锁具唯一编号
		//      闭锁码  ShutLockcode    是      值：闭锁码

	}
//生成测试用的接收初始闭锁码报文，实际上建行并无此报文，只是为了形成一问一答方便测试
	    string & myTestMsgRecvCloseCode(string & strXML, ptree & pt) {
		//开始生成请求报文
		pt.put(CCBSTR_CODE, "1000");
		pt.put(CCBSTR_NAME, "SendShutLockCode");
		std::ostringstream ss;
		write_xml(ss, pt);
		strXML = ss.str();
		return strXML;
	}
}				//namespace jcAtmcMsg{
