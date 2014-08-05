#include "stdafx.h"
#include "zwCcbElockHdr.h"



using namespace boost::property_tree;
//把ATMC DLL的XML和JSON互转函数集中于此，便于单元测试；
namespace jcAtmcConvertDLL{
	const char *LOCKMAN_NAME="BeiJing.JinChu";
	//下发方向处理
	void zwconvLockActiveDown(const ptree &ptccb, ptree &ptjc );
	void zwconvLockInitDown( const ptree &ptccb, ptree &ptjc );
	void zwconvReadCloseCodeDown( const ptree &ptccb, ptree &ptjc );
	//上传方向处理
	void zwconvLockActiveUp(const ptree &ptjc, ptree &ptccb );
	void zwconvLockInitUp( const ptree &ptjc, ptree &ptccb );
	void zwconvReadCloseCodeUp( const ptree &ptjc, ptree &ptccb );
	//以下4个字段，为的是在上下转换期间保存建行报文中冗余的，我们基本不用但又必须返回给建行的字段
	string ns_ActReqName;
	string ns_LockInitName;
	string ns_ReadCloseCodeName;
	string ns_ccbAtmno;		//ATM编号


	const JC_MSG_TYPE zwCCBxml2JCjson( const string &downXML,string &downJson )
	{
		//接受ATMC下发的XML，转化为中间形式ptree
		assert(downXML.length()>42);	//XML开头的固定内容38个字符，外加起码一个标签的两对尖括号合计4个字符
		cout<<"******************建行ATMC下发XML开始*********************\n";
		cout<<downXML<<endl;
		cout<<"******************建行ATMC下发XML结束*********************\n";

		JC_MSG_TYPE msgType=JCMSG_INVALID_TYPE;
		ptree pt;
		std::stringstream ss;
		ss<<downXML;
		read_xml(ss,pt);
		//////////////////////////////////////////////////////////////////////////

		std::stringstream ssccb;
		write_json(ssccb,pt);
		string ccbJson= ssccb.str();
		cout<<"***建行XML转换后的JSON开始********************************\n"<<ccbJson;
		cout<<"***建行XML转换后的JSON结束********************************\n";


		//判断消息类型
		string transCode=pt.get<string>("TransCode");
		//保存建行冗余字段以便上传返回时提供给建行		
		ns_ccbAtmno=pt.get<string>("DevCode");
		//根据消息类型调用不同函数处理	
		//从建行的接口所需字段变为我们的JSON接口
		ptree pt2;
		if ("0000"==transCode)
		{//锁具激活请求
		ns_ActReqName=pt.get<string>("TransName");
			msgType= JCMSG_LOCK_ACTIVE_REQUEST;
			zwconvLockActiveDown(pt,pt2);
		}
		if ("0001"==transCode)
		{//发送锁具激活信息(初始化)
			ns_LockInitName=pt.get<string>("TransName");
			msgType= JCMSG_SEND_LOCK_ACTIVEINFO;
			zwconvLockInitDown(pt,pt2);
		}
		if ("0004"==transCode)
		{//读取闭锁码
			ns_ReadCloseCodeName=pt.get<string>("TransName");
			msgType= JCMSG_GET_CLOSECODE;
			zwconvReadCloseCodeDown(pt,pt2);
		}
		//处理结果输出为Json供下位机使用
		std::stringstream ss2;
		write_json(ss2,pt2);
		downJson= ss2.str();
		cout<<"***金储JSON下发请求开始***********************************\n"<<downJson;
		cout<<"***金储JSON下发请求结束***********************************\n";

		return msgType;
	}

	const JC_MSG_TYPE zwJCjson2CCBxml( const string &upJson,string &upXML )
	{
		//从下位机接收而来的json结果字符串，解码为中间形式ptree
		assert(upJson.length()>9);	//json最基本的符号起码好像要9个字符左右
		ptree ptJC;
		std::stringstream ss;
		ss<<upJson;
		read_json(ss,ptJC);
		//////////////////////////////////////////////////////////////////////////
		std::stringstream sst1;
		write_json(sst1,ptJC);
		string jsonJc= sst1.str();
		cout<<"***金储锁具返回的JSON应答开始*****************************\n"<<jsonJc;
		cout<<"***金储锁具返回的JSON应答结束*****************************\n";


		//判断消息类型并从我们的JSON接口变为建行的接口所需字段
		string jcCmd=ptJC.get<string>(jcAtmcConvertDLL::JCSTR_CMDTITLE);
		ptree ptCCB;
		ptCCB=ptJC;	//如果不符合以下任何一条，则保持原样
		if (JCSTR_LOCK_ACTIVE_REQUEST==jcCmd)
		{//发送锁具激活请求
			zwconvLockActiveUp(ptJC,ptCCB);
		}
		if (JCSTR_LOCK_INIT==jcCmd)
		{//发送锁具激活信息(锁具初始化)
			zwconvLockInitUp(ptJC,ptCCB);
		}
		if (JCSTR_READ_CLOSECODE==jcCmd)
		{//读取闭锁码
			zwconvReadCloseCodeUp(ptJC,ptCCB);
		}

		//////////////////////////////////////////////////////////////////////////

		std::stringstream sst2;
		write_json(sst2,ptCCB);
		string jsonCcb=sst2.str();
		//////////////////////////////////////////////////////////////////////////
		cout<<"***金储锁具应答JSON处理为建行所需元素 开始****************\n"<<jsonCcb;
		cout<<"***金储锁具应答JSON处理为建行所需元素 结束****************\n";

		//转换JSON为XML供ATMC使用
		std::stringstream ss2;
		write_xml(ss2,ptCCB);
		upXML= ss2.str();

		cout<<"*********************金储应答XML开始**********************\n";
		cout<<upXML<<endl;
		cout<<"*********************金储应答XML结束**********************\n";

		string transCode=ptCCB.get<string>("TransCode");
		if ("0000"==transCode)
		{
			return JCMSG_LOCK_ACTIVE_REQUEST;
		}
		return JCMSG_INVALID_TYPE;
	}

	////////////////////////////每一条报文的具体处理函数开始//////////////////////////////////////////////

	//发送锁具激活请求
	void zwconvLockActiveDown( const ptree &ptccb, ptree &ptjc )
	{	
		ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,JCSTR_LOCK_ACTIVE_REQUEST);
//		ptjc.put("State","get");
//		ptjc.put("Public_Key","123456");	//该行其实无意义，但是json接口里面有，就写上
	}


	void zwconvLockActiveUp( const ptree &ptjc, ptree &ptccb )
	{
		//无用的形式化部分
		ptccb.put("TransCode","0000");
		assert("CallForActInfo"==ns_ActReqName);
		ptccb.put("TransName",ns_ActReqName);
		ptccb.put("DevCode",ns_ccbAtmno);
//////////////////////////////////////////////////////////////////////////
		string zwDate,zwTime;
		zwGetDateTimeString(ptjc.get<time_t>("Lock_Time"),zwDate,zwTime);
		ptccb.put("TransDate",zwDate);
		ptccb.put("TransTime",zwTime);

//////////////////////////////////////////////////////////////////////////
		//有用部分
		
		ptccb.put("LockId",ptjc.get<string>("Lock_Serial"));
		ptccb.put("LockPubKey",ptjc.get<string>("Lock_Public_Key"));	
		ptccb.put("LockMan",LOCKMAN_NAME);
	}
	//////////////////////////////////////////////////////////////////////////

	//发送锁具激活信息(锁具初始化)
	void zwconvLockInitDown( const ptree &ptccb, ptree &ptjc )
	{	
		//锁具初始化

		//>> 上位机下发
		//	"command": JCSTR_LOCK_INIT,
		//		"Lock_Time"
		//		"Atm_Serial"
		//		"Lock_Init_Info":"ECIES加密过的PSK"

		//>> 锁具正确接收后，返回
		//	"command": JCSTR_LOCK_INIT,
		//	"Lock_Time"
		//	"Lock_Serial"
		//	"State"
		//	"Lock_Init_Info":"锁具ECIES解密出来的PSK明文,用于ATMVH存档"

		ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,JCSTR_LOCK_INIT);
		//以下字段(LMS时间)是JC特有，CCB没有,时间或许还应该用建行报文中的时间来转换
		ptjc.put("Lock_Time",time(NULL));
		ptjc.put("Atm_Serial",ptccb.get<string>("DevCode"));	
		//此处1.1版本已经支持ECIES加密过的PSK输入了，使用第一套密文
		ptjc.put("Lock_Init_Info",ptccb.get<string>("ActInfo"));
		//以下字段是CCB有，JC没有
		ptjc.put("PswSrvPubKey",ptccb.get<string>("PswSrvPubKey"));
		
	}

	void zwconvLockInitUp( const ptree &ptjc, ptree &ptccb )
	{
		//无用的形式化部分
		ptccb.put("TransCode","0001");
		ptccb.put("TransName",ns_LockInitName);	//使用缓存在内存中的值
		assert("SendActInfo"==ns_LockInitName);
		string zwDate,zwTime;
		zwGetDateTimeString(ptjc.get<time_t>("Lock_Time"),zwDate,zwTime);
		ptccb.put("TransDate",zwDate);
		ptccb.put("TransTime",zwTime);
		ptccb.put("DevCode",ns_ccbAtmno);	//使用缓存在内存中的值
		ptccb.put("LockMan",LOCKMAN_NAME);
		ptccb.put("LockId",ptjc.get<string>("Lock_Serial"));	
		//有用部分
		int ActiveResult=0;	//建行定义该字段0为成功，1为失败；
		string strState=ptjc.get<string>("State");
		if ("ok"==strState)
		{
			ActiveResult=0;
		}
		else
		{
			ActiveResult=1;
		}
		ptccb.put("ActiveResult",ActiveResult);
		//1.1版本里面下位机解密了ECIES加密的PSK并在Lock_Init_Info字段返回
		ptccb.put("ActInfo",ptjc.get<string>("Lock_Init_Info"));
	}

	//////////////////////////////////////////////////////////////////////////

	//读取闭锁码
	void zwconvReadCloseCodeDown( const ptree &ptccb, ptree &ptjc )
	{
		//>> 读取闭锁码
		//请求
		//	"command": JCSTR_READ_CLOSECODE,
		//	"Lock_Time"	
		//应答
		//"Command":JCSTR_READ_CLOSECODE


		ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,JCSTR_READ_CLOSECODE);
		ptjc.put("Lock_Time",time(NULL));
	}

	void zwconvReadCloseCodeUp( const ptree &ptjc, ptree &ptccb )
	{
		//读取闭锁码 应答
		//	交易代码	TransCode	是	值：0004
		//	交易名称	TransName	是	值：ReadShutLockCode
		//	交易日期	TransDate	是	值：YYYYMMDD，如20140401
		//	交易时间	TransTime	是	值：hhmmss，如134050
		//	ATM设备编号	DevCode	是	值：我行12位设备编号
		//	锁具厂商	LockMan	是	值：厂商自定与其他厂商不同的名称
		//	锁具编号	LockId	是	值：厂商自定的锁具唯一编号
		//	闭锁码	ShutLockcode	是	值：闭锁码
		//无用的形式化部分
		ptccb.put("TransCode","0004");
		assert("ReadShutLockCode"==ns_ReadCloseCodeName);
		ptccb.put("TransName",ns_ReadCloseCodeName);
		string zwDate,zwTime;
		zwGetDateTimeString(time(NULL),zwDate,zwTime);
		ptccb.put("TransDate",zwDate);
		ptccb.put("TransTime",zwTime);

		ptccb.put("DevCode",ns_ccbAtmno);
		ptccb.put("LockMan",LOCKMAN_NAME);
		ptccb.put("LockId",ptjc.get<string>("Lock_Serial"));
		ptccb.put("ShutLockcode",ptjc.get<int>("Code"));
	}


	////////////////////////////每一条报文的具体处理函数结束//////////////////////////////////////////////
}	//namespace jcAtmcConvertDLL