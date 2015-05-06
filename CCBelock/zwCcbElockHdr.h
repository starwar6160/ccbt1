#ifndef zwCcbElockHdr_h__
#define zwCcbElockHdr_h__
//报文类型
typedef enum jcmsg_ccb_elock_t {
	JCMSG_INVALID_TYPE,	//无效报文类型
	//ATMC TO LOCK MSG:     
	JCMSG_LOCK_ACTIVE_REQUEST,	//激活锁具请求(获取锁具编号和公钥)
	JCMSG_SEND_LOCK_ACTIVEINFO,	//发送锁具激活信息(锁具初始化)
	JCMSG_QUERY_LOCK_STATUS,	//查询锁具状态
	JCMSG_TIME_SYNC,	//时间同步
	JCMSG_GET_CLOSECODE,	//获取闭锁码
	JCMSG_GET_LOCK_LOG,	//读取日志
	//LOCK TO ATMC MSG:
	JCMSG_SEND_INITCLOSECODE,	//发送闭锁码
	JCMSG_PUSH_WARNING,	//锁具主动上送告警
	JCMSG_SEND_UNLOCK_CERTCODE,	//锁具发送开锁验证码
	JCMSG_REQUEST_TIME_SYNC,	//锁具主动要求时间同步
	//////////////////////////////////////////////////////////////////////////
	//20141111.万敏要求增加的温度和振动传感器支持报文
	JCMSG_SENSE_TEMPTURE,	//温度传感器
	JCMSG_SENSE_SHOCK,	//振动传感器
	JCMSG_SENSE_SET_INSIDE_LOOP_TIMES,//ATM机设置上传的小循环次数命令
	JCMSG_SENSE_SET_INSIDE_LOOP_PERIOD,//ATM机设置上传的小循环周期(单位秒)命令
	JCMSG_SENSE_SET_OUTSIDE_LOOP_PERIOD,//ATM机设置上传的大循环周期(单位分钟)命令
} JC_MSG_TYPE;

//为了日志，报警等用途设计的数据结构
typedef struct jcLockStatus_t {
	string LogGenDate;	//20140912。1540,应万敏要求加上的日志生成的时间字段
	string ActiveStatus;
	string EnableStatus;
	string LockStatus;
	string DoorStatus;
	string BatteryStatus;
	string ShockAlert;
	string ShockValue;
	string TempAlert;
	string nodeTemp;
	string PswTryAlert;
	string LockOverTime;
} JCLOCKSTATUS;

namespace jcAtmcConvertDLL {
	//JSON命令字符串定义
	extern const char *JCSTR_CMDTITLE;
	extern const char *JCSTR_LOCK_ACTIVE_REQUEST;	//锁具激活请求
	extern const char *JCSTR_LOCK_INIT;	//锁具初始化
	extern const char *JCSTR_QUERY_LOCK_STATUS;	//查询锁具状态
	extern const char *JCSTR_TIME_SYNC;	//时间同步
	extern const char *JCSTR_READ_CLOSECODE;	//读取闭锁码
	extern const char *JCSTR_SEND_INITCLOSECODE;	//下位机主动发送初始闭锁码
	extern const char *JCSTR_SEND_UNLOCK_CERTCODE;	//下位机主动发送验证码
	extern const char *JCSTR_GET_LOCK_LOG;	//读取日志
	extern const char *JCSTR_PUSH_WARNING;	//锁具主动上送告警
	extern const char *JCSTR_REQUEST_TIME_SYNC;	//锁具主动要求时间同步
	//20141111万敏.温度振动传感器报文支持
	extern const char *JCSTR_SENSE_TEMPTURE;	//温度传感器
	extern const char *JCSTR_SENSE_SHOCK;	//振动传感器
	extern const char *JCSTR_SENSE_SET_INSIDE_LOOP_TIMES;//ATM机设置上传的小循环次数命令
	extern const char *JCSTR_SENSE_SET_INSIDE_LOOP_PERIOD;//ATM机设置上传的小循环周期(单位秒)命令
	extern const char *JCSTR_SENSE_SET_OUTSIDE_LOOP_PERIOD;//ATM机设置上传的大循环周期(单位分钟)命令


	extern const char *CCBSTR_CODE;
	extern const char *CCBSTR_NAME;
	extern const char *CCBSTR_DATE;
	extern const char *CCBSTR_TIME;
	extern const char *CCBSTR_DEVCODE;

	const JC_MSG_TYPE zwCCBxml2JCjson(const string & inXML,
					  string & outJson);
	const JC_MSG_TYPE zwJCjson2CCBxml(const string & inJson,
					  string & outXML);
	extern const char *LOCKMAN_NAME;
} //namespace jcAtmcConvertDLL{ 

enum JC_CCBELOCK_ERROR_CODE {
	ELOCK_ERROR_SUCCESS = 0,	//操作成功
	ELOCK_ERROR_NOTSUPPORT = 1,	//不支持的接口
	ELOCK_ERROR_HARDWAREERROR = 2,	//硬件故障
	ELOCK_ERROR_PARAMINVALID = 3,	//参数非法
	ELOCK_ERROR_CONNECTLOST = 4,	//失去设备连接
	ELOCK_ERROR_TIMEOUT = 5	//操作超时
};

class zw_trace {
	string m_str;
	string m_start;
	string m_end;
      public:
	 zw_trace(const char *funcName);
	~zw_trace();
};
#define ZWFUNCTRACE	zw_trace fntr(__FUNCTION__);

void zwLockStatusDataSplit(const char *LockStatus, JCLOCKSTATUS & lst);
void zwStatusData2String(const JCLOCKSTATUS & lst, JCLOCKSTATUS & ostr);
string LockStatusStringMerge(JCLOCKSTATUS & ostr);
void zwGetDLLPath(HMODULE hDLL, char *pDllPath, const int dllPathLen);

//从给定的时间秒数，获取日期(YYYYMMDD)和时间(HHMMSS)字符串
namespace zwTimeFunc {
	//从给定的时间GMT秒数，获取本地日期(YYYYMMDD)和时间(HHMMSS)字符串
	void zwGetLocalDateTimeString(time_t inTimeUTC, string & outDateLocal,
				      string & outTimeLocal);
	//把建行的本地日期和时间两项，合成之后，转换为一个UTC时间(格林尼治时间,协调世界时)秒数
	//建行的日期是YYYYMMDD格式8位，时间是HHMMSS6位
	void zwCCBDateTime2UTC(const char *ccbDateLocal,
			       const char *ccbTimeLocal, time_t * outUTC);
} //namespace zwTimeFunc{ 
using zwTimeFunc::zwGetLocalDateTimeString;
using zwTimeFunc::zwCCBDateTime2UTC;

typedef void (cdecl * RecvMsgRotine) (const char *pszMsg);
namespace zwCfg {
	extern RecvMsgRotine g_WarnCallback;
	extern bool s_hidOpened;
} 
void ZWDBGMSG(const char *x);
void ZWDBGWARN(const char *x);
#define ZWUSE_HID_MSG_SPLIT
extern HMODULE G_DLL_HMODULE;

//最长为128字节,用于测试目的尽快达到限制暴露问题
//从原来的namespace zwCfg里面单独提出来的，因为该变量有点链接问题，先单独提出来；
#define JC_MSG_MAXLEN	(1*1024)
#define JC_CCBDLL_TIMEOUT	(30)

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif


unsigned char crc8Short( const void *inputData,const int inputLen );
unsigned long Crc32_ComputeBuf(unsigned long inCrc32, const void *buf, size_t bufLen);


namespace jchidDevice2015{

}	//namespace jchidDevice2015{

#endif // zwCcbElockHdr_h__
