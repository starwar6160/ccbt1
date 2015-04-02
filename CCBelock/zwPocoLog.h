#ifndef zwPocoLog_h__
#define zwPocoLog_h__

#include "stdafx.h"

//#define ZWFATAL(x)    OutputDebugStringA("ZWFATAL"); OutputDebugStringA(x);
#define ZWINFO(x)  LOG(INFO)<<(x);
#define ZWWARN(x) LOG(WARNING)<<(x);
#define ZWERROR(x) LOG(ERROR)<<(x);
#define ZWFATAL(x) LOG(FATAL)<<(x);

#endif // zwPocoLog_h__
