#ifndef zwPocoLog_h__
#define zwPocoLog_h__

#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <glog/logging.h>   // glog м╥нд╪Ч


#define ZWINFO(x) LOG(INFO)<<x<<endl;OutputDebugStringA(x);
#define ZWWARN(x) LOG(WARNING)<<x<<endl;OutputDebugStringA(x);
#define ZWERROR(x) LOG(ERROR)<<x<<endl;OutputDebugStringA(x);
#define ZWFATAL(x) LOG(FATAL)<<x<<endl;OutputDebugStringA(x);


#endif // zwPocoLog_h__