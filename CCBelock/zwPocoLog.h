#ifndef zwPocoLog_h__
#define zwPocoLog_h__

#include "stdafx.h"

//extern Poco::LogStream * pocoLog;
//#define ZWFATAL(x)    OutputDebugStringA("ZWFATAL"); OutputDebugStringA(x);
#define ZWDEBUG(x) LOG(INFO)<<(x);OutputDebugStringA(x);
#define ZWINFO(x)  LOG(INFO)<<(x);OutputDebugStringA(x);
#define ZWNOTICE(x) LOG(WARNING)<<(x);OutputDebugStringA(x);
#define ZWWARN(x) LOG(WARNING)<<(x);OutputDebugStringA(x);
#define ZWERROR(x) LOG(ERROR)<<(x);OutputDebugStringA(x);
#define ZWFATAL(x) LOG(FATAL)<<(x);OutputDebugStringA(x);

#endif // zwPocoLog_h__
