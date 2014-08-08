#include "stdafx.h"
#include "zwCcbElockHdr.h"
using namespace boost::property_tree;
//把锁具服务器端处理Json的函数集中于此，便于单元测试

const JC_MSG_TYPE lockParseInJson( const string & inJson, ptree &pt );
void LockGenOutputJson( const ptree &pt, string &outJson );
//内部函数，不必暴露给外界
void myLockActive(ptree &pt );
void myLockInit(ptree &ptIn );
void myReadCloseCode(ptree &pt );
void myGenInitCloseCodeJson( ptree &ptInOut );
void myGenVerifyCodeJson(ptree &ptInOut);

//此函数没有实际功能，功能都在下级子函数中，便于单元测试；此函数在此是满足联网的单元测试的；
int zwjclms_command_proc(const string &inJson,string &outJson)
{
	if (inJson.length()<=9)
	{
		return 0;
	}
	assert(inJson.length()>9);	//json最基本的符号起码好像要9个字符左右
	ZWTRACE("***模拟锁具收到的JSON请求开始***********************\n");
	ZWTRACE(inJson.c_str());
	ZWTRACE("***模拟锁具收到的JSON请求结束***********************\n");
	ptree pt;
	//解析输入JSON，并对其处理，结果在pt中，类型返回在mtype中
	JC_MSG_TYPE mtype=lockParseInJson(inJson, pt);
	//把pt输出为json返回
	LockGenOutputJson(pt, outJson);
	ZWTRACE("***模拟锁具生成的JSON应答开始***********************\n");
	ZWTRACE(outJson.c_str());
	ZWTRACE("***模拟锁具生成的JSON应答结束***********************\n");

	return mtype;
}



//进来的Json,对其分类，然后内容解析到pt中，并根据分类调用相应的底层函数进行组织相应的返回值的处理存入pt中，返回分类
const JC_MSG_TYPE lockParseInJson( const string & inJson, ptree &pt )
{

	JC_MSG_TYPE mtype=JCMSG_INVALID_TYPE;
	std::stringstream ss;
	ss<<inJson;
	read_json(ss,pt);
	ptree ptOut;

	try{
		string sCommand=pt.get<string>(jcAtmcConvertDLL::JCSTR_CMDTITLE);	
		if (jcAtmcConvertDLL::JCSTR_LOCK_ACTIVE_REQUEST==sCommand)
		{//是锁具激活请求，进行相关处理
			myLockActive(pt);
			return JCMSG_LOCK_ACTIVE_REQUEST;
		}
		if (jcAtmcConvertDLL::JCSTR_LOCK_INIT==sCommand)
		{//是锁具初始化请求，进行相关处理
			myLockInit(pt);
			return JCMSG_SEND_LOCK_ACTIVEINFO;
		}
		if (jcAtmcConvertDLL::JCSTR_READ_CLOSECODE==sCommand)
		{//是锁具初始化请求，进行相关处理
			myReadCloseCode(pt);
			return JCMSG_GET_CLOSECODE;
		}
		if (jcAtmcConvertDLL::JCSTR_SEND_INITCLOSECODE==sCommand)
		{//是测试用的要求锁具“主动”发送初始闭锁码的消息
			myGenInitCloseCodeJson(pt);
			return JCMSG_SEND_INITCLOSECODE;
		}
		if (jcAtmcConvertDLL::JCSTR_SEND_UNLOCK_CERTCODE==sCommand)
		{//是测试用的要求锁具“主动”发送初始闭锁码的消息
			myGenVerifyCodeJson(pt);
			return JCMSG_SEND_UNLOCK_CERTCODE;
		}
	}
	catch(...)
	{
		OutputDebugStringA("CPPEXECPTION804C");
		OutputDebugStringA(__FUNCTION__);
		cout<<"input json not found command item!20140801.1431"<<endl;
		return JCMSG_INVALID_TYPE;
	}


	return mtype;
}

//把处理完毕的pt中的内容输出为Json输出
void LockGenOutputJson( const ptree &pt, string &outJson )
{
	std::stringstream ss2;
	write_json(ss2,pt);
	outJson=ss2.str();
	assert(outJson.length()>9);
}


//锁具激活消息的具体处理函数。
void myLockActive(ptree &ptInOut )
{
	cout<<"锁具激活请求"<<endl;
	ptInOut.put("Lock_Serial","ZWFAKELOCKNO1631");
	ptInOut.put("Lock_Public_Key","BJpnccGKE5muLO3RLOe+hDjUftMJJwpmnuxEir0P3ss5/sxpEKNQ5AXcSsW1CbC/pXlqAk9/NZoquFJXHW3n1Cw=,");
	ptInOut.put("Lock_Time",time(NULL));

}

//锁具初始化的具体处理函数。
void myLockInit( ptree &ptInOut  )
{
	ptree ptOut;
	cout<<"锁具初始化"<<endl;
	ptOut.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,jcAtmcConvertDLL::JCSTR_LOCK_INIT);
	ptOut.put("Lock_Time",time(NULL));
	ptOut.put("Lock_Serial",ptInOut.get<string>("Atm_Serial"));
	ptOut.put("State","ok");
	//在此暂不计算，直接返回预先计算好的第一套密文数据里面的PSK
	ptOut.put("Lock_Init_Info","77498EB7D7CE8B92D871791C99B85AB337FF73235A89E7A20764EFE6EA41E4CE");
	ptInOut=ptOut;
}


//读取闭锁码消息的具体处理函数
void myReadCloseCode(ptree &ptInOut )
{
	cout<<"读取闭锁码"<<endl;
	//>> 锁具返回闭锁码至上位机
	//{
	//	"command": JCSTR_READ_CLOSECODE,
	//		JCSTR_READ_CLOSECODE: "12345678",  //uint64_t
	ptree ptOut;
	ptOut.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,jcAtmcConvertDLL::JCSTR_READ_CLOSECODE);
	ptOut.put("Lock_Time",time(NULL));
	ptOut.put("Lock_Serial","ZWFAKELOCKNO1548");	
	ptOut.put("Code","11112222");	
	ptInOut=ptOut;
}

//生成初始闭锁码报文
void myGenInitCloseCodeJson( ptree &ptInOut )
{
	ptree ptOut;
	ptOut.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,"Lock_Close_Code_Lock");
	ptOut.put("Lock_Time",time(NULL));
	ptOut.put("Lock_Serial","ZWFAKELOCKNO1548");	
	ptOut.put("Code","10007777");	
	ptInOut=ptOut;
}

//生成验证码报文
void myGenVerifyCodeJson(ptree &ptInOut)
{
	ptree ptOut;
	ptOut.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,"Lock_Open_Ident");
	ptOut.put("Lock_Time",time(NULL));
	ptOut.put("Lock_Serial","ZWFAKELOCKNO1548");	
	ptOut.put("Lock_Ident_Info","10028888");	
	ptInOut=ptOut;
}