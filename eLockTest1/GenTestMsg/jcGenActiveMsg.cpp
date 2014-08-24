#include "stdafx.h"
#include "..\\eLockTest\\jcElockTestHdr.h"

namespace jcAtmcMsg{
	//生成锁具激活XML报文，获取锁具编号和公钥
	string & myAtmcMsgLockActive( string & strXML ,ptree &pt )
	{
		//请求
		//	交易代码	TransCode	是	值：0000
		//	交易名称	TransName	是	值：CallForActInfo
		//	交易日期	TransDate	是	值：YYYYMMDD，如20140401
		//	交易时间	TransTime	是	值：hhmmss，如134050
		//	ATM设备编号	DevCode	是	值：我行12位设备编号
		//开始生成请求报文
		pt.put(CCBSTR_CODE,"0000");
		pt.put(CCBSTR_NAME,"CallForActInfo");
		pt.put(CCBSTR_DATE,"20140730");
		pt.put(CCBSTR_TIME,"142248");
		pt.put(CCBSTR_DEVCODE,ATMNO_CCBTEST);
		//pt.put(CCBSTR_DEVCODE,"12345678");

		std::ostringstream ss;
		write_xml(ss,pt);
		strXML=ss.str();
		assert(strXML.length()>42);	//XML开头的固定内容38个字符，外加起码一个标签的两对尖括号合计4个字符
		return strXML;
		//应答
		//	交易代码	TransCode	是	值：0000
		//	交易名称	TransName	是	值：CallForActInfo
		//	交易日期	TransDate	是	值：YYYYMMDD，如20140401
		//	交易时间	TransTime	是	值：hhmmss，如134050
		//	ATM设备编号	DevCode	是	值：我行12位设备编号
		//	锁具厂商	LockMan	是	值：厂商自定与其他厂商不同的名称
		//	锁具编号	LockId	是	值：厂商自定的锁具唯一编号
		//	锁具公钥	LockPubKey	是	值：每把锁具生成的公私钥对中的公钥 16个字符
	}

	//生成发送锁具激活信息报文，发送用锁具公钥加密过后的PSK到锁具
	string & myAtmcMsgSendActiveInfo( string & strXML ,ptree &pt )
	{

		//请求
		//	交易代码	TransCode	是	值：0001
		//	交易名称	TransName	是	值：SendActInfo
		//	交易日期	TransDate	是	值：YYYYMMDD，如20140401
		//	交易时间	TransTime	是	值：hhmmss，如134050
		//	ATM设备编号	DevCode	是	值：我行12位设备编号
		//	锁具厂商	LockMan	是	值：厂商自定与其他厂商不同的名称
		//	锁具编号	LockId	是	值：厂商自定的锁具唯一编号
		//	激活信息	ActInfo	是	密码和加密服务器分散产生，经过锁具公钥加密的激活信息 长度96个字符
		//开始生成请求报文
		pt.put(CCBSTR_CODE,"0001");
		pt.put(CCBSTR_NAME,"SendActInfo");
		string zwDate,zwTime;
		zwGetLocalDateTimeString(time(NULL),zwDate,zwTime);

		pt.put(CCBSTR_DATE,zwDate);
		pt.put(CCBSTR_TIME,zwTime);
		pt.put(CCBSTR_DEVCODE,ATMNO_CCBTEST);
		pt.put("root.LockMan",jcAtmcMsg::G_LOCKMAN_NAMEG);
		pt.put("root.LockId","ZWFAKELOCKNO1548");
		//使用第一套激活信息,提供"ActInfo"的值
		pt.put("root.ActInfo","BG3j9JZxpssY0bdb1oMwg4obKmZ93GTvbbnY8VZnQIglLGO8m7JvhTlnvKPnsuBv"
			"1vAySJell1QrrkiMhsob1oc=.StjumFvZi4W4j2n/NUliN/72PY72IHjT.7tnYku3DAGnbsAas0+E98i"
			"Ql2mr+CoJbZcc2uQS3oVEFBbwtAgspY+oC+lSJcKI62395wPYkSG+F+Rd1Bj66CaVyBk8I7KvO/R+ofR5BeeM=");

		std::ostringstream ss;
		write_xml(ss,pt);
		strXML=ss.str();
		assert(strXML.length()>42);	//XML开头的固定内容38个字符，外加起码一个标签的两对尖括号合计4个字符
		return strXML;

		//应答
		//交易代码	TransCode	是	值：0001
		//交易名称	TransName	是	值：SendActInfo
		//交易日期	TransDate	是	值：YYYYMMDD，如20140401
		//交易时间	TransTime	是	值：hhmmss，如134050
		//ATM设备编号	DevCode	是	值：我行12位设备编号
		//锁具厂商	LockMan	是	值：厂商自定与其他厂商不同的名称
		//锁具编号	LockId	是	值：厂商自定的锁具唯一编号
		//激活标志	ActiveResult	是	值：0成功，1失败
		//激活信息	ActInfo	是	值：请求中的激活信息，96位，经过锁具解密，传到ATMVH，以后用于计算开锁密码，激活完成后提交闭锁码
	}




}	//namespace jcAtmcMsg{