#include "stdafx.h"
#include "zwCcbElockHdr.h"
//把ATMC端的XML生成与结果XML解析集中于此，便于单元测试
//第一套激活信息
//CCB 1.1版本算法ECIES(椭圆曲线集成加密公钥算法)安全初始化演示开始
//	ECIES PubKey=   BK4gbEtRRx+W3CVn96/V0zYzZOnhRrBAYGgNgBeRVRnWz1vaU4i4b0GdvA/yVR1i
//	JBEHmHyDBN8W93f+1kIx4jM=,
//	ECIES Prikey=   lI+akxkuWIkTq4yphAG5h72I0yTNQr25W9lHavJOCMM=
//PSK:
//77498EB7D7CE8B92D871791C99B85AB337FF73235A89E7A20764EFE6EA41E4CE
//cryptText:      BG3j9JZxpssY0bdb1oMwg4obKmZ93GTvbbnY8VZnQIglLGO8m7JvhTlnvKPnsuBv
//				1vAySJell1QrrkiMhsob1oc=.StjumFvZi4W4j2n/NUliN/72PY72IHjT.7tnYku3DAGnbsAas0+E98i
//				Ql2mr+CoJbZcc2uQS3oVEFBbwtAgspY+oC+lSJcKI62395wPYkSG+F+Rd1Bj66CaVyBk8I7KvO/R+ofR
//				5BeeM=
//decryptPSK:
//77498EB7D7CE8B92D871791C99B85AB337FF73235A89E7A20764EFE6EA41E4CE

//同样的PSK，另一组公钥私钥
//CCB 1.1版本算法ECIES(椭圆曲线集成加密公钥算法)安全初始化演示开始
//	ECIES PubKey=   BAnTxOLq5q+bEPJlJJ8uvJf5mC6j4z5DkdCssb+7zSvjeZPuNHnyvd4yPoY1As5T
//	H9YBtRtCqtULO61+X2L8miM=,
//	ECIES Prikey=   te1ieS34jNV+pAZ1yRl8bPtSTyM/86sADoDSKIJtUlM=
//PSK:
//77498EB7D7CE8B92D871791C99B85AB337FF73235A89E7A20764EFE6EA41E4CE
//cryptText:      BHxPVBhRdyjSHRnj01c/d3VSRFLXcgwHQ4v7jmazmGIcQrv9snTBaw3J4R+9Kl/7
//				dQInhtkb5f/R0a9GoIlicDk=.7yR0xGdEn13xO0cxv4ulCcNIZ4mrteYi.0pmz+bQNIizFIaOM/n27B3
//				xHQlR9z8jFiB6Pm8Qapm/J9Fsh4GyeXBeeSAgT2gIsgAxr7y3Wk6ZkCX2x1qj6sMAlpTW4dN0/bLbSM5
//				0EMTY=
//decryptPSK:
//77498EB7D7CE8B92D871791C99B85AB337FF73235A89E7A20764EFE6EA41E4CE

using namespace boost::property_tree;
namespace jcAtmcMsg{
	const char *ATMNO_CCBTEST="CCBATMNO1234";
	string & myAtmcMsgLockActive( string & strXML ,ptree &pt);
	string & myAtmcMsgSendActiveInfo( string & strXML ,ptree &pt );
	string & myAtmcMsgReadCloseCodeInfo( string & strXML ,ptree &pt );
//生成模拟的ATMC XML消息的总入口，根据枚举生成相应那一条的XML消息
void zwAtmcMsgGen( const JC_MSG_TYPE type,string &strXML,ptree &pt )
{
	switch (type)
	{
	case JCMSG_LOCK_ACTIVE_REQUEST:
		strXML = myAtmcMsgLockActive(strXML, pt);
		assert(strXML.length()>42);	//XML开头的固定内容38个字符，外加起码一个标签的两对尖括号合计4个字符
		break;
	case JCMSG_SEND_LOCK_ACTIVEINFO:
		strXML=myAtmcMsgSendActiveInfo(strXML,pt);
		assert(strXML.length()>42);
		break;
	case JCMSG_GET_CLOSECODE:
		strXML=myAtmcMsgReadCloseCodeInfo(strXML,pt);
		assert(strXML.length()>42);
		break;
	}
}
	


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
	pt.put("TransCode","0000");
	pt.put("TransName","CallForActInfo");
	pt.put("TransDate","20140730");
	pt.put("TransTime","142248");
	pt.put("DevCode",ATMNO_CCBTEST);

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
	//	密码和加密服务器公钥	PswSrvPubKey	是	密码和加密服务器的公钥，用于通信加密和验签
	//开始生成请求报文
	pt.put("TransCode","0001");
	pt.put("TransName","SendActInfo");
	pt.put("TransDate","20140802");
	pt.put("TransTime","120845");
	pt.put("DevCode",ATMNO_CCBTEST);
	pt.put("LockMan",jcAtmcConvertDLL::LOCKMAN_NAME);
	pt.put("LockId","ZWFAKELOCKNO1548");
	//使用第一套激活信息,提供"ActInfo"和"PswSrvPubKey"的值
	pt.put("ActInfo","BG3j9JZxpssY0bdb1oMwg4obKmZ93GTvbbnY8VZnQIglLGO8m7JvhTlnvKPnsuBv"
	"1vAySJell1QrrkiMhsob1oc=.StjumFvZi4W4j2n/NUliN/72PY72IHjT.7tnYku3DAGnbsAas0+E98i"
	"Ql2mr+CoJbZcc2uQS3oVEFBbwtAgspY+oC+lSJcKI62395wPYkSG+F+Rd1Bj66CaVyBk8I7KvO/R+ofR5BeeM=");
	pt.put("PswSrvPubKey",	"BK4gbEtRRx+W3CVn96/V0zYzZOnhRrBAYGgNgBeRVRnWz1vaU4i4b0GdvA/yVR1i"
							"JBEHmHyDBN8W93f+1kIx4jM=,");

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


//生成读取闭锁码报文
string & myAtmcMsgReadCloseCodeInfo( string & strXML ,ptree &pt )
{
	//读取闭锁码 请求：
	//	交易代码	TransCode	是	值：0004
	//	交易名称	TransName	是	值：ReadShutLockCode
	//	交易日期	TransDate	是	值：YYYYMMDD，如20140401
	//	交易时间	TransTime	是	值：hhmmss，如134050
	//开始生成请求报文
	pt.put("TransCode","0004");
	pt.put("TransName","ReadShutLockCode");
	pt.put("TransDate","20140802");
	pt.put("TransTime","140826");
	//建行给出的报文里面没有这个字段，但是会导致后续流程很难处理
	pt.put("DevCode",ATMNO_CCBTEST);
	pt.put("command",jcAtmcConvertDLL::JCSTR_READ_CLOSECODE);
	std::ostringstream ss;
	write_xml(ss,pt);
	strXML=ss.str();
	assert(strXML.length()>42);	//XML开头的固定内容38个字符，外加起码一个标签的两对尖括号合计4个字符
	return strXML;
	
	
	//推送闭锁码 

	//读取闭锁码 应答
	//	交易代码	TransCode	是	值：0004
	//	交易名称	TransName	是	值：ReadShutLockCode
	//	交易日期	TransDate	是	值：YYYYMMDD，如20140401
	//	交易时间	TransTime	是	值：hhmmss，如134050
	//	ATM设备编号	DevCode	是	值：我行12位设备编号
	//	锁具厂商	LockMan	是	值：厂商自定与其他厂商不同的名称
	//	锁具编号	LockId	是	值：厂商自定的锁具唯一编号
	//	闭锁码	ShutLockcode	是	值：闭锁码

}




}	//namespace jcAtmcMsg