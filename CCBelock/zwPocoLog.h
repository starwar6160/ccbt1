#ifndef zwPocoLog_h__
#define zwPocoLog_h__

#include "stdafx.h"

extern Poco::LogStream *g_log;
//#define ZWFATAL(x)	OutputDebugStringA("ZWFATAL"); OutputDebugStringA(x);
#define ZWFATAL(x) g_log->fatal(x);
#endif // zwPocoLog_h__