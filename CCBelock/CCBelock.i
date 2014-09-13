%module ccbElock
using namespace std;
%include "std_string.i"
%include "typemaps.i"
typedef std::string String;
%{
#define _ZWUSE_AS_JNI
#include "CCBelock.h"
%}

#define _ZWUSE_AS_JNI
%include "CCBelock.h"

