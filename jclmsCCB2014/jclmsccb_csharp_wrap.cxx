/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.11
 *
 * This file is not intended to be easily readable and contains a number of
 * coding conventions designed to improve portability and efficiency. Do not make
 * changes to this file unless you know what you are doing--modify the SWIG
 * interface file instead.
 * ----------------------------------------------------------------------------- */

#define SWIGCSHARP


#ifdef __cplusplus
/* SwigValueWrapper is described in swig.swg */
template<typename T> class SwigValueWrapper {
  struct SwigMovePointer {
    T *ptr;
    SwigMovePointer(T *p) : ptr(p) { }
    ~SwigMovePointer() { delete ptr; }
    SwigMovePointer& operator=(SwigMovePointer& rhs) { T* oldptr = ptr; ptr = 0; delete oldptr; ptr = rhs.ptr; rhs.ptr = 0; return *this; }
  } pointer;
  SwigValueWrapper& operator=(const SwigValueWrapper<T>& rhs);
  SwigValueWrapper(const SwigValueWrapper<T>& rhs);
public:
  SwigValueWrapper() : pointer(0) { }
  SwigValueWrapper& operator=(const T& t) { SwigMovePointer tmp(new T(t)); pointer = tmp; return *this; }
  operator T&() const { return *pointer.ptr; }
  T *operator&() { return pointer.ptr; }
};

template <typename T> T SwigValueInit() {
  return T();
}
#endif

/* -----------------------------------------------------------------------------
 *  This section contains generic SWIG labels for method/variable
 *  declarations/attributes, and other compiler dependent labels.
 * ----------------------------------------------------------------------------- */

/* template workaround for compilers that cannot correctly implement the C++ standard */
#ifndef SWIGTEMPLATEDISAMBIGUATOR
# if defined(__SUNPRO_CC) && (__SUNPRO_CC <= 0x560)
#  define SWIGTEMPLATEDISAMBIGUATOR template
# elif defined(__HP_aCC)
/* Needed even with `aCC -AA' when `aCC -V' reports HP ANSI C++ B3910B A.03.55 */
/* If we find a maximum version that requires this, the test would be __HP_aCC <= 35500 for A.03.55 */
#  define SWIGTEMPLATEDISAMBIGUATOR template
# else
#  define SWIGTEMPLATEDISAMBIGUATOR
# endif
#endif

/* inline attribute */
#ifndef SWIGINLINE
# if defined(__cplusplus) || (defined(__GNUC__) && !defined(__STRICT_ANSI__))
#   define SWIGINLINE inline
# else
#   define SWIGINLINE
# endif
#endif

/* attribute recognised by some compilers to avoid 'unused' warnings */
#ifndef SWIGUNUSED
# if defined(__GNUC__)
#   if !(defined(__cplusplus)) || (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4))
#     define SWIGUNUSED __attribute__ ((__unused__))
#   else
#     define SWIGUNUSED
#   endif
# elif defined(__ICC)
#   define SWIGUNUSED __attribute__ ((__unused__))
# else
#   define SWIGUNUSED
# endif
#endif

#ifndef SWIG_MSC_UNSUPPRESS_4505
# if defined(_MSC_VER)
#   pragma warning(disable : 4505) /* unreferenced local function has been removed */
# endif
#endif

#ifndef SWIGUNUSEDPARM
# ifdef __cplusplus
#   define SWIGUNUSEDPARM(p)
# else
#   define SWIGUNUSEDPARM(p) p SWIGUNUSED
# endif
#endif

/* internal SWIG method */
#ifndef SWIGINTERN
# define SWIGINTERN static SWIGUNUSED
#endif

/* internal inline SWIG method */
#ifndef SWIGINTERNINLINE
# define SWIGINTERNINLINE SWIGINTERN SWIGINLINE
#endif

/* exporting methods */
#if (__GNUC__ >= 4) || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
#  ifndef GCC_HASCLASSVISIBILITY
#    define GCC_HASCLASSVISIBILITY
#  endif
#endif

#ifndef SWIGEXPORT
# if defined(_WIN32) || defined(__WIN32__) || defined(__CYGWIN__)
#   if defined(STATIC_LINKED)
#     define SWIGEXPORT
#   else
#     define SWIGEXPORT __declspec(dllexport)
#   endif
# else
#   if defined(__GNUC__) && defined(GCC_HASCLASSVISIBILITY)
#     define SWIGEXPORT __attribute__ ((visibility("default")))
#   else
#     define SWIGEXPORT
#   endif
# endif
#endif

/* calling conventions for Windows */
#ifndef SWIGSTDCALL
# if defined(_WIN32) || defined(__WIN32__) || defined(__CYGWIN__)
#   define SWIGSTDCALL __stdcall
# else
#   define SWIGSTDCALL
# endif
#endif

/* Deal with Microsoft's attempt at deprecating C standard runtime functions */
#if !defined(SWIG_NO_CRT_SECURE_NO_DEPRECATE) && defined(_MSC_VER) && !defined(_CRT_SECURE_NO_DEPRECATE)
# define _CRT_SECURE_NO_DEPRECATE
#endif

/* Deal with Microsoft's attempt at deprecating methods in the standard C++ library */
#if !defined(SWIG_NO_SCL_SECURE_NO_DEPRECATE) && defined(_MSC_VER) && !defined(_SCL_SECURE_NO_DEPRECATE)
# define _SCL_SECURE_NO_DEPRECATE
#endif



#include <stdlib.h>
#include <string.h>
#include <stdio.h>


/* Support for throwing C# exceptions from C/C++. There are two types: 
 * Exceptions that take a message and ArgumentExceptions that take a message and a parameter name. */
typedef enum {
  SWIG_CSharpApplicationException,
  SWIG_CSharpArithmeticException,
  SWIG_CSharpDivideByZeroException,
  SWIG_CSharpIndexOutOfRangeException,
  SWIG_CSharpInvalidCastException,
  SWIG_CSharpInvalidOperationException,
  SWIG_CSharpIOException,
  SWIG_CSharpNullReferenceException,
  SWIG_CSharpOutOfMemoryException,
  SWIG_CSharpOverflowException,
  SWIG_CSharpSystemException
} SWIG_CSharpExceptionCodes;

typedef enum {
  SWIG_CSharpArgumentException,
  SWIG_CSharpArgumentNullException,
  SWIG_CSharpArgumentOutOfRangeException
} SWIG_CSharpExceptionArgumentCodes;

typedef void (SWIGSTDCALL* SWIG_CSharpExceptionCallback_t)(const char *);
typedef void (SWIGSTDCALL* SWIG_CSharpExceptionArgumentCallback_t)(const char *, const char *);

typedef struct {
  SWIG_CSharpExceptionCodes code;
  SWIG_CSharpExceptionCallback_t callback;
} SWIG_CSharpException_t;

typedef struct {
  SWIG_CSharpExceptionArgumentCodes code;
  SWIG_CSharpExceptionArgumentCallback_t callback;
} SWIG_CSharpExceptionArgument_t;

static SWIG_CSharpException_t SWIG_csharp_exceptions[] = {
  { SWIG_CSharpApplicationException, NULL },
  { SWIG_CSharpArithmeticException, NULL },
  { SWIG_CSharpDivideByZeroException, NULL },
  { SWIG_CSharpIndexOutOfRangeException, NULL },
  { SWIG_CSharpInvalidCastException, NULL },
  { SWIG_CSharpInvalidOperationException, NULL },
  { SWIG_CSharpIOException, NULL },
  { SWIG_CSharpNullReferenceException, NULL },
  { SWIG_CSharpOutOfMemoryException, NULL },
  { SWIG_CSharpOverflowException, NULL },
  { SWIG_CSharpSystemException, NULL }
};

static SWIG_CSharpExceptionArgument_t SWIG_csharp_exceptions_argument[] = {
  { SWIG_CSharpArgumentException, NULL },
  { SWIG_CSharpArgumentNullException, NULL },
  { SWIG_CSharpArgumentOutOfRangeException, NULL }
};

static void SWIGUNUSED SWIG_CSharpSetPendingException(SWIG_CSharpExceptionCodes code, const char *msg) {
  SWIG_CSharpExceptionCallback_t callback = SWIG_csharp_exceptions[SWIG_CSharpApplicationException].callback;
  if ((size_t)code < sizeof(SWIG_csharp_exceptions)/sizeof(SWIG_CSharpException_t)) {
    callback = SWIG_csharp_exceptions[code].callback;
  }
  callback(msg);
}

static void SWIGUNUSED SWIG_CSharpSetPendingExceptionArgument(SWIG_CSharpExceptionArgumentCodes code, const char *msg, const char *param_name) {
  SWIG_CSharpExceptionArgumentCallback_t callback = SWIG_csharp_exceptions_argument[SWIG_CSharpArgumentException].callback;
  if ((size_t)code < sizeof(SWIG_csharp_exceptions_argument)/sizeof(SWIG_CSharpExceptionArgument_t)) {
    callback = SWIG_csharp_exceptions_argument[code].callback;
  }
  callback(msg, param_name);
}


#ifdef __cplusplus
extern "C" 
#endif
SWIGEXPORT void SWIGSTDCALL SWIGRegisterExceptionCallbacks_jclmsCCB2014(
                                                SWIG_CSharpExceptionCallback_t applicationCallback,
                                                SWIG_CSharpExceptionCallback_t arithmeticCallback,
                                                SWIG_CSharpExceptionCallback_t divideByZeroCallback, 
                                                SWIG_CSharpExceptionCallback_t indexOutOfRangeCallback, 
                                                SWIG_CSharpExceptionCallback_t invalidCastCallback,
                                                SWIG_CSharpExceptionCallback_t invalidOperationCallback,
                                                SWIG_CSharpExceptionCallback_t ioCallback,
                                                SWIG_CSharpExceptionCallback_t nullReferenceCallback,
                                                SWIG_CSharpExceptionCallback_t outOfMemoryCallback, 
                                                SWIG_CSharpExceptionCallback_t overflowCallback, 
                                                SWIG_CSharpExceptionCallback_t systemCallback) {
  SWIG_csharp_exceptions[SWIG_CSharpApplicationException].callback = applicationCallback;
  SWIG_csharp_exceptions[SWIG_CSharpArithmeticException].callback = arithmeticCallback;
  SWIG_csharp_exceptions[SWIG_CSharpDivideByZeroException].callback = divideByZeroCallback;
  SWIG_csharp_exceptions[SWIG_CSharpIndexOutOfRangeException].callback = indexOutOfRangeCallback;
  SWIG_csharp_exceptions[SWIG_CSharpInvalidCastException].callback = invalidCastCallback;
  SWIG_csharp_exceptions[SWIG_CSharpInvalidOperationException].callback = invalidOperationCallback;
  SWIG_csharp_exceptions[SWIG_CSharpIOException].callback = ioCallback;
  SWIG_csharp_exceptions[SWIG_CSharpNullReferenceException].callback = nullReferenceCallback;
  SWIG_csharp_exceptions[SWIG_CSharpOutOfMemoryException].callback = outOfMemoryCallback;
  SWIG_csharp_exceptions[SWIG_CSharpOverflowException].callback = overflowCallback;
  SWIG_csharp_exceptions[SWIG_CSharpSystemException].callback = systemCallback;
}

#ifdef __cplusplus
extern "C" 
#endif
SWIGEXPORT void SWIGSTDCALL SWIGRegisterExceptionArgumentCallbacks_jclmsCCB2014(
                                                SWIG_CSharpExceptionArgumentCallback_t argumentCallback,
                                                SWIG_CSharpExceptionArgumentCallback_t argumentNullCallback,
                                                SWIG_CSharpExceptionArgumentCallback_t argumentOutOfRangeCallback) {
  SWIG_csharp_exceptions_argument[SWIG_CSharpArgumentException].callback = argumentCallback;
  SWIG_csharp_exceptions_argument[SWIG_CSharpArgumentNullException].callback = argumentNullCallback;
  SWIG_csharp_exceptions_argument[SWIG_CSharpArgumentOutOfRangeException].callback = argumentOutOfRangeCallback;
}


/* Callback for returning strings to C# without leaking memory */
typedef char * (SWIGSTDCALL* SWIG_CSharpStringHelperCallback)(const char *);
static SWIG_CSharpStringHelperCallback SWIG_csharp_string_callback = NULL;


#ifdef __cplusplus
extern "C" 
#endif
SWIGEXPORT void SWIGSTDCALL SWIGRegisterStringCallback_jclmsCCB2014(SWIG_CSharpStringHelperCallback callback) {
  SWIG_csharp_string_callback = callback;
}


/* Contract support */

#define SWIG_contract_assert(nullreturn, expr, msg) if (!(expr)) {SWIG_CSharpSetPendingExceptionArgument(SWIG_CSharpArgumentOutOfRangeException, msg, ""); return nullreturn; } else


#include <string>


#define _ZWUSE_AS_JNI
#include "jclmsCCB2014.h"


#ifdef __cplusplus
extern "C" {
#endif

SWIGEXPORT void SWIGSTDCALL CSharp_JCOFFLINE_s_datetime_set(void * jarg1, int jarg2) {
  jclms::jcOfflineResult *arg1 = (jclms::jcOfflineResult *) 0 ;
  int arg2 ;
  
  arg1 = (jclms::jcOfflineResult *)jarg1; 
  arg2 = (int)jarg2; 
  if (arg1) (arg1)->s_datetime = arg2;
}


SWIGEXPORT int SWIGSTDCALL CSharp_JCOFFLINE_s_datetime_get(void * jarg1) {
  int jresult ;
  jclms::jcOfflineResult *arg1 = (jclms::jcOfflineResult *) 0 ;
  int result;
  
  arg1 = (jclms::jcOfflineResult *)jarg1; 
  result = (int) ((arg1)->s_datetime);
  jresult = result; 
  return jresult;
}


SWIGEXPORT void SWIGSTDCALL CSharp_JCOFFLINE_s_validity_set(void * jarg1, int jarg2) {
  jclms::jcOfflineResult *arg1 = (jclms::jcOfflineResult *) 0 ;
  int arg2 ;
  
  arg1 = (jclms::jcOfflineResult *)jarg1; 
  arg2 = (int)jarg2; 
  if (arg1) (arg1)->s_validity = arg2;
}


SWIGEXPORT int SWIGSTDCALL CSharp_JCOFFLINE_s_validity_get(void * jarg1) {
  int jresult ;
  jclms::jcOfflineResult *arg1 = (jclms::jcOfflineResult *) 0 ;
  int result;
  
  arg1 = (jclms::jcOfflineResult *)jarg1; 
  result = (int) ((arg1)->s_validity);
  jresult = result; 
  return jresult;
}


SWIGEXPORT void * SWIGSTDCALL CSharp_new_JCOFFLINE() {
  void * jresult ;
  jclms::jcOfflineResult *result = 0 ;
  
  result = (jclms::jcOfflineResult *)new jclms::jcOfflineResult();
  jresult = (void *)result; 
  return jresult;
}


SWIGEXPORT void SWIGSTDCALL CSharp_delete_JCOFFLINE(void * jarg1) {
  jclms::jcOfflineResult *arg1 = (jclms::jcOfflineResult *) 0 ;
  
  arg1 = (jclms::jcOfflineResult *)jarg1; 
  delete arg1;
}


SWIGEXPORT void SWIGSTDCALL CSharp_JcLockInput_m_atmno_set(void * jarg1, char * jarg2) {
  jclms::JcLockInput *arg1 = (jclms::JcLockInput *) 0 ;
  std::string *arg2 = 0 ;
  
  arg1 = (jclms::JcLockInput *)jarg1; 
  if (!jarg2) {
    SWIG_CSharpSetPendingExceptionArgument(SWIG_CSharpArgumentNullException, "null string", 0);
    return ;
  }
  std::string arg2_str(jarg2);
  arg2 = &arg2_str; 
  if (arg1) (arg1)->m_atmno = *arg2;
}


SWIGEXPORT char * SWIGSTDCALL CSharp_JcLockInput_m_atmno_get(void * jarg1) {
  char * jresult ;
  jclms::JcLockInput *arg1 = (jclms::JcLockInput *) 0 ;
  std::string *result = 0 ;
  
  arg1 = (jclms::JcLockInput *)jarg1; 
  result = (std::string *) & ((arg1)->m_atmno);
  jresult = SWIG_csharp_string_callback(result->c_str()); 
  return jresult;
}


SWIGEXPORT void SWIGSTDCALL CSharp_JcLockInput_m_lockno_set(void * jarg1, char * jarg2) {
  jclms::JcLockInput *arg1 = (jclms::JcLockInput *) 0 ;
  std::string *arg2 = 0 ;
  
  arg1 = (jclms::JcLockInput *)jarg1; 
  if (!jarg2) {
    SWIG_CSharpSetPendingExceptionArgument(SWIG_CSharpArgumentNullException, "null string", 0);
    return ;
  }
  std::string arg2_str(jarg2);
  arg2 = &arg2_str; 
  if (arg1) (arg1)->m_lockno = *arg2;
}


SWIGEXPORT char * SWIGSTDCALL CSharp_JcLockInput_m_lockno_get(void * jarg1) {
  char * jresult ;
  jclms::JcLockInput *arg1 = (jclms::JcLockInput *) 0 ;
  std::string *result = 0 ;
  
  arg1 = (jclms::JcLockInput *)jarg1; 
  result = (std::string *) & ((arg1)->m_lockno);
  jresult = SWIG_csharp_string_callback(result->c_str()); 
  return jresult;
}


SWIGEXPORT void SWIGSTDCALL CSharp_JcLockInput_m_psk_set(void * jarg1, char * jarg2) {
  jclms::JcLockInput *arg1 = (jclms::JcLockInput *) 0 ;
  std::string *arg2 = 0 ;
  
  arg1 = (jclms::JcLockInput *)jarg1; 
  if (!jarg2) {
    SWIG_CSharpSetPendingExceptionArgument(SWIG_CSharpArgumentNullException, "null string", 0);
    return ;
  }
  std::string arg2_str(jarg2);
  arg2 = &arg2_str; 
  if (arg1) (arg1)->m_psk = *arg2;
}


SWIGEXPORT char * SWIGSTDCALL CSharp_JcLockInput_m_psk_get(void * jarg1) {
  char * jresult ;
  jclms::JcLockInput *arg1 = (jclms::JcLockInput *) 0 ;
  std::string *result = 0 ;
  
  arg1 = (jclms::JcLockInput *)jarg1; 
  result = (std::string *) & ((arg1)->m_psk);
  jresult = SWIG_csharp_string_callback(result->c_str()); 
  return jresult;
}


SWIGEXPORT void SWIGSTDCALL CSharp_JcLockInput_m_datetime_set(void * jarg1, int jarg2) {
  jclms::JcLockInput *arg1 = (jclms::JcLockInput *) 0 ;
  int arg2 ;
  
  arg1 = (jclms::JcLockInput *)jarg1; 
  arg2 = (int)jarg2; 
  if (arg1) (arg1)->m_datetime = arg2;
}


SWIGEXPORT int SWIGSTDCALL CSharp_JcLockInput_m_datetime_get(void * jarg1) {
  int jresult ;
  jclms::JcLockInput *arg1 = (jclms::JcLockInput *) 0 ;
  int result;
  
  arg1 = (jclms::JcLockInput *)jarg1; 
  result = (int) ((arg1)->m_datetime);
  jresult = result; 
  return jresult;
}


SWIGEXPORT void SWIGSTDCALL CSharp_JcLockInput_m_validity_set(void * jarg1, int jarg2) {
  jclms::JcLockInput *arg1 = (jclms::JcLockInput *) 0 ;
  int arg2 ;
  
  arg1 = (jclms::JcLockInput *)jarg1; 
  arg2 = (int)jarg2; 
  if (arg1) (arg1)->m_validity = arg2;
}


SWIGEXPORT int SWIGSTDCALL CSharp_JcLockInput_m_validity_get(void * jarg1) {
  int jresult ;
  jclms::JcLockInput *arg1 = (jclms::JcLockInput *) 0 ;
  int result;
  
  arg1 = (jclms::JcLockInput *)jarg1; 
  result = (int) ((arg1)->m_validity);
  jresult = result; 
  return jresult;
}


SWIGEXPORT void SWIGSTDCALL CSharp_JcLockInput_m_closecode_set(void * jarg1, int jarg2) {
  jclms::JcLockInput *arg1 = (jclms::JcLockInput *) 0 ;
  int arg2 ;
  
  arg1 = (jclms::JcLockInput *)jarg1; 
  arg2 = (int)jarg2; 
  if (arg1) (arg1)->m_closecode = arg2;
}


SWIGEXPORT int SWIGSTDCALL CSharp_JcLockInput_m_closecode_get(void * jarg1) {
  int jresult ;
  jclms::JcLockInput *arg1 = (jclms::JcLockInput *) 0 ;
  int result;
  
  arg1 = (jclms::JcLockInput *)jarg1; 
  result = (int) ((arg1)->m_closecode);
  jresult = result; 
  return jresult;
}


SWIGEXPORT void SWIGSTDCALL CSharp_JcLockInput_m_cmdtype_set(void * jarg1, int jarg2) {
  jclms::JcLockInput *arg1 = (jclms::JcLockInput *) 0 ;
  jclms::JCCMD arg2 ;
  
  arg1 = (jclms::JcLockInput *)jarg1; 
  arg2 = (jclms::JCCMD)jarg2; 
  if (arg1) (arg1)->m_cmdtype = arg2;
}


SWIGEXPORT int SWIGSTDCALL CSharp_JcLockInput_m_cmdtype_get(void * jarg1) {
  int jresult ;
  jclms::JcLockInput *arg1 = (jclms::JcLockInput *) 0 ;
  jclms::JCCMD result;
  
  arg1 = (jclms::JcLockInput *)jarg1; 
  result = (jclms::JCCMD) ((arg1)->m_cmdtype);
  jresult = result; 
  return jresult;
}


SWIGEXPORT void * SWIGSTDCALL CSharp_new_JcLockInput() {
  void * jresult ;
  jclms::JcLockInput *result = 0 ;
  
  result = (jclms::JcLockInput *)new jclms::JcLockInput();
  jresult = (void *)result; 
  return jresult;
}


SWIGEXPORT void SWIGSTDCALL CSharp_JcLockInput_DebugPrint(void * jarg1) {
  jclms::JcLockInput *arg1 = (jclms::JcLockInput *) 0 ;
  
  arg1 = (jclms::JcLockInput *)jarg1; 
  (arg1)->DebugPrint();
}


SWIGEXPORT int SWIGSTDCALL CSharp_JcLockInput_CheckInput(void * jarg1) {
  int jresult ;
  jclms::JcLockInput *arg1 = (jclms::JcLockInput *) 0 ;
  jclms::JCERROR result;
  
  arg1 = (jclms::JcLockInput *)jarg1; 
  result = (jclms::JCERROR)(arg1)->CheckInput();
  jresult = result; 
  return jresult;
}


SWIGEXPORT void SWIGSTDCALL CSharp_delete_JcLockInput(void * jarg1) {
  jclms::JcLockInput *arg1 = (jclms::JcLockInput *) 0 ;
  
  arg1 = (jclms::JcLockInput *)jarg1; 
  delete arg1;
}


SWIGEXPORT int SWIGSTDCALL CSharp_zwGetDynaCode(void * jarg1) {
  int jresult ;
  jclms::JcLockInput *arg1 = 0 ;
  int result;
  
  arg1 = (jclms::JcLockInput *)jarg1;
  if (!arg1) {
    SWIG_CSharpSetPendingExceptionArgument(SWIG_CSharpArgumentNullException, "jclms::JcLockInput const & type is null", 0);
    return 0;
  } 
  result = (int)jclms::zwGetDynaCode((jclms::JcLockInput const &)*arg1);
  jresult = result; 
  return jresult;
}


SWIGEXPORT int SWIGSTDCALL CSharp_zwVerifyDynaCode(void * jarg1, int jarg2) {
  int jresult ;
  jclms::JcLockInput *arg1 = 0 ;
  int arg2 ;
  jclms::JCERROR result;
  
  arg1 = (jclms::JcLockInput *)jarg1;
  if (!arg1) {
    SWIG_CSharpSetPendingExceptionArgument(SWIG_CSharpArgumentNullException, "jclms::JcLockInput const & type is null", 0);
    return 0;
  } 
  arg2 = (int)jarg2; 
  result = (jclms::JCERROR)jclms::zwVerifyDynaCode((jclms::JcLockInput const &)*arg1,arg2);
  jresult = result; 
  return jresult;
}


SWIGEXPORT void * SWIGSTDCALL CSharp_zwOfflineVerifyDynaCode(void * jarg1, int jarg2) {
  void * jresult ;
  jclms::JcLockInput *arg1 = 0 ;
  int arg2 ;
  jclms::JCOFFLINE result;
  
  arg1 = (jclms::JcLockInput *)jarg1;
  if (!arg1) {
    SWIG_CSharpSetPendingExceptionArgument(SWIG_CSharpArgumentNullException, "jclms::JcLockInput const & type is null", 0);
    return 0;
  } 
  arg2 = (int)jarg2; 
  result = jclms::zwOfflineVerifyDynaCode((jclms::JcLockInput const &)*arg1,arg2);
  jresult = new jclms::JCOFFLINE((const jclms::JCOFFLINE &)result); 
  return jresult;
}


SWIGEXPORT int SWIGSTDCALL CSharp_getVersion() {
  int jresult ;
  int result;
  
  result = (int)jclms::getVersion();
  jresult = result; 
  return jresult;
}


SWIGEXPORT int SWIGSTDCALL CSharp_ZW_AES_BLOCK_SIZE_get() {
  int jresult ;
  int result;
  
  result = (int)((128/8));
  jresult = result; 
  return jresult;
}


SWIGEXPORT int SWIGSTDCALL CSharp_ZW_SM3_DGST_SIZE_get() {
  int jresult ;
  int result;
  
  result = (int)((256/8));
  jresult = result; 
  return jresult;
}


SWIGEXPORT void * SWIGSTDCALL CSharp_new_zwHexTool__SWIG_0(char * jarg1) {
  void * jresult ;
  char *arg1 = (char *) 0 ;
  zwTools::zwHexTool *result = 0 ;
  
  arg1 = (char *)jarg1; 
  result = (zwTools::zwHexTool *)new zwTools::zwHexTool((char const *)arg1);
  jresult = (void *)result; 
  return jresult;
}


SWIGEXPORT void * SWIGSTDCALL CSharp_new_zwHexTool__SWIG_1(void * jarg1, int jarg2) {
  void * jresult ;
  void *arg1 = (void *) 0 ;
  int arg2 ;
  zwTools::zwHexTool *result = 0 ;
  
  arg1 = (void *)jarg1; 
  arg2 = (int)jarg2; 
  result = (zwTools::zwHexTool *)new zwTools::zwHexTool((void const *)arg1,arg2);
  jresult = (void *)result; 
  return jresult;
}


SWIGEXPORT void SWIGSTDCALL CSharp_delete_zwHexTool(void * jarg1) {
  zwTools::zwHexTool *arg1 = (zwTools::zwHexTool *) 0 ;
  
  arg1 = (zwTools::zwHexTool *)jarg1; 
  delete arg1;
}


SWIGEXPORT char * SWIGSTDCALL CSharp_zwHexTool_getBin(void * jarg1) {
  char * jresult ;
  zwTools::zwHexTool *arg1 = (zwTools::zwHexTool *) 0 ;
  char *result = 0 ;
  
  arg1 = (zwTools::zwHexTool *)jarg1; 
  result = (char *)(arg1)->getBin();
  jresult = SWIG_csharp_string_callback((const char *)result); 
  return jresult;
}


SWIGEXPORT int SWIGSTDCALL CSharp_zwHexTool_getBinLen(void * jarg1) {
  int jresult ;
  zwTools::zwHexTool *arg1 = (zwTools::zwHexTool *) 0 ;
  int result;
  
  arg1 = (zwTools::zwHexTool *)jarg1; 
  result = (int)(arg1)->getBinLen();
  jresult = result; 
  return jresult;
}


SWIGEXPORT int SWIGSTDCALL CSharp_zwHexTool_getPadedLen(void * jarg1) {
  int jresult ;
  zwTools::zwHexTool *arg1 = (zwTools::zwHexTool *) 0 ;
  int result;
  
  arg1 = (zwTools::zwHexTool *)jarg1; 
  result = (int)(arg1)->getPadedLen();
  jresult = result; 
  return jresult;
}


SWIGEXPORT int SWIGSTDCALL CSharp_zwHexTool_getXXTEABlockNum(void * jarg1) {
  int jresult ;
  zwTools::zwHexTool *arg1 = (zwTools::zwHexTool *) 0 ;
  int result;
  
  arg1 = (zwTools::zwHexTool *)jarg1; 
  result = (int)(arg1)->getXXTEABlockNum();
  jresult = result; 
  return jresult;
}


SWIGEXPORT void SWIGSTDCALL CSharp_zwHexTool_PrintBin(void * jarg1) {
  zwTools::zwHexTool *arg1 = (zwTools::zwHexTool *) 0 ;
  
  arg1 = (zwTools::zwHexTool *)jarg1; 
  (arg1)->PrintBin();
}


SWIGEXPORT char * SWIGSTDCALL CSharp_zwHexTool_getCArrayStr(void * jarg1) {
  char * jresult ;
  zwTools::zwHexTool *arg1 = (zwTools::zwHexTool *) 0 ;
  char *result = 0 ;
  
  arg1 = (zwTools::zwHexTool *)jarg1; 
  result = (char *)(arg1)->getCArrayStr();
  jresult = SWIG_csharp_string_callback((const char *)result); 
  return jresult;
}


#ifdef __cplusplus
}
#endif

