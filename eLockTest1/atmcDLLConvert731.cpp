#include "stdafx.h"
#include "CCBelock.h"
using namespace boost::property_tree;
//把ATMC DLL的XML和JSON互转函数集中于此，便于单元测试；

const JC_MSG_TYPE zwXML2Json( const string &downXML,string &downJson )
{
	assert(downXML.length()>42);	//XML开头的固定内容38个字符，外加起码一个标签的两对尖括号合计4个字符
	cout<<"*********************ATMC Lock Active DOWN XML Start	*********************\n";
	cout<<downXML<<endl;
	cout<<"*********************ATMC Lock Active DOWN XML End	*********************\n";

	JC_MSG_TYPE msgType=JCMSG_INVALID_TYPE;
	ptree pt;
	std::stringstream ss;
	ss<<downXML;
	read_xml(ss,pt);
	string transCode=pt.get<string>("TransCode");
	if ("0000"==transCode)
	{
		msgType= JCMSG_LOCK_ACTIVE_REQUEST;
		//ptree pt2;
	}

	std::stringstream ss2;
	write_json(ss2,pt);
	downJson= ss2.str();
	cout<<"***ATMC DLL json down before convert by DLL Start********************\n"<<downJson;
	cout<<"***ATMC DLL json down before convert by DLL End**********************\n";
	return msgType;
}

const JC_MSG_TYPE zwJson2XML( const string &upJson,string &upXML )
{
	assert(upJson.length()>9);	//json最基本的符号起码好像要9个字符左右
	cout<<"***ATMC DLL json up before convert by DLL Start********************\n"<<upJson;
	cout<<"***ATMC DLL json up before convert by DLL End**********************\n";


	ptree pt;
	std::stringstream ss;
	ss<<upJson;
	read_json(ss,pt);
	std::stringstream ss2;
	write_xml(ss2,pt);
	upXML= ss2.str();

	cout<<"*********************ATMC Lock Active UP XML Start	*********************\n";
	cout<<upXML<<endl;
	cout<<"*********************ATMC Lock Active UP XML End	    *********************\n";

	string transCode=pt.get<string>("TransCode");
	if ("0000"==transCode)
	{
		return JCMSG_LOCK_ACTIVE_REQUEST;
	}
	return JCMSG_INVALID_TYPE;
}

namespace jcAtmcConvertDLL{



}	//namespace jcAtmcConvertDLL