%module HidProtocol
using namespace std;
%include "std_string.i"
%include "typemaps.i"
%include "enums.swg"
typedef std::string String;
%{
#define _ZWUSE_AS_JNI
//#include "zwSecretBoxCCBcsWrap.h"
%}

#define _ZWUSE_AS_JNI
//%include "zwSecretBoxCCBcsWrap.h"
