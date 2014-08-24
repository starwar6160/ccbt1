#include "stdafx.h"
#include "zwCcbElockHdr.h"
#include "zwPocoLog.h"

using namespace boost::property_tree;

void testjson819(void)
{
	const char *jstest1="{\"Command\": \"Lock_Time_Sync_ATM\",\"Lock_Time\": 1408443318 }";
	ptree ptt;
	std::stringstream ss;
	ss<<jstest1;
	read_json(ss,ptt);
	write_json(ss,ptt);
	cout<<__FUNCTION__<<" "<<ss.str()<<endl;
}


//把ATMC DLL的XML和JSON互转函数集中于此，便于单元测试；
namespace jcAtmcConvertDLL{
	const char *LOCKMAN_NAME="BeiJing.JinChu";
	//下发方向处理
	void zwconvLockActiveDown(const ptree &ptccb, ptree &ptjc );
	void zwconvLockInitDown( const ptree &ptccb, ptree &ptjc );
	void zwconvReadCloseCodeDown( const ptree &ptccb, ptree &ptjc );
	void zwconvQueryLockStatusDown( const ptree &ptccb, ptree &ptjc );
	void zwconvTimeSyncDown( const ptree &ptccb, ptree &ptjc );
	void zwconvGetLockLogDown( const ptree &ptccb, ptree &ptjc );
	void zwconvLockPushWarnDown(const ptree &ptccb, ptree &ptjc );
	void zwconvLockReqTimeSyncDown(const ptree &ptccb, ptree &ptjc );
	
	//上传方向处理
	void zwconvLockActiveUp(const ptree &ptjc, ptree &ptccb );
	void zwconvLockInitUp( const ptree &ptjc, ptree &ptccb );
	void zwconvReadCloseCodeUp( const ptree &ptjc, ptree &ptccb );
	void zwconvTimeSyncUp( const ptree &ptjc, ptree &ptccb );
	void zwconvCheckLockStatusUp( const ptree &ptjc, ptree &ptccb );
	void zwconvGetLockLogUp( const ptree &ptjc, ptree &ptccb );
	//接收锁具主动发送的初始闭锁码，只有上传方向
	void zwconvRecvInitCloseCodeDown(const ptree &ptccb, ptree &ptjc );
	void zwconvRecvInitCloseCodeUp(const ptree &ptjc, ptree &ptccb);
	//接收锁具主动发送的验证码，只有上传方向
	void zwconvRecvVerifyCodeDown(const ptree &ptccb, ptree &ptjc );
	void zwconvRecvVerifyCodeUp(const ptree &ptjc, ptree &ptccb);
	void zwconvLockPushWarnUp(const ptree &ptjc, ptree &ptccb);
	void zwconvLockReqTimeSyncUp(const ptree &ptjc, ptree &ptccb);
	//以下4个字段，为的是在上下转换期间保存建行报文中冗余的，我们基本不用但又必须返回给建行的字段
	string ns_ActReqName;
	string ns_LockInitName;
	string ns_ReadCloseCodeName;
	string ns_ccbAtmno;		//ATM编号

	const JC_MSG_TYPE zwCCBxml2JCjson( const string &downXML,string &downJson )
	{
		ZWFUNCTRACE
		//接受ATMC下发的XML，转化为中间形式ptree
		assert(downXML.length()>42);	//XML开头的固定内容38个字符，外加起码一个标签的两对尖括号合计4个字符
		ZWTRACE("******************建行ATMC下发XML开始*********************\n");
		ZWTRACE(downXML.c_str());
		ZWTRACE("******************建行ATMC下发XML结束*********************\n");
		JC_MSG_TYPE msgType=JCMSG_INVALID_TYPE;
		ptree ptCCB;
		std::stringstream ss;
		ss<<downXML;
		read_xml(ss,ptCCB);
		//////////////////////////////////////////////////////////////////////////

		std::stringstream ssccb;
		write_json(ssccb,ptCCB);
		string ccbJson= ssccb.str();
		ZWTRACE("***建行XML转换后的JSON开始********************************\n");
		ZWTRACE(ccbJson.c_str());
		ZWTRACE("***建行XML转换后的JSON结束********************************\n");


		//判断消息类型
		string transCode=ptCCB.get<string>(CCBSTR_CODE);
		//保存建行冗余字段以便上传返回时提供给建行			
		ns_ccbAtmno=ptCCB.get(CCBSTR_DEVCODE,"CCBATMFAKE88");
		//根据消息类型调用不同函数处理	
		//从建行的接口所需字段变为我们的JSON接口
		ptree ptJC;
		if ("0000"==transCode)
		{//锁具激活请求
		ns_ActReqName=ptCCB.get<string>(CCBSTR_NAME);
			msgType= JCMSG_LOCK_ACTIVE_REQUEST;
			zwconvLockActiveDown(ptCCB,ptJC);
		}
		if ("0001"==transCode)
		{//发送锁具激活信息(初始化)
			ns_LockInitName=ptCCB.get<string>(CCBSTR_NAME);
			msgType= JCMSG_SEND_LOCK_ACTIVEINFO;
			zwconvLockInitDown(ptCCB,ptJC);
		}
		if ("0002"==transCode)
		{//查询锁具状态
			zwconvQueryLockStatusDown(ptCCB,ptJC);
			msgType=JCMSG_QUERY_LOCK_STATUS;
		}
		if ("0003"==transCode)
		{//时间同步
			zwconvTimeSyncDown(ptCCB,ptJC);
			msgType=JCMSG_TIME_SYNC;
		}
		if ("0004"==transCode)
		{//读取闭锁码
			ns_ReadCloseCodeName=ptCCB.get<string>(CCBSTR_NAME);
			msgType= JCMSG_GET_CLOSECODE;
			zwconvReadCloseCodeDown(ptCCB,ptJC);
		}
		if ("0005"==transCode)
		{//读取日志
			msgType= JCMSG_GET_LOCK_LOG;
			zwconvGetLockLogDown(ptCCB,ptJC);
		}
		if ("1001"==transCode)
		{//锁具主动上送告警
			msgType= JCMSG_PUSH_WARNING;
			zwconvLockPushWarnDown(ptCCB,ptJC);
		}
		if ("1003"==transCode)
		{//锁具主动要求时间同步
			msgType= JCMSG_REQUEST_TIME_SYNC;
			zwconvLockReqTimeSyncDown(ptCCB,ptJC);
		}
//////////////////////////////////////////////////////////////////////////
		//锁具单向上传消息的配合一问一答测试消息：
		if ("1000"==transCode)
		{//接收初始闭锁码
			msgType= JCMSG_SEND_INITCLOSECODE;
			zwconvRecvInitCloseCodeDown(ptCCB,ptJC);
		}
		if ("1002"==transCode)
		{//接收验证码
			msgType= JCMSG_SEND_UNLOCK_CERTCODE;
			zwconvRecvVerifyCodeDown(ptCCB,ptJC);
		}
		//处理结果输出为Json供下位机使用
		std::stringstream ss2;
		write_json(ss2,ptJC);
		downJson= ss2.str();
		ZWTRACE("***金储JSON下发请求开始***********************************\n");
		ZWTRACE(downJson.c_str());
		ZWTRACE("***金储JSON下发请求结束***********************************\n");

		return msgType;
	}

	const JC_MSG_TYPE zwJCjson2CCBxml( const string &upJson,string &upXML )
	{
		ZWFUNCTRACE
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
		ZWTRACE("***金储锁具返回的JSON应答开始*************************####\n");
		ZWTRACE(jsonJc.c_str());
		ZWTRACE("***金储锁具返回的JSON应答结束*************************####\n");


		//判断消息类型并从我们的JSON接口变为建行的接口所需字段
		string jcCmd=ptJC.get<string>(jcAtmcConvertDLL::JCSTR_CMDTITLE);
		ptree ptCCB;
		if (JCSTR_LOCK_ACTIVE_REQUEST==jcCmd)
		{//发送锁具激活请求
			zwconvLockActiveUp(ptJC,ptCCB);
		}
		if (JCSTR_LOCK_INIT==jcCmd)
		{//发送锁具激活信息(锁具初始化)
			zwconvLockInitUp(ptJC,ptCCB);
		}
		if (JCSTR_QUERY_LOCK_STATUS==jcCmd)
		{//查询锁具状态
			zwconvCheckLockStatusUp(ptJC,ptCCB);
		}	
		if (JCSTR_TIME_SYNC==jcCmd)
		{//时间同步
			zwconvTimeSyncUp(ptJC,ptCCB);
		}
		if (JCSTR_READ_CLOSECODE==jcCmd)
		{//读取闭锁码
			zwconvReadCloseCodeUp(ptJC,ptCCB);
		}
		if (JCSTR_GET_LOCK_LOG==jcCmd)
		{//读取闭锁码
			zwconvGetLockLogUp(ptJC,ptCCB);
		}

		if (JCSTR_SEND_INITCLOSECODE==jcCmd)
		{//接收下位机主动发来的初始闭锁码
			zwconvRecvInitCloseCodeUp(ptJC,ptCCB);
		}		
		if (JCSTR_SEND_UNLOCK_CERTCODE==jcCmd)
		{//接收下位机主动发来的验证码
			zwconvRecvVerifyCodeUp(ptJC,ptCCB);
		}

		if (JCSTR_PUSH_WARNING==jcCmd)
		{//接收锁具主动上送的告警信息
			zwconvLockPushWarnUp(ptJC,ptCCB);
		}
		if (JCSTR_REQUEST_TIME_SYNC==jcCmd)
		{//接收锁具主动的时间同步请求
			zwconvLockReqTimeSyncUp(ptJC,ptCCB);
		}

		//////////////////////////////////////////////////////////////////////////
		std::stringstream sst2;
		write_json(sst2,ptCCB);
		string jsonCcb=sst2.str();
		//////////////////////////////////////////////////////////////////////////
		ZWTRACE("***金储锁具应答JSON处理为建行所需元素 开始************####\n");
		ZWTRACE(jsonCcb.c_str());
		ZWTRACE("***金储锁具应答JSON处理为建行所需元素 结束************####\n");

		//转换JSON为XML供ATMC使用
		std::stringstream ss2;
		write_xml(ss2,ptCCB);
		upXML= ss2.str();
		ZWTRACE("*********************金储应答XML开始******************####\n");
		ZWTRACE(upXML.c_str());
		ZWTRACE("*********************金储应答XML结束******************####\n");
try{
		string transCode=ptCCB.get<string>(CCBSTR_CODE);
		if ("0000"==transCode)
		{
			return JCMSG_LOCK_ACTIVE_REQUEST;
		}
}
catch(...)
{
	ZWFATAL("金储JSON补全为CCB所需字段过程中有缺失")
	return JCMSG_INVALID_TYPE;
}
		return JCMSG_INVALID_TYPE;
	}

	////////////////////////////每一条报文的具体处理函数开始//////////////////////////////////////////////

	//发送锁具激活请求
	void zwconvLockActiveDown( const ptree &, ptree &ptjc )
	{	
		ZWFUNCTRACE
		ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,JCSTR_LOCK_ACTIVE_REQUEST);
	}


	void zwconvLockActiveUp( const ptree &ptjc, ptree &ptccb )
	{
		ZWFUNCTRACE
		//无用的形式化部分
		ptccb.put(CCBSTR_CODE,"0000");
		assert("CallForActInfo"==ns_ActReqName);
		ptccb.put(CCBSTR_NAME,ns_ActReqName);
		ptccb.put(CCBSTR_DEVCODE,ns_ccbAtmno);
//////////////////////////////////////////////////////////////////////////
		string zwDate,zwTime;
		zwGetLocalDateTimeString(ptjc.get<time_t>("Lock_Time"),zwDate,zwTime);
		ptccb.put(CCBSTR_DATE,zwDate);
		ptccb.put(CCBSTR_TIME,zwTime);

//////////////////////////////////////////////////////////////////////////
		//有用部分
		
		ptccb.put("root.LockId",ptjc.get<string>("Lock_Serial"));
		ptccb.put("root.LockPubKey",ptjc.get<string>("Lock_Public_Key"));	
		ptccb.put("root.LockMan",LOCKMAN_NAME);
	}
	//////////////////////////////////////////////////////////////////////////

	//发送锁具激活信息(锁具初始化)
	void zwconvLockInitDown( const ptree &ptccb, ptree &ptjc )
	{	
		ZWFUNCTRACE
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
		ptjc.put("Atm_Serial",ptccb.get<string>(CCBSTR_DEVCODE));	
		//此处1.1版本已经支持ECIES加密过的PSK输入了，使用第一套密文
		ptjc.put("Lock_Init_Info",ptccb.get<string>("root.ActInfo"));		
	}

	void zwconvLockInitUp( const ptree &ptjc, ptree &ptccb )
	{
		ZWFUNCTRACE
		//无用的形式化部分
		ptccb.put(CCBSTR_CODE,"0001");
		ptccb.put(CCBSTR_NAME,ns_LockInitName);	//使用缓存在内存中的值
		assert("SendActInfo"==ns_LockInitName);
		string zwDate,zwTime;
		zwGetLocalDateTimeString(ptjc.get<time_t>("Lock_Time"),zwDate,zwTime);
		ptccb.put(CCBSTR_DATE,zwDate);
		ptccb.put(CCBSTR_TIME,zwTime);
		ptccb.put(CCBSTR_DEVCODE,ns_ccbAtmno);	//使用缓存在内存中的值
		ptccb.put("root.LockMan",LOCKMAN_NAME);
		ptccb.put("root.LockId",ptjc.get<string>("Lock_Serial"));	
		//有用部分
		int ActiveResult=0;	//建行定义该字段0为成功，1为失败；
		ActiveResult=ptjc.get<int>("State");
		ptccb.put("root.ActiveResult",ActiveResult);
		//1.1版本里面下位机解密了ECIES加密的PSK并在Lock_Init_Info字段返回
		ptccb.put("root.ActInfo",ptjc.get<string>("Lock_Init_Info"));
	}


	
	
	//查询锁具状态
	void zwconvQueryLockStatusDown( const ptree &, ptree &ptjc )
	{
		ZWFUNCTRACE
		ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,JCSTR_QUERY_LOCK_STATUS);
	}



	void zwconvCheckLockStatusUp( const ptree &ptjc, ptree &ptccb )
	{
		ZWFUNCTRACE
			//"Command": "Lock_Now_Info",
			//"Lock_Time": "1408434961",
			//"Lock_Serial": "A3KE2OK256EO2SPE",
			//"Lock_Status": "0,0,0,1,100,0,1,20,100,0,0"
			//无用的形式化部分
		ptccb.put(CCBSTR_CODE,"0002");
		ptccb.put(CCBSTR_NAME,"QueryForLockStatus");
		string zwDate,zwTime;
		zwGetLocalDateTimeString(ptjc.get<time_t>("Lock_Time"),zwDate,zwTime);
		ptccb.put(CCBSTR_DATE,zwDate);
		ptccb.put(CCBSTR_TIME,zwTime);
		//////////////////////////////////////////////////////////////////////////
		ptccb.put(CCBSTR_DEVCODE,ns_ccbAtmno);	//使用缓存在内存中的值
		ptccb.put("root.LockMan",LOCKMAN_NAME);
		ptccb.put("root.LockId",ptjc.get<string>("Lock_Serial"));	
		string LockStatusStr=ptjc.get<string>("Lock_Status");
		//格式：ActiveStatus,EnableStatus,LockStatus,DoorStatus,
		//BatteryStatus,ShockAlert,TempAlert,PswTryAlert,LockOverTime
		//0,0,0,1,100,0,1,20,100,0,0
		JCLOCKSTATUS lockStatusStrings;
		zwLockStatusDataSplit(LockStatusStr.c_str(),lockStatusStrings);
		
	ptccb.put("root.ActiveStatus",lockStatusStrings.ActiveStatus);
	ptccb.put("root.EnableStatus",lockStatusStrings.EnableStatus);
	ptccb.put("root.LockStatus",lockStatusStrings.LockStatus);
	ptccb.put("root.DoorStatus",lockStatusStrings.DoorStatus);
	ptccb.put("root.BatteryStatus",lockStatusStrings.BatteryStatus);
	ptccb.put("root.ShockAlert",lockStatusStrings.ShockAlert);
	ptccb.put("root.ShockValue",lockStatusStrings.ShockValue);
	ptccb.put("root.TempAlert",lockStatusStrings.TempAlert);
	ptccb.put("root.NodeTemp",lockStatusStrings.nodeTemp);
	ptccb.put("root.PswTryAlert",lockStatusStrings.PswTryAlert);
	ptccb.put("root.LockOverTime",lockStatusStrings.LockOverTime);
	}

	//////////////////////////////////////////////////////////////////////////
	//时间同步
	void zwconvTimeSyncDown( const ptree &, ptree &ptjc )
	{
		ZWFUNCTRACE
		ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,JCSTR_TIME_SYNC);
		ptjc.put<time_t>("Lock_Time",time(NULL));
	}

		//时间同步
	void zwconvTimeSyncUp( const ptree &ptjc, ptree &ptccb )
	{
		ZWFUNCTRACE
		//无用的形式化部分
		ptccb.put(CCBSTR_CODE,"0003");
		ptccb.put(CCBSTR_NAME,"TimeSync");	//使用缓存在内存中的值
		string zwDate,zwTime;
		zwGetLocalDateTimeString(ptjc.get<time_t>("Lock_Time"),zwDate,zwTime);
		ptccb.put(CCBSTR_DATE,zwDate);
		ptccb.put(CCBSTR_TIME,zwTime);
		//////////////////////////////////////////////////////////////////////////
		ptccb.put(CCBSTR_DEVCODE,ns_ccbAtmno);	//使用缓存在内存中的值
		ptccb.put("root.LockMan",LOCKMAN_NAME);
		ptccb.put("root.LockId",ptjc.get<string>("Lock_Serial"));	
		//////////////////////////////////////////////////////////////////////////
		//有用部分
		time_t exTimeValue=ptjc.get<time_t>("Ex_Syn_Time");
		string exDate,exTime;
		zwGetLocalDateTimeString(exTimeValue,exDate,exTime);
		ptccb.put("root.ExSynDate",exDate);
		ptccb.put("root.ExSynTime",exTime);
	}

	//////////////////////////////////////////////////////////////////////////

	//读取闭锁码
	void zwconvReadCloseCodeDown( const ptree &, ptree &ptjc )
	{
		ZWFUNCTRACE
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
		ZWFUNCTRACE
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
		ptccb.put(CCBSTR_CODE,"0004");
		assert("ReadShutLockCode"==ns_ReadCloseCodeName);
		ptccb.put(CCBSTR_NAME,ns_ReadCloseCodeName);
		string zwDate,zwTime;
		zwGetLocalDateTimeString(time(NULL),zwDate,zwTime);
		ptccb.put(CCBSTR_DATE,zwDate);
		ptccb.put(CCBSTR_TIME,zwTime);

		ptccb.put(CCBSTR_DEVCODE,ns_ccbAtmno);
		ptccb.put("root.LockMan",LOCKMAN_NAME);
		ptccb.put("root.LockId",ptjc.get<string>("Lock_Serial"));
		ptccb.put("root.ShutLockcode",ptjc.get<int>("Code"));
	}



	//读取日志
	void zwconvGetLockLogDown( const ptree &ptccb, ptree &ptjc )
	{
		ZWFUNCTRACE
		ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,JCSTR_GET_LOCK_LOG);
		ptjc.put("Begin_No",ptccb.get<int>("root.BeginNo"));
		ptjc.put("End_No",ptccb.get<int>("root.EndNo"));
	}

	void zwconvGetLockLogUp( const ptree &ptjc, ptree &ptccb )
	{
		ZWFUNCTRACE
		ptccb.put(CCBSTR_CODE,"0005");
		ptccb.put(CCBSTR_NAME,"ReadLog");
		string zwDate,zwTime;
		zwGetLocalDateTimeString(ptjc.get<time_t>("Lock_Time"),zwDate,zwTime);
		ptccb.put(CCBSTR_DATE,zwDate);
		ptccb.put(CCBSTR_TIME,zwTime);
		ptccb.put("root.RevResult",ptjc.get<int>("State"));
		string LockStatus=ptjc.get<string>("Journal");
		JCLOCKSTATUS lst,ostr;
		zwLockStatusDataSplit(LockStatus.c_str(),lst);
		zwStatusData2String(lst,ostr);
		string LockStatusHumanReadable=LockStatusStringMerge(ostr);
		ptccb.put("root.Log",LockStatusHumanReadable);
	}


	void zwconvRecvInitCloseCodeDown(const ptree &, ptree &ptjc )
	{
		ZWFUNCTRACE
		ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,"Lock_Close_Code_Lock");
	}


	void zwconvRecvInitCloseCodeUp(const ptree &ptjc, ptree &ptccb)
	{
		ZWFUNCTRACE
		ptccb.put(CCBSTR_CODE,"1000");
		ptccb.put(CCBSTR_NAME,"SendShutLockCode");
		string zwDate,zwTime;
		zwGetLocalDateTimeString(time(NULL),zwDate,zwTime);
		ptccb.put(CCBSTR_DATE,zwDate);
		ptccb.put(CCBSTR_TIME,zwTime);
		//锁具发送初始闭锁码时，ATM编号应该已经在激活请求中获得，但是
		//1.1版本报文里面没有给出，所以此处可能会有问题
		ptccb.put(CCBSTR_DEVCODE,ns_ccbAtmno);	
		ptccb.put("root.LockMan",LOCKMAN_NAME);
		ptccb.put("root.ShutLockcode",ptjc.get<int>("Code"));
	}

	void zwconvRecvVerifyCodeDown(const ptree &, ptree &ptjc )
	{
		ZWFUNCTRACE
		ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,"Lock_Open_Ident");
	}

	void zwconvRecvVerifyCodeUp(const ptree &ptjc, ptree &ptccb)
	{
		ZWFUNCTRACE
		ptccb.put(CCBSTR_CODE,"1002");
		ptccb.put(CCBSTR_NAME,"SendUnLockIdent");
		string zwDate,zwTime;
		zwGetLocalDateTimeString(time(NULL),zwDate,zwTime);
		ptccb.put(CCBSTR_DATE,zwDate);
		ptccb.put(CCBSTR_TIME,zwTime);
		//锁具发送初始闭锁码时，ATM编号应该已经在激活请求中获得，但是
		//1.1版本报文里面没有给出，所以此处可能会有问题
		ptccb.put(CCBSTR_DEVCODE,ns_ccbAtmno);	
		ptccb.put("root.LockMan",LOCKMAN_NAME);
		ptccb.put("root.LockId",ptjc.get<string>("Lock_Serial"));
		//关键的验证码本体
		ptccb.put("root.UnLockIdentInfo",ptjc.get<int>("Lock_Ident_Info"));
	}

	void zwconvLockPushWarnDown(const ptree &ptccb, ptree &ptjc )
	{
		ZWFUNCTRACE
		ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,jcAtmcConvertDLL::JCSTR_PUSH_WARNING);
		ptjc.put("State",ptccb.get<int>("RevResult"));
	}

	void zwconvLockReqTimeSyncDown(const ptree &, ptree &ptjc )
	{
		ZWFUNCTRACE
		ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,jcAtmcConvertDLL::JCSTR_REQUEST_TIME_SYNC);
		ptjc.put("Lock_Time",time(NULL));
	}

	void zwconvLockPushWarnUp(const ptree &ptjc, ptree &ptccb)
	{
		ZWFUNCTRACE
		ptccb.put(CCBSTR_CODE,"1001");
		ptccb.put(CCBSTR_NAME,"SendAlertStatus");
		string zwDate,zwTime;
		zwGetLocalDateTimeString(time(NULL),zwDate,zwTime);
		ptccb.put(CCBSTR_DATE,zwDate);
		ptccb.put(CCBSTR_TIME,zwTime);
		//锁具发送初始闭锁码时，ATM编号应该已经在激活请求中获得，但是
		//1.1版本报文里面没有给出，所以此处可能会有问题
		ptccb.put(CCBSTR_DEVCODE,ns_ccbAtmno);	
		ptccb.put("root.LockMan",LOCKMAN_NAME);
		ptccb.put("root.LockId",ptjc.get<string>("Lock_Serial"));
		string LockStatusStr=ptjc.get<string>("Lock_Status");
		//格式：ActiveStatus,EnableStatus,LockStatus,DoorStatus,
		//BatteryStatus,ShockAlert,TempAlert,PswTryAlert,LockOverTime
		//0,0,0,1,100,0,1,20,100,0,0
		JCLOCKSTATUS lockStatusStrings;
		zwLockStatusDataSplit(LockStatusStr.c_str(),lockStatusStrings);

		ptccb.put("root.ActiveStatus",lockStatusStrings.ActiveStatus);
		ptccb.put("root.EnableStatus",lockStatusStrings.EnableStatus);
		ptccb.put("root.LockStatus",lockStatusStrings.LockStatus);
		ptccb.put("root.DoorStatus",lockStatusStrings.DoorStatus);
		ptccb.put("root.BatteryStatus",lockStatusStrings.BatteryStatus);
		ptccb.put("root.ShockAlert",lockStatusStrings.ShockAlert);
		ptccb.put("root.ShockValue",lockStatusStrings.ShockValue);
		ptccb.put("root.TempAlert",lockStatusStrings.TempAlert);
		ptccb.put("root.NodeTemp",lockStatusStrings.nodeTemp);
		ptccb.put("root.PswTryAlert",lockStatusStrings.PswTryAlert);
		ptccb.put("root.LockOverTime",lockStatusStrings.LockOverTime);
	}

	void zwconvLockReqTimeSyncUp(const ptree &ptjc, ptree &ptccb)
	{
		ZWFUNCTRACE
		ptccb.put(CCBSTR_CODE,"1003");
		ptccb.put(CCBSTR_NAME,"TimeSync");
		string zwDate,zwTime;
		zwGetLocalDateTimeString(time(NULL),zwDate,zwTime);
		ptccb.put(CCBSTR_DATE,zwDate);
		ptccb.put(CCBSTR_TIME,zwTime);
		//锁具发送初始闭锁码时，ATM编号应该已经在激活请求中获得，但是
		//1.1版本报文里面没有给出，所以此处可能会有问题
		ptccb.put(CCBSTR_DEVCODE,ns_ccbAtmno);	
		ptccb.put("root.LockMan",LOCKMAN_NAME);
		ptccb.put("root.LockId",ptjc.get<string>("Lock_Serial"));
	}

	////////////////////////////每一条报文的具体处理函数结束//////////////////////////////////////////////
}	//namespace jcAtmcConvertDLLLock_Open_Ident
