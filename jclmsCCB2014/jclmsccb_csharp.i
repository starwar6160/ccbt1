%module jclmsCCB2014
using namespace std;
%include "std_string.i"
%include "typemaps.i"
%include "enums.swg"
typedef std::string String;
%{
#define _ZWUSE_AS_JNI
#include "src\jclmsCCB2014.h"
#include "zwECIES\zwEcies529.h"
%}

#define _ZWUSE_AS_JNI
%include "src\jclmsCCB2014.h"
%include "zwECIES\zwEcies529.h"
