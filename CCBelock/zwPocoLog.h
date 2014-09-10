#ifndef zwPocoLog_h__
#define zwPocoLog_h__

#include "stdafx.h"

extern Poco::LogStream *pocoLog;
//#define ZWFATAL(x)	OutputDebugStringA("ZWFATAL"); OutputDebugStringA(x);
#define ZWDEBUG(x) OutputDebugStringA(x);
#define ZWINFO(x) OutputDebugStringA(x);
#define ZWNOTICE(x) OutputDebugStringA(x);
#define ZWWARN(x) OutputDebugStringA(x);
#define ZWERROR(x) OutputDebugStringA(x);
#define ZWFATAL(x) OutputDebugStringA(x);

#endif // zwPocoLog_h__