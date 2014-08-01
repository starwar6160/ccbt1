#include "stdafx.h"
#include "CCBelock.h"
using namespace boost::property_tree;
//把ATMC DLL的XML和JSON互转函数集中于此，便于单元测试；
namespace jcAtmcConvertDLL{
	const char *LOCKMAN_NAME="BeiJing.JinChu";

	void zwconvLockActive( ptree &pt2 );
const JC_MSG_TYPE zwXML2Json( const string &downXML,string &downJson )
{
	//接受ATMC下发的XML，转化为中间形式ptree
	assert(downXML.length()>42);	//XML开头的固定内容38个字符，外加起码一个标签的两对尖括号合计4个字符
	cout<<"*********************ATMC Lock Active DOWN XML Start	*********************\n";
	cout<<downXML<<endl;
	cout<<"*********************ATMC Lock Active DOWN XML End	*********************\n";

	JC_MSG_TYPE msgType=JCMSG_INVALID_TYPE;
	ptree pt;
	std::stringstream ss;
	ss<<downXML;
	read_xml(ss,pt);
	//判断消息类型
	string transCode=pt.get<string>("TransCode");
	//根据消息类型调用不同函数处理		
	ptree pt2;
	if ("0000"==transCode)
	{//锁具激活
		msgType= JCMSG_LOCK_ACTIVE_REQUEST;
		zwconvLockActive(pt2);
	}
	//处理结果输出为Json供下位机使用
	std::stringstream ss2;
	write_json(ss2,pt2);
	downJson= ss2.str();
	cout<<"***ATMC DLL json down after convert by DLL Start********************\n"<<downJson;
	cout<<"***ATMC DLL json down after convert by DLL End**********************\n";

	return msgType;
}

const JC_MSG_TYPE zwJson2XML( const string &upJson,string &upXML )
{
	//从下位机接收而来的json结果字符串，解码为中间形式ptree
	assert(upJson.length()>9);	//json最基本的符号起码好像要9个字符左右
	ptree pt;
	std::stringstream ss;
	ss<<upJson;
	read_json(ss,pt);
//////////////////////////////////////////////////////////////////////////
	pt.put("TransCode","0000");
	pt.put("LockMan",LOCKMAN_NAME);
	pt.erase("State");
	std::stringstream sst1;
	write_json(sst1,pt);
	string afterJson=sst1.str();
//////////////////////////////////////////////////////////////////////////
	cout<<"***ATMC DLL json up after convert by DLL Start********************\n"<<afterJson;
	cout<<"***ATMC DLL json up after convert by DLL End**********************\n";

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

void zwconvLockActive( ptree &pt2 )
{
	pt2.put("command","Lock_Secretkey");
	pt2.put("State","get");
	pt2.put("Public_Key","123456");	//该行其实无意义，但是json接口里面有，就写上
}

}	//namespace jcAtmcConvertDLL