#include "stdafx.h"
#include "CCBelock.h"
using namespace boost::property_tree;
//把ATMC DLL的XML和JSON互转函数集中于此，便于单元测试；
namespace jcAtmcConvertDLL{
	const char *LOCKMAN_NAME="BeiJing.JinChu";
	//下发方向处理
	void zwconvLockActiveDown(const ptree &ptccb, ptree &ptjc );
	//上传方向处理
	void zwconvLockActiveUp(const ptree &ptjc, ptree &ptccb );

const JC_MSG_TYPE zwCCBxml2JCjson( const string &downXML,string &downJson )
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
	//从建行的接口所需字段变为我们的JSON接口
	ptree pt2;
	if ("0000"==transCode)
	{//锁具激活
		msgType= JCMSG_LOCK_ACTIVE_REQUEST;
		zwconvLockActiveDown(pt,pt2);
	}
	//处理结果输出为Json供下位机使用
	std::stringstream ss2;
	write_json(ss2,pt2);
	downJson= ss2.str();
	cout<<"***ATMC DLL json down after convert by DLL Start********************\n"<<downJson;
	cout<<"***ATMC DLL json down after convert by DLL End**********************\n";

	return msgType;
}

const JC_MSG_TYPE zwJCjson2CCBxml( const string &upJson,string &upXML )
{
	//从下位机接收而来的json结果字符串，解码为中间形式ptree
	assert(upJson.length()>9);	//json最基本的符号起码好像要9个字符左右
	ptree ptjc;
	std::stringstream ss;
	ss<<upJson;
	read_json(ss,ptjc);
	//判断消息类型并从我们的JSON接口变为建行的接口所需字段
	string jcCmd=ptjc.get<string>("command");
	ptree ptccb;
	if ("Lock_Secretkey"==jcCmd)
	{
		zwconvLockActiveUp(ptjc,ptccb);
	}

//////////////////////////////////////////////////////////////////////////
	
	std::stringstream sst1;
	write_json(sst1,ptccb);
	string jsonCcb=sst1.str();
//////////////////////////////////////////////////////////////////////////
	cout<<"***ATMC DLL jsonCcb after convert by DLL Start********************\n"<<jsonCcb;
	cout<<"***ATMC DLL jsonCcb after convert by DLL End**********************\n";

	//转换JSON为XML供ATMC使用
	std::stringstream ss2;
	write_xml(ss2,ptccb);
	upXML= ss2.str();

	cout<<"*********************ATMC Lock Active UP XML Start	*********************\n";
	cout<<upXML<<endl;
	cout<<"*********************ATMC Lock Active UP XML End	    *********************\n";

	string transCode=ptccb.get<string>("TransCode");
	if ("0000"==transCode)
	{
		return JCMSG_LOCK_ACTIVE_REQUEST;
	}
	return JCMSG_INVALID_TYPE;
}

////////////////////////////每一条报文的具体处理函数开始//////////////////////////////////////////////

//下发方向处理
void zwconvLockActiveDown( const ptree &ptccb, ptree &ptjc )
{
	ptjc.put("command","Lock_Secretkey");
	ptjc.put("State","get");
	ptjc.put("Public_Key","123456");	//该行其实无意义，但是json接口里面有，就写上
}

//上传方向处理
void zwconvLockActiveUp( const ptree &ptjc, ptree &ptccb )
{
	//无用的形式化部分,未完
	ptccb.put("TransCode","0000");
	//有用部分
	ptccb.put("LockId",ptjc.get<string>("Lock_Serial"));
	ptccb.put("LockPubKey",ptjc.get<string>("Public_Key"));	
	ptccb.put("LockMan",LOCKMAN_NAME);
}
////////////////////////////每一条报文的具体处理函数结束//////////////////////////////////////////////
}	//namespace jcAtmcConvertDLL