#ifndef zwCcbElockHdr_h__
#define zwCcbElockHdr_h__

/////////////////////////////////测试用函数/////////////////////////////////////////
//报文类型
typedef enum jcmsg_ccb_elock_t{
	JCMSG_INVALID_TYPE,	//无效报文类型
	//ATMC TO LOCK MSG:	
	JCMSG_LOCK_ACTIVE_REQUEST,	//激活锁具请求(获取锁具编号和公钥)
	JCMSG_SEND_LOCK_ACTIVEINFO,	//发送锁具激活信息(锁具初始化)
	JCMSG_QUERY_LOCK_STATUS,
	JCMSG_TIME_SYNC,
	JCMSG_GET_CLOSECODE,		//获取闭锁码
	JCMSG_GET_LOCK_LOG,
	//LOCK TO ATMC MSG:
	JCMSG_SEND_CLOSECODE,
	JCMSG_PUSH_WARNING,
	JCMSG_SEND_UNLOCK_CERTCODE,	//锁具发送开锁验证码
	JCMSG_REQUEST_TIME_SYNC,
	JCMSG_TEST730A1,	//测试用
}JC_MSG_TYPE;

namespace jcAtmcConvertDLL{
	//JSON命令字符串定义
	extern const char *JCSTR_LOCK_ACTIVE_REQUEST;	//锁具激活请求
	extern const char *JCSTR_LOCK_INIT;				//锁具初始化
	extern const char *JCSTR_READ_CLOSECODE;		//读取闭锁码

	const JC_MSG_TYPE zwCCBxml2JCjson(const string &inXML,string &outJson);
	const JC_MSG_TYPE zwJCjson2CCBxml(const string &inJson,string &outXML);
	extern const char *LOCKMAN_NAME;
}	//namespace jcAtmcConvertDLL{

enum JC_CCBELOCK_ERROR_CODE{
	ELOCK_ERROR_SUCCESS=0,		//操作成功
	ELOCK_ERROR_NOTSUPPORT=1,	//不支持的接口
	ELOCK_ERROR_HARDWAREERROR=2,//硬件故障
	ELOCK_ERROR_PARAMINVALID=3,	//参数非法
	ELOCK_ERROR_CONNECTLOST=4,	//失去设备连接
	ELOCK_ERROR_TIMEOUT=5		//操作超时
};

//通讯线程运行状态控制
enum{
	//1145这个值随便取的，无意义，只是为了避免外部传入一个0或者1之类恰好符合枚举值的错误输入
	ZWTHR_STOP=1145,	
	ZWTHR_RUN,
};

namespace zwccbthr{
	void wait(int milliseconds);
void zwStartLockCommThread(void);	//启动与锁具之间的通讯线程
void zwStopLockCommThread(void);	//停止与锁具之间的通讯线程
const string getString(void);

}	//namespace zwccbthr{

typedef void (cdecl *RecvMsgRotine)(const char *pszMsg);
namespace zwCfg{
	extern RecvMsgRotine g_WarnCallback;
}

//从给定的时间秒数，获取日期(YYYYMMDD)和时间(HHMMSS)字符串
void zwGetDateTimeString(time_t inTime,string &outDate,string &outTime);


#endif // zwCcbElockHdr_h__
