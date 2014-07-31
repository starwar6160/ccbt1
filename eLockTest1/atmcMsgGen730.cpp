#include "stdafx.h"
#include "CCBelock.h"
//把ATMC端的XML生成与结果XML解析集中于此，便于单元测试


using namespace boost::property_tree;
namespace jcAtmcMsg{
string & myAtmcMsgLockActive( string & strXML );
//生成模拟的ATMC XML消息的总入口，根据枚举生成相应那一条的XML消息
void zwAtmcMsgGen( const JC_MSG_TYPE type,string &strXML )
{
	switch (type)
	{
	case JCMSG_LOCK_ACTIVE_REQUEST:
		strXML = myAtmcMsgLockActive(strXML);
		assert(strXML.length()>42);	//XML开头的固定内容38个字符，外加起码一个标签的两对尖括号合计4个字符
		break;
	}
	
}

//生成锁具激活XML报文
string & myAtmcMsgLockActive( string & strXML )
{
	
	ptree pt;
	pt.put("TransCode","0000");
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
	assert(strXML.length()>42);	//XML开头的固定内容38个字符，外加起码一个标签的两对尖括号合计4个字符
	return strXML;
}

}	//namespace jcAtmcMsg