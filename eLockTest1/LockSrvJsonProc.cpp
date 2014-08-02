#include "stdafx.h"
#include "zwCcbElockHdr.h"
using namespace boost::property_tree;
//把锁具服务器端处理Json的函数集中于此，便于单元测试

const JC_MSG_TYPE lockParseJson( const string & inJson, ptree &pt );
void LockOutJson( const ptree &pt, string &outJson );
//内部函数，不必暴露给外界
void myLockActive(ptree &pt );
void myLockInit(ptree &pt );

//此函数没有实际功能，功能都在下级子函数中，便于单元测试；此函数在此是满足联网的单元测试的；
int zwjclms_command_proc(const string &inJson,string &outJson)
{
	if (inJson.length()<=9)
	{
		return 0;
	}
	assert(inJson.length()>9);	//json最基本的符号起码好像要9个字符左右
	cout<<__FUNCTION__<<"***EMUSRV IN JSON START***********************\n";
	cout<<inJson;
	cout<<"***EMUSRV IN JSON END  ***********************\n";
	ptree pt;
	//解析输入JSON，并对其处理，结果在pt中，类型返回在mtype中
	JC_MSG_TYPE mtype=lockParseJson(inJson, pt);
	//把pt输出为json返回
	LockOutJson(pt, outJson);
	cout<<__FUNCTION__<<"***EMUSRV OUT JSON START***********************\n";
	cout<<outJson;
	cout<<"***EMUSRV OUT JSON END  ***********************\n";

	return mtype;
}



//进来的Json,对其分类，然后内容解析到pt中，并根据分类调用相应的底层函数进行组织相应的返回值的处理存入pt中，返回分类
const JC_MSG_TYPE lockParseJson( const string & inJson, ptree &pt )
{

	JC_MSG_TYPE mtype=JCMSG_INVALID_TYPE;
	std::stringstream ss;
	ss<<inJson;
	read_json(ss,pt);

	try{
		string sCommand=pt.get<string>("command");	
		if ("Lock_Secretkey"==sCommand)
		{//是锁具激活请求，进行相关处理
			myLockActive(pt);
			return JCMSG_LOCK_ACTIVE_REQUEST;
		}
		if ("Lock_Init"==sCommand)
		{//是锁具初始化请求，进行相关处理
			myLockInit(pt);
			return JCMSG_SEND_LOCK_ACTIVEINFO;
		}

	}
	catch(...)
	{
		cout<<"input json not found command item!20140801.1431"<<endl;
		return JCMSG_INVALID_TYPE;
	}


	return mtype;
}

//把处理完毕的pt中的内容输出为Json输出
void LockOutJson( const ptree &pt, string &outJson )
{
	std::stringstream ss2;
	write_json(ss2,pt);
	outJson=ss2.str();
	assert(outJson.length()>9);
}


//锁具激活消息的具体处理函数。
void myLockActive(ptree &pt )
{
	//pt.put("LockMan","BeiJing JinChu");
	pt.put("Lock_Serial","ZWFAKELOCKNO1548");
	pt.put("Public_Key","BJpnccGKE5muLO3RLOe+hDjUftMJJwpmnuxEir0P3ss5/sxpEKNQ5AXcSsW1CbC/pXlqAk9/NZoquFJXHW3n1Cw=,");
	pt.put("State","get");

}

//锁具初始化的具体处理函数。
void myLockInit(ptree &pt )
{
	ptree pt2;
	pt2.put("command","Lock_Init");
	pt2.put("State","ok");
	pt=pt2;
}