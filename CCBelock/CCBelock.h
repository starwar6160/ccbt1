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
CCBELOCK_API int	zwwsTest1(const char *pMsg);
CCBELOCK_API void zwTestWebSocket();

enum JC_CCBELOCK_ERROR_CODE{
	ELOCK_ERROR_SUCCESS=0,		//操作成功
	ELOCK_ERROR_NOTSUPPORT=1,	//不支持的接口
	ELOCK_ERROR_HARDWAREERROR=2,//硬件故障
	ELOCK_ERROR_PARAMINVALID=3,	//参数非法
	ELOCK_ERROR_CONNECTLOST=4,	//失去设备连接
	ELOCK_ERROR_TIMEOUT=5		//操作超时
};