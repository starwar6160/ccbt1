#include "stdafx.h"
#include "zwCcbElockHdr.h"
#include "zwPocoLog.h"
#include "myConvIntHdr.h"

//把ATMC DLL的XML和JSON互转函数集中于此，便于单元测试；
namespace jcAtmcConvertDLL {

	const JC_MSG_TYPE zwCCBxml2JCjson(const string & downXML,
					  string & downJson) {
		ZWFUNCTRACE
		    //接受ATMC下发的XML，转化为中间形式ptree
		    assert(downXML.length() > 42);	//XML开头的固定内容38个字符，外加起码一个标签的两对尖括号合计4个字符
		ZWDBGMSG
		    ("******************建行ATMC下发XML开始*********************\n");
		ZWDBGMSG(downXML.c_str());
		ZWDBGMSG
		    ("******************建行ATMC下发XML结束*********************\n");
		JC_MSG_TYPE msgType = JCMSG_INVALID_TYPE;
		ptree ptCCB;
		 std::stringstream ss;
		 ss << downXML;
		 read_xml(ss, ptCCB);
		//////////////////////////////////////////////////////////////////////////

		 std::stringstream ssccb;
		 write_json(ssccb, ptCCB);
		string ccbJson = ssccb.str();
		 ZWDBGMSG
		    ("***建行XML转换后的JSON开始********************************\n");
		 ZWDBGMSG(ccbJson.c_str());
		 ZWDBGMSG
		    ("***建行XML转换后的JSON结束********************************\n");

		//判断消息类型
		string transCode = ptCCB.get < string > (CCBSTR_CODE);
		//根据消息类型调用不同函数处理  
		//从建行的接口所需字段变为我们的JSON接口
		ptree ptJC;
		if ("0000" == transCode) {	//锁具激活请求
			ns_ActReqName = ptCCB.get < string > (CCBSTR_NAME);
			msgType = JCMSG_LOCK_ACTIVE_REQUEST;
			zwconvLockActiveDown(ptCCB, ptJC);
		}
		if ("0001" == transCode) {	//发送锁具激活信息(初始化)
			ns_LockInitName = ptCCB.get < string > (CCBSTR_NAME);
			msgType = JCMSG_SEND_LOCK_ACTIVEINFO;
			zwconvLockInitDown(ptCCB, ptJC);
		}
		if ("0002" == transCode) {	//查询锁具状态
			zwconvQueryLockStatusDown(ptCCB, ptJC);
			msgType = JCMSG_QUERY_LOCK_STATUS;
		}
		if ("0003" == transCode) {	//时间同步
			zwconvTimeSyncDown(ptCCB, ptJC);
			msgType = JCMSG_TIME_SYNC;
		}
		if ("0004" == transCode) {	//读取闭锁码
			ns_ReadCloseCodeName =
			    ptCCB.get < string > (CCBSTR_NAME);
			msgType = JCMSG_GET_CLOSECODE;
			zwconvReadCloseCodeDown(ptCCB, ptJC);
		}
		if ("0005" == transCode) {	//读取日志
			msgType = JCMSG_GET_LOCK_LOG;
			zwconvGetLockLogDown(ptCCB, ptJC);
		}
		if ("1001" == transCode) {	//锁具主动上送告警
			msgType = JCMSG_PUSH_WARNING;
			zwconvLockPushWarnDown(ptCCB, ptJC);
		}
		if ("1003" == transCode) {	//锁具主动要求时间同步
			msgType = JCMSG_REQUEST_TIME_SYNC;
			zwconvLockReqTimeSyncDown(ptCCB, ptJC);
		}
		//////////////////////////////////////////////////////////////////////////
		//20141111.万敏要求增加的温度和振动传感器支持报文
		if ("5000" == transCode) {	//温度传感器报文
			msgType = JCMSG_SENSE_TEMPTURE;
			zwconvTemptureSenseDown(ptCCB, ptJC);
		}
		if ("5001" == transCode) {	//振动传感器报文
			msgType = JCMSG_SENSE_SHOCK;
			zwconvShockSenseDown(ptCCB, ptJC);
		}

		//////////////////////////////////////////////////////////////////////////
		//锁具单向上传消息的配合一问一答测试消息：
		if ("1000" == transCode) {	//接收初始闭锁码
			msgType = JCMSG_SEND_INITCLOSECODE;
			zwconvRecvInitCloseCodeDown(ptCCB, ptJC);
		}
		if ("1002" == transCode) {	//接收验证码
			msgType = JCMSG_SEND_UNLOCK_CERTCODE;
			zwconvRecvVerifyCodeDown(ptCCB, ptJC);
		}
		//处理结果输出为Json供下位机使用
		std::stringstream ss2;
		write_json(ss2, ptJC);
		downJson = ss2.str();
		ZWDBGMSG
		    ("***金储JSON下发请求开始***********************************\n");
		ZWDBGMSG(downJson.c_str());
		printf("%s\n",downJson.c_str());
		ZWDBGMSG
		    ("***金储JSON下发请求结束***********************************\n");

		return msgType;
	}
//////////////////////////////////////////////////////////////////////////

	const JC_MSG_TYPE zwJCjson2CCBxml(const string & upJson, string & upXML) {
		ZWFUNCTRACE
		    //从下位机接收而来的json结果字符串，解码为中间形式ptree
		    assert(upJson.length() > 9);	//json最基本的符号起码好像要9个字符左右
		ptree ptJC;
		std::stringstream ss;
		ss << upJson;
		read_json(ss, ptJC);
		//////////////////////////////////////////////////////////////////////////
		std::stringstream sst1;
		write_json(sst1, ptJC);
		string jsonJc = sst1.str();
		ZWDBGMSG
		    ("***金储锁具返回的JSON应答开始*************************####\n");
		ZWDBGMSG(jsonJc.c_str());
		printf("%s\n",jsonJc.c_str());
		ZWDBGMSG
		    ("***金储锁具返回的JSON应答结束*************************####\n");

		//判断消息类型并从我们的JSON接口变为建行的接口所需字段
		string jcCmd =
		    ptJC.get < string > (jcAtmcConvertDLL::JCSTR_CMDTITLE);
		ptree ptCCB;
		if (JCSTR_LOCK_ACTIVE_REQUEST == jcCmd) {	//发送锁具激活请求
			zwconvLockActiveUp(ptJC, ptCCB);
		}
		if (JCSTR_LOCK_INIT == jcCmd) {	//发送锁具激活信息(锁具初始化)
			zwconvLockInitUp(ptJC, ptCCB);
		}
		if (JCSTR_QUERY_LOCK_STATUS == jcCmd) {	//查询锁具状态
			zwconvCheckLockStatusUp(ptJC, ptCCB);
		}
		if (JCSTR_TIME_SYNC == jcCmd) {	//时间同步
			zwconvTimeSyncUp(ptJC, ptCCB);
		}
		if (JCSTR_READ_CLOSECODE == jcCmd) {	//读取闭锁码
			zwconvReadCloseCodeUp(ptJC, ptCCB);
		}
		if (JCSTR_GET_LOCK_LOG == jcCmd) {	//读取闭锁码
			zwconvGetLockLogUp(ptJC, ptCCB);
		}

		if (JCSTR_SEND_INITCLOSECODE == jcCmd) {	//接收下位机主动发来的初始闭锁码
			zwconvRecvInitCloseCodeUp(ptJC, ptCCB);
		}
		if (JCSTR_SEND_UNLOCK_CERTCODE == jcCmd) {	//接收下位机主动发来的验证码
			OutputDebugStringA("20141017.1116.MaHaoTest1");
			zwconvRecvVerifyCodeUp(ptJC, ptCCB);
			OutputDebugStringA("20141017.1116.MaHaoTest2");
		}

		if (JCSTR_PUSH_WARNING == jcCmd) {	//接收锁具主动上送的告警信息
			zwconvLockPushWarnUp(ptJC, ptCCB);
		}
		if (JCSTR_REQUEST_TIME_SYNC == jcCmd) {	//接收锁具主动的时间同步请求
			zwconvLockReqTimeSyncUp(ptJC, ptCCB);
		}
		//////////////////////////////////////////////////////////////////////////
		//20141111万敏.温度振动传感器报文支持
		if (JCSTR_SENSE_TEMPTURE == jcCmd) {	//温度传感器
			zwconvTemptureSenseUp(ptJC, ptCCB);
		}
		if (JCSTR_SENSE_SHOCK == jcCmd) {	//振动传感器
			zwconvShockSenseUp(ptJC, ptCCB);
		}

		//////////////////////////////////////////////////////////////////////////
		std::stringstream sst2;
		OutputDebugStringA("20141017.1116.MaHaoTest3");
		write_json(sst2, ptCCB);
		OutputDebugStringA("20141017.1116.MaHaoTest4");
		string jsonCcb = sst2.str();
		//////////////////////////////////////////////////////////////////////////
		ZWDBGMSG
		    ("***金储锁具应答JSON处理为建行所需元素 开始************####\n");
		ZWDBGMSG(jsonCcb.c_str());
		ZWDBGMSG
		    ("***金储锁具应答JSON处理为建行所需元素 结束************####\n");
		OutputDebugStringA("20141017.1116.MaHaoTest5");
		//转换JSON为XML供ATMC使用
		std::stringstream ss2;
		OutputDebugStringA("20141017.1116.MaHaoTest6");
		write_xml(ss2, ptCCB);
		OutputDebugStringA("20141017.1116.MaHaoTest7");
		upXML = ss2.str();
		ZWDBGMSG
		    ("*********************金储应答XML开始******************####\n");
		ZWDBGMSG(upXML.c_str());
		printf("%s\n",upXML.c_str());
		ZWDBGMSG
		    ("*********************金储应答XML结束******************####\n");
		try {
			OutputDebugStringA("20141017.1116.MaHaoTest8");
			string transCode = ptCCB.get < string > (CCBSTR_CODE);
			OutputDebugStringA("20141017.1116.MaHaoTest9");
			if ("0000" == transCode) {
				return JCMSG_LOCK_ACTIVE_REQUEST;
			}
		}
		catch(...) {
			OutputDebugStringA("20141017.1116.MaHaoTest10");
			ZWFATAL("金储JSON补全为CCB所需字段过程中有缺失")
			    return JCMSG_INVALID_TYPE;
		}
		OutputDebugStringA("20141017.1116.MaHaoTest11");
		return JCMSG_INVALID_TYPE;
	}
}				//namespace jcAtmcConvertDLL{
