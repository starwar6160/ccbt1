// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 JCLOCKLOG_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// JCLOCKLOG_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef JCLOCKLOG_EXPORTS
#define JCLOCKLOG_API __declspec(dllexport)
#else
#define JCLOCKLOG_API __declspec(dllimport)
#endif

#ifdef  __cplusplus
extern "C" {
#endif

//黑钥匙日志处理：
//黑钥匙日志处理回调函数：
//TouchKeyLogItem是HEX格式原文，TouchKeyLogFactor是输出的人类可读字符串；
typedef void (CDECL *ReturnTouchKeyLog)(char* TouchKeyLogItem, char* TouchKeyLogFactor);
//分解黑钥匙日志的函数：(返回值是什么含义？)
//该函数通过下面的回调函数返回处理结果
JCLOCKLOG_API int SwapTouchKeyLog(char * TouchKeyLogItem);
//设置回调函数的函数：
JCLOCKLOG_API void SetReturnTouchKeyLog(ReturnTouchKeyLog _TouchKeyLogHandleFun);


//红钥匙日志处理
typedef void (CDECL *ReturnLockLog)(char* LockLogItem,char* LockLogEventTime,char* LockLogFactor);
JCLOCKLOG_API void SetReturnLockLog(ReturnLockLog _LockLogHandleFun);
JCLOCKLOG_API int SwapLockLog(char * LockLogItem,int IsFinish);



#ifdef  __cplusplus
}
#endif
