#include "stdafx.h"
#include "..\\eLockTest\\jcElockTestHdr.h"

namespace jcAtmcMsg{

	//生成测试用的0003时间同步报文，目的在于模拟建行ATM机器的行为
	string & myTestMsgTimeSync( string & strXML ,ptree &pt )
	{
		//开始生成请求报文
		pt.put(CCBSTR_CODE,"0003");
		pt.put(CCBSTR_NAME,"TimeSync");
		string zwDate,zwTime;
		zwGetLocalDateTimeString(time(NULL),zwDate,zwTime);
		pt.put(CCBSTR_DATE,zwDate);
		pt.put(CCBSTR_TIME,zwTime);
		std::ostringstream ss;
		write_xml(ss,pt);
		strXML=ss.str();
		return strXML;
	}

}	//namespace jcAtmcMsg{