#include "stdafx.h"
#include "CCBelock.h"
//��ATMC�˵�XML��������XML���������ڴˣ����ڵ�Ԫ����


using namespace boost::property_tree;
namespace jcAtmcMsg{
	string & myAtmcMsgLockActive( string & strXML ,ptree &pt);
//����ģ���ATMC XML��Ϣ������ڣ�����ö��������Ӧ��һ����XML��Ϣ
void zwAtmcMsgGen( const JC_MSG_TYPE type,string &strXML,ptree &pt )
{
	switch (type)
	{
	case JCMSG_LOCK_ACTIVE_REQUEST:
		strXML = myAtmcMsgLockActive(strXML, pt);
		assert(strXML.length()>42);	//XML��ͷ�Ĺ̶�����38���ַ����������һ����ǩ�����Լ����źϼ�4���ַ�
		break;
	}
	
}

//�������߼���XML����
string & myAtmcMsgLockActive( string & strXML ,ptree &pt )
{
	
	//ptree pt;
	pt.put("TransCode","0000");
	pt.put("TransName","CallForActInfo");
	pt.put("TransDate","20140730");
	pt.put("TransTime","142248");
	pt.put("DevCode","CCBDEV123456");
	pt.put("SpareString1","CCBReverse1");
	pt.put("SpareString2","CCBReverse2");

	std::ostringstream ss;
	write_xml(ss,pt);
	strXML=ss.str();
	assert(strXML.length()>42);	//XML��ͷ�Ĺ̶�����38���ַ����������һ����ǩ�����Լ����źϼ�4���ַ�
	return strXML;
}

}	//namespace jcAtmcMsg