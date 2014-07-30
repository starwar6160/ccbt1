#include "stdafx.h"
using namespace boost::property_tree;

void zwAtmcMsgLockActive(string &strXML)
{
	ptree pt;
	pt.put("TransCode","0000");
	pt.put("TransType",0);
	pt.put("TransName","CallForActInfo");
	pt.put("TransDate","20140730");
	pt.put("TransTime","142203");
	pt.put("DevCode","CCBDEV123456");
	pt.put("SpareString1","CCBReverse1");
	pt.put("SpareString2","CCBReverse2");

	pt.put("app.version", 140);
	pt.put("app.theme", "blue");
	pt.put("app.about.url", "http://www.jinchu.com.cn");
	pt.put("app.about.email", "jinchu_zhouwei@126.com");
	pt.put("app.about.content", "coryright (C) jinchu.com 2001-2014");
	std::ostringstream ss;
	write_xml(ss,pt);
	strXML=ss.str();
}