#include "stdafx.h"
#include "CCBelock.h"
using namespace boost::property_tree;

int zwTestXML730(const char * atmcXML)
{
	cout<<"*****************"<<__FUNCTION__<<"*******************START"<<endl;
	cout<<"Demo on ATMC DLL \t"<<atmcXML<<endl;
	cout<<"*****************"<<__FUNCTION__<<"*******************END"<<endl;
//////////////////////////////////////////////////////////////////////////

	return 0;
}

const int zwXML2Json(const string &inXML,string &outJson)
{
	ptree pt;
	std::stringstream ss;
	ss<<inXML;
	read_xml(ss,pt);


	outJson="testdemostr";
	return JCMSG_LOCK_ACTIVE_REQUEST;
}