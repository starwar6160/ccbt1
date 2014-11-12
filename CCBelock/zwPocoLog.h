#ifndef zwPocoLog_h__
#define zwPocoLog_h__

#include "stdafx.h"

extern Poco::LogStream * pocoLog;
//#define ZWFATAL(x)    OutputDebugStringA("ZWFATAL"); OutputDebugStringA(x);
#define ZWDEBUG(x) pocoLog->debug(x);OutputDebugStringA(x);
#define ZWINFO(x) pocoLog->information(x);OutputDebugStringA(x);
#define ZWNOTICE(x) pocoLog->notice(x);OutputDebugStringA(x);
#define ZWWARN(x) pocoLog->warning(x);OutputDebugStringA(x);
#define ZWERROR(x) pocoLog->error(x);OutputDebugStringA(x);
#define ZWFATAL(x) pocoLog->fatal(x);OutputDebugStringA(x);

#endif // zwPocoLog_h__
