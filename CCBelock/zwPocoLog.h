#ifndef zwPocoLog_h__
#define zwPocoLog_h__

#include "stdafx.h"

extern Poco::LogStream *g_log;
//#define ZWFATAL(x)	OutputDebugStringA("ZWFATAL"); OutputDebugStringA(x);
#define ZWDEBUG(x) g_log->debug(x);OutputDebugStringA(x);
#define ZWINFO(x) g_log->information(x);OutputDebugStringA(x);
#define ZWNOTICE(x) g_log->notice(x);OutputDebugStringA(x);
#define ZWWARN(x) g_log->warning(x);OutputDebugStringA(x);
#define ZWERROR(x) g_log->error(x);OutputDebugStringA(x);
#define ZWFATAL(x) g_log->fatal(x);OutputDebugStringA(x);

#endif // zwPocoLog_h__