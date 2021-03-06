#include "stdafx.h"
#include "zwCcbElockHdr.h"
#include "myConvIntHdr.h"
//20150105.万敏和马浩测试用的东西
#define TMP_MAHAO_TEST_20150105

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

//把ATMC DLL的XML和JSON互转函数集中于此，便于单元测试；
namespace jcAtmcConvertDLL {

	// zwtrim from start
	std::string &zwltrim(std::string &s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
		return s;
	}

	// zwtrim from end
	std::string &zwrtrim(std::string &s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
		return s;
	}

	// zwtrim from both ends
	std::string &zwtrim(std::string &s) {
		return zwltrim(zwrtrim(s));
	}


#ifdef TMP_MAHAO_TEST_20150105
	//20150105.万敏和马浩测试用的东西
	int G_MAHAO_LOG_COUNT=1;
#endif // TMP_MAHAO_TEST_20150105
	const JC_MSG_TYPE zwCCBxml2JCjson(const string & downXML,
					  string & downJson) {
		    //接受ATMC下发的XML，转化为中间形式ptree
		    assert(downXML.length() > 42);	//XML开头的固定内容38个字符，外加起码一个标签的两对尖括号合计4个字符
#ifdef _DEBUG401ZW
		ZWDBGMSG
		    ("******************建行ATMC下发XML开始***\n");
		ZWDBGMSG(downXML.c_str());
		ZWDBGMSG
		    ("******************建行ATMC下发XML结束****\n");
#endif // _DEBUG401ZW
		JC_MSG_TYPE msgType = JCMSG_INVALID_TYPE;
		ptree ptCCB;
		 std::stringstream ss;
		 ss << downXML;
		 read_xml(ss, ptCCB);
		//////////////////////////////////////////////////////////////////////////

		 std::stringstream ssccb;
		 write_json(ssccb, ptCCB);
		string ccbJson = ssccb.str();
		//LOG(INFO)<<"建行XML转换后的JSON开始\n"<<ccbJson;
		 //ZWDBGWARN("CCBXML转换后的JSON结束\n");

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
#ifdef TMP_MAHAO_TEST_20150105
			//每次新的读取日志命令，计数器重新归1
			G_MAHAO_LOG_COUNT=1;
#endif // TMP_MAHAO_TEST_20150105
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
		if ("5002" == transCode) {	//ATM机设置上传的小循环次数命令
			msgType = JCMSG_SENSE_SET_INSIDE_LOOP_TIMES;
			zwconvTemptureSetInsideLoopTimesDown(ptCCB, ptJC);
		}
		if ("5003" == transCode) {	//ATM机设置上传的小循环周期(单位秒)命令
			msgType = JCMSG_SENSE_SET_INSIDE_LOOP_PERIOD;
			zwconvTemptureSetInsideLoopPeriodDown(ptCCB, ptJC);
		}
		if ("5004" == transCode) {	//ATM机设置上传的大循环周期(单位分钟)命令
			msgType = JCMSG_SENSE_SET_OUTSIDE_LOOP_PERIOD;
			zwconvTemptureSetOutsideLoopPeriodDown(ptCCB, ptJC);
		}

#ifdef _JINCHU_DEV1608
		if ("5005" == transCode) {	
			//金储内部开发方便用的在线锁卸载命令，绝不能出现在给建行的版本里面；
			//所以特地在这里调用，把实际代码用于条件编译，那么给建行的正式Release版本
			// 如果调用了这两个函数就会链接错误，特意这么做的；20160801.1441.周伟
			msgType = JCMSG_PRV_LOCK_UNINSTALL;
			zwconvJCDevLockUninstallDown(ptCCB, ptJC);
		}
#endif // _JINCHU_DEV1608


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
////////////////////////////////20150430//////////////////////////////////////////

		//处理结果输出为Json供下位机使用
		std::stringstream ss2;
		write_json(ss2, ptJC);
		downJson = ss2.str();
//#ifdef _DEBUG401
#ifdef _DEBUG
		//printf("当前机器时间time(NULL)=%u\n",time(NULL));
#endif // _DEBUG
		//LOG(WARNING)<<"金储JSON下发请求开始\n"		<<downJson;
		//printf("%s\n", downJson.c_str());
		//ZWDBGWARN("金储JSON下发请求结束\n");
//#endif // _DEBUG401

		return msgType;
	}
//////////////////////////////////////////////////////////////////////////

	const JC_MSG_TYPE zwJCjson2CCBxml(const string & upJson, string & upXML) {
		    //从下位机接收而来的json结果字符串，解码为中间形式ptree
		    assert(upJson.length() > 9);	//json最基本的符号起码好像要9个字符左右
			ptree ptJC;
			std::stringstream ss;
			std::stringstream sst1;
			ptree ptCCB;

		try
		{
			

#ifdef _DEBUG_TEST721
			if (time(NULL)%3==0)
			{
				string modiJson=upJson;
				modiJson[5]=':';
				ss<<modiJson;
			}
#else
			ss << upJson;
#endif // _DEBUG
			
			read_json(ss, ptJC);
			//////////////////////////////////////////////////////////////////////////			
			write_json(sst1, ptJC);
			string jsonJc = sst1.str();
//#ifdef _DEBUG401
		//LOG(WARNING)<<"金储锁具返回的JSON应答开始\n"<<jsonJc;
		//printf("%s\n", jsonJc.c_str());
		//ZWDBGWARN("JINCHU锁具返回的JSON应答结束\n");
//#endif // _DEBUG401

		//判断消息类型并从我们的JSON接口变为建行的接口所需字段
		string jcCmd =
		    ptJC.get < string > (jcAtmcConvertDLL::JCSTR_CMDTITLE);
		
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
			zwconvRecvVerifyCodeUp(ptJC, ptCCB);
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
		if (JCSTR_SENSE_SET_INSIDE_LOOP_TIMES == jcCmd) {	//ATM机设置上传的小循环次数命令
			zwconvTemptureSetInsideLoopTimesUp(ptCCB, ptJC);
		}
		if (JCSTR_SENSE_SET_INSIDE_LOOP_PERIOD == jcCmd) {	//ATM机设置上传的小循环周期(单位秒)命令
			zwconvTemptureSetInsideLoopPeriodUp(ptCCB, ptJC);
		}
		if (JCSTR_SENSE_SET_OUTSIDE_LOOP_PERIOD == jcCmd) {	//ATM机设置上传的大循环周期(单位分钟)命令
			zwconvTemptureSetOutsideLoopPeriodUp(ptCCB, ptJC);
		}
#ifdef _JINCHU_DEV1608
		if (JCSTR_PRV_LOCKUNINSTALL == jcCmd) {	
			//金储内部使用的锁具卸载指令，绝不能出现在给建行的版本中
			zwconvJCDevLockUninstallUp(ptJC, ptCCB);
		}
#endif // _JINCHU_DEV1608
		//////////////////////////////////////////////////////////////////////////
		std::stringstream sst2;		
		write_json(sst2, ptCCB);		
		string jsonCcb = sst2.str();
		//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG401
		//LOG(INFO)<<"金储锁具应答JSON处理为建行所需元素开始\n"<<jsonCcb.c_str();
		//ZWDBGMSG("金储锁具应答JSON处理为建行所需元素结束\n");	
#endif // _DEBUG401
	
		//转换JSON为XML供ATMC使用
		std::stringstream ss2;		
		write_xml(ss2, ptCCB);
		upXML = ss2.str();
#ifdef _DEBUG401ZW
		ZWDBGMSG("金储应答XML开始\n");
		ZWDBGMSG(upXML.c_str());
		//printf("%s\n",upXML.c_str());
		//ZWDBGMSG("金储应答XML结束\n");
#endif // _DEBUG401ZW
#ifdef TMP_MAHAO_TEST_20150105
		//20150105.1623.应万敏和马浩的临时测试要求添加把0005报文返回内容写入文件的要求临时添加，以后不需要；
		if (JCSTR_GET_LOCK_LOG == jcCmd) {	//读取闭锁码
			FILE *fp=fopen("mahaoLog20150105.txt","a");
			char buf[64];
			memset(buf,0,64);
			sprintf(buf,"\n第%d条:\n",G_MAHAO_LOG_COUNT++);
			fwrite(buf,strlen(buf),1,fp);
			fwrite(upXML.c_str(),upXML.length(),1,fp);
			fclose(fp);
		}
#endif // TMP_MAHAO_TEST_20150105
				}
				catch(boost::property_tree::ptree_bad_path &e)
				{
					string myErrMsg="锁具上行JSON报文解析错误ptree_bad_path "+upJson+" "+e.what();
					OutputDebugStringA(myErrMsg.c_str());
					//MessageBoxA(NULL,myErrMsg.c_str(),"zwJCjson2CCBxml",MB_OK);
					LOG(ERROR)<<myErrMsg<<endl;
					upXML="";
					return JCMSG_INVALID_TYPE;
				}
				catch(boost::property_tree::ptree_bad_data &e)
				{
					string myErrMsg="锁具上行JSON报文解析错误ptree_bad_data "+upJson+" "+e.what();
					OutputDebugStringA(myErrMsg.c_str());
					//MessageBoxA(NULL,myErrMsg.c_str(),"zwJCjson2CCBxml",MB_OK);
					LOG(ERROR)<<myErrMsg<<endl;
					upXML="";
					return JCMSG_INVALID_TYPE;
				}
				catch (const boost::property_tree::ptree_error &e)
				{
					string myErrMsg="锁具上行JSON报文解析错误ptree_error "+upJson+" "+e.what();
					OutputDebugStringA(myErrMsg.c_str());
					//MessageBoxA(NULL,myErrMsg.c_str(),"zwJCjson2CCBxml",MB_OK);
					LOG(ERROR)<<myErrMsg<<endl;
					upXML="";
					return JCMSG_INVALID_TYPE;
				}


		try {			
			string transCode = ptCCB.get < string > (CCBSTR_CODE);			
			if ("0000" == transCode) {
				return JCMSG_LOCK_ACTIVE_REQUEST;
			}
		}
		catch(...) {			
			ZWFATAL("金储JSON补全为CCB所需字段过程中有缺失")
			    return JCMSG_INVALID_TYPE;
		}		
		return JCMSG_INVALID_TYPE;
	}
}				//namespace jcAtmcConvertDLL{
