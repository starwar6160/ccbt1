#include "stdafx.h"
#include "CCBelock.h"
using namespace boost::property_tree;

int zwTestXML730(const char * atmcXML)
{
	cout<<"*****************"<<__FUNCTION__<<"*******************START"<<endl;
	cout<<"Demo on ATMC DLL \t"<<atmcXML<<endl;
	cout<<"*****************"<<__FUNCTION__<<"*******************END"<<endl;

	return 0;
}

const int zwXML2Json(const string &inXML,string &outJson)
{
	ptree pt;
	std::stringstream ss;
	ss<<inXML;
	read_xml(ss,pt);
	std::stringstream ss2;
	write_json(ss2,pt);
	outJson= ss2.str();
	cout<<"VERSION730:"<<pt.get<int>("app.version")<<endl;
	return JCMSG_LOCK_ACTIVE_REQUEST;
}