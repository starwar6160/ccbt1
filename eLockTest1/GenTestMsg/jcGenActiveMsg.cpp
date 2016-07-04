#include "stdafx.h"
#include "..\\eLockTest\\jcElockTestHdr.h"
//#include "zwecies529.h"


extern string s_repActReqXML;	//从锁具收到的激活请求的返回报文
//从0号报文返回XML提取公钥并返回
string myGetPubKeyFromMsg0000Rep(const string msg0000RepXML);

namespace jcAtmcMsg {
	//生成锁具激活XML报文，获取锁具编号和公钥
	string & myAtmcMsgLockActive(string & strXML, ptree & pt) {
		//请求
		//      交易代码        TransCode       是      值：0000
		//      交易名称        TransName       是      值：CallForActInfo
		//      交易日期        TransDate       是      值：YYYYMMDD，如20140401
		//      交易时间        TransTime       是      值：hhmmss，如134050
		//      ATM设备编号     DevCode 是      值：我行12位设备编号
		//开始生成请求报文
		pt.put(CCBSTR_CODE, "0000");
		pt.put(CCBSTR_NAME, "CallForActInfo");
		string zwDate, zwTime;
		zwGetLocalDateTimeString(time(NULL),zwDate, zwTime);
		pt.put(CCBSTR_DATE, zwDate);
		pt.put(CCBSTR_TIME, zwTime);
		//金储此报文根本没有ATMNO字段，所以写死的值也不会有任何影响
		pt.put(CCBSTR_DEVCODE, ATMNO_CCBTEST);	

		std::ostringstream ss;
		write_xml(ss, pt);
		strXML = ss.str();
		assert(strXML.length() > 42);	//XML开头的固定内容38个字符，外加起码一个标签的两对尖括号合计4个字符
		return strXML;
		//应答
		//      交易代码        TransCode       是      值：0000
		//      交易名称        TransName       是      值：CallForActInfo
		//      交易日期        TransDate       是      值：YYYYMMDD，如20140401
		//      交易时间        TransTime       是      值：hhmmss，如134050
		//      ATM设备编号     DevCode 是      值：我行12位设备编号
		//      锁具厂商        LockMan 是      值：厂商自定与其他厂商不同的名称
		//      锁具编号        LockId  是      值：厂商自定的锁具唯一编号
		//      锁具公钥        LockPubKey      是      值：每把锁具生成的公私钥对中的公钥 16个字符
	}

//20150326，为了迅速调试姚工报告的USB拔掉线之后还能成功Open的问题暂时屏蔽掉这部分代码
//好像是因为ecies算法从zwBaseLib里面提取出来到了jclms DLL才导致的；
#ifdef _DEBUG326
	//生成发送锁具激活信息报文，发送用锁具公钥加密过后的PSK到锁具
	    string & myAtmcMsgSendActiveInfo(string & strXML, ptree & pt) {

		//请求
		//      交易代码        TransCode       是      值：0001
		//      交易名称        TransName       是      值：SendActInfo
		//      交易日期        TransDate       是      值：YYYYMMDD，如20140401
		//      交易时间        TransTime       是      值：hhmmss，如134050
		//      ATM设备编号     DevCode 是      值：我行12位设备编号
		//      锁具厂商        LockMan 是      值：厂商自定与其他厂商不同的名称
		//      锁具编号        LockId  是      值：厂商自定的锁具唯一编号
		//      激活信息        ActInfo 是      密码和加密服务器分散产生，经过锁具公钥加密的激活信息 长度96个字符
		//开始生成请求报文
		pt.put(CCBSTR_CODE, "0001");
		pt.put(CCBSTR_NAME, "SendActInfo");
		string zwDate, zwTime;
		zwGetLocalDateTimeString(time(NULL), zwDate, zwTime);
		pt.put(CCBSTR_DATE, zwDate);
		pt.put(CCBSTR_TIME, zwTime);
		pt.put(CCBSTR_DEVCODE, ATMNO_CCBTEST);
		pt.put("root.LockMan", jcAtmcMsg::G_LOCKMAN_NAMEG);
		pt.put("root.LockId", "ZWFAKELOCKNO1548");

		string pubKeyFromLock=myGetPubKeyFromMsg0000Rep(s_repActReqXML);
		assert(pubKeyFromLock.length()>0);
		string pskinput="zhouwei20140912.1014FAKEPSK"+zwDate+zwTime;
		string psk=zwMergePsk(pskinput.c_str());
		string actInfo=EciesEncrypt(pubKeyFromLock.c_str(),psk.c_str());
		assert(actInfo.length()>0);
		cout<<"PSK912 FROM ATMC IS "<<psk<<endl;
		cout<<"ACTINFO912\n"<<actInfo<<endl;
		pt.put("root.ActInfo",actInfo);

		std::ostringstream ss;
		write_xml(ss, pt);
		strXML = ss.str();
		assert(strXML.length() > 42);	//XML开头的固定内容38个字符，外加起码一个标签的两对尖括号合计4个字符
		return strXML;

		//应答
		//交易代码      TransCode       是      值：0001
		//交易名称      TransName       是      值：SendActInfo
		//交易日期      TransDate       是      值：YYYYMMDD，如20140401
		//交易时间      TransTime       是      值：hhmmss，如134050
		//ATM设备编号   DevCode 是      值：我行12位设备编号
		//锁具厂商      LockMan 是      值：厂商自定与其他厂商不同的名称
		//锁具编号      LockId  是      值：厂商自定的锁具唯一编号
		//激活标志      ActiveResult    是      值：0成功，1失败
		//激活信息      ActInfo 是      值：请求中的激活信息，96位，经过锁具解密，传到ATMVH，以后用于计算开锁密码，激活完成后提交闭锁码
	}
#endif // _DEBUG326

}				//namespace jcAtmcMsg{
