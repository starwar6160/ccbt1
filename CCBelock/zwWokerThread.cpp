#include "stdafx.h"
#include "CCBelock.h"
using namespace boost::property_tree;

const int zwXML2Json(const string &inXML,string &outJson)
{
	assert(inXML.length()>42);	//XML开头的固定内容38个字符，外加起码一个标签的两对尖括号合计4个字符
	int status=JCMSG_INVALID_TYPE;
	ptree pt;
	std::stringstream ss;
	ss<<inXML;
	read_xml(ss,pt);
	string transCode=pt.get<string>("TransCode");
	if ("0000"==transCode)
	{
		status= JCMSG_LOCK_ACTIVE_REQUEST;
		//ptree pt2;
	}

	std::stringstream ss2;
	write_json(ss2,pt);
	outJson= ss2.str();
	return status;
}

const int zwJson2XML(const string &inJson,string &outXML)
{
	assert(inJson.length()>9);	//json最基本的符号起码好像要9个字符左右
	ptree pt;
	std::stringstream ss;
	ss<<inJson;
	read_json(ss,pt);
	std::stringstream ss2;
	write_xml(ss2,pt);
	outXML= ss2.str();
	string transCode=pt.get<string>("TransCode");
	if ("0000"==transCode)
	{
		return JCMSG_LOCK_ACTIVE_REQUEST;
	}

	return JCMSG_INVALID_TYPE;
}