// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 CCBELOCK_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// CCBELOCK_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef CCBELOCK_EXPORTS
#define CCBELOCK_API __declspec(dllexport)
#else
#define CCBELOCK_API __declspec(dllimport)
#endif

typedef void (cdecl *RecvMsgRotine)(const char *pszMsg);

CCBELOCK_API long	Open(long lTimeOut);
CCBELOCK_API long	Close();
CCBELOCK_API long	Notify(const char *pszMsg);
CCBELOCK_API int	SetRecvMsgRotine(RecvMsgRotine pRecvMsgFun);
/////////////////////////////////测试用函数/////////////////////////////////////////
//报文类型
typedef enum jcmsg_ccb_elock_t{
	JCMSG_INVALID_TYPE,	//无效报文类型
	//ATMC TO LOCK MSG:	
	JCMSG_LOCK_ACTIVE_REQUEST,
	JCMSG_GET_LOCK_ACTIVEINFO,
	JCMSG_QUERY_LOCK_STATUS,
	JCMSG_TIME_SYNC,
	JCMSG_GET_CLOSECODE,
	JCMSG_GET_LOCK_LOG,
	//LOCK TO ATMC MSG:
	JCMSG_SEND_CLOSECODE,
	JCMSG_PUSH_WARNING,
	JCMSG_SEND_UNLOCK_CERTCODE,
	JCMSG_REQUEST_TIME_SYNC,
	JCMSG_TEST730A1,	//测试用
}JC_MSG_TYPE;

const JC_MSG_TYPE zwXML2Json(const string &inXML,string &outJson);
const JC_MSG_TYPE zwJson2XML(const string &inJson,string &outXML);

enum JC_CCBELOCK_ERROR_CODE{
	ELOCK_ERROR_SUCCESS=0,		//操作成功
	ELOCK_ERROR_NOTSUPPORT=1,	//不支持的接口
	ELOCK_ERROR_HARDWAREERROR=2,//硬件故障
	ELOCK_ERROR_PARAMINVALID=3,	//参数非法
	ELOCK_ERROR_CONNECTLOST=4,	//失去设备连接
	ELOCK_ERROR_TIMEOUT=5		//操作超时
};

