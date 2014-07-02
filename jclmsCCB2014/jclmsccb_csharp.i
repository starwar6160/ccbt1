%module jclmsccb
using namespace std;
%include "std_string.i"
%include "typemaps.i"
%include "enums.swg"
typedef std::string String;
%{
#define _ZWUSE_AS_JNI
#include "jclmsCCB2014.h"
%}

#define _ZWUSE_AS_JNI
%include "jclmsCCB2014.h"
