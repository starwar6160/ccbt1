#include "stdafx.h"
using namespace boost::property_tree;

void zwAtmcMsgLockActive(string &strXML)
{
	ptree pt;
	pt.put("app.version", 140);
	pt.put("app.theme", "blue");
	pt.put("app.about.url", "http://www.jinchu.com.cn");
	pt.put("app.about.email", "jinchu_zhouwei@126.com");
	pt.put("app.about.content", "coryright (C) jinchu.com 2001-2014");
	std::ostringstream ss;
	write_xml(ss,pt);
	strXML=ss.str();
}