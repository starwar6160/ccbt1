#include "stdafx.h"
#include "CCBelock.h"

string & myAtmcMsgLockActive( string & strXML );

using namespace boost::property_tree;

//����ģ���ATMC XML��Ϣ������ڣ�����ö��������Ӧ��һ����XML��Ϣ
void zwAtmcMsgGen( const JC_MSG_TYPE type,string &strXML )
{
	switch (type)
	{
	case JCMSG_LOCK_ACTIVE_REQUEST:
		strXML = myAtmcMsgLockActive(strXML);
		break;
	}
	
}

//�������߼���XML����
string & myAtmcMsgLockActive( string & strXML )
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

	std::ostringstream ss;
	write_xml(ss,pt);
	strXML=ss.str();
	cout<<"ATMC XML Lock Active is"<<endl<<strXML<<endl;
	return strXML;
}
