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

#endif // zwCcbElockHdr_h__
