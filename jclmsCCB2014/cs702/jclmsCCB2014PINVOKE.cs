/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 3.0.2
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

namespace jclms {

class jclmsCCB2014PINVOKE {

  protected class SWIGExceptionHelper {

    public delegate void ExceptionDelegate(string message);
    public delegate void ExceptionArgumentDelegate(string message, string paramName);

    static ExceptionDelegate applicationDelegate = new ExceptionDelegate(SetPendingApplicationException);
    static ExceptionDelegate arithmeticDelegate = new ExceptionDelegate(SetPendingArithmeticException);
    static ExceptionDelegate divideByZeroDelegate = new ExceptionDelegate(SetPendingDivideByZeroException);
    static ExceptionDelegate indexOutOfRangeDelegate = new ExceptionDelegate(SetPendingIndexOutOfRangeException);
    static ExceptionDelegate invalidCastDelegate = new ExceptionDelegate(SetPendingInvalidCastException);
    static ExceptionDelegate invalidOperationDelegate = new ExceptionDelegate(SetPendingInvalidOperationException);
    static ExceptionDelegate ioDelegate = new ExceptionDelegate(SetPendingIOException);
    static ExceptionDelegate nullReferenceDelegate = new ExceptionDelegate(SetPendingNullReferenceException);
    static ExceptionDelegate outOfMemoryDelegate = new ExceptionDelegate(SetPendingOutOfMemoryException);
    static ExceptionDelegate overflowDelegate = new ExceptionDelegate(SetPendingOverflowException);
    static ExceptionDelegate systemDelegate = new ExceptionDelegate(SetPendingSystemException);

    static ExceptionArgumentDelegate argumentDelegate = new ExceptionArgumentDelegate(SetPendingArgumentException);
    static ExceptionArgumentDelegate argumentNullDelegate = new ExceptionArgumentDelegate(SetPendingArgumentNullException);
    static ExceptionArgumentDelegate argumentOutOfRangeDelegate = new ExceptionArgumentDelegate(SetPendingArgumentOutOfRangeException);

    [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="SWIGRegisterExceptionCallbacks_jclmsCCB2014")]
    public static extern void SWIGRegisterExceptionCallbacks_jclmsCCB2014(
                                ExceptionDelegate applicationDelegate,
                                ExceptionDelegate arithmeticDelegate,
                                ExceptionDelegate divideByZeroDelegate, 
                                ExceptionDelegate indexOutOfRangeDelegate, 
                                ExceptionDelegate invalidCastDelegate,
                                ExceptionDelegate invalidOperationDelegate,
                                ExceptionDelegate ioDelegate,
                                ExceptionDelegate nullReferenceDelegate,
                                ExceptionDelegate outOfMemoryDelegate, 
                                ExceptionDelegate overflowDelegate, 
                                ExceptionDelegate systemExceptionDelegate);

    [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="SWIGRegisterExceptionArgumentCallbacks_jclmsCCB2014")]
    public static extern void SWIGRegisterExceptionCallbacksArgument_jclmsCCB2014(
                                ExceptionArgumentDelegate argumentDelegate,
                                ExceptionArgumentDelegate argumentNullDelegate,
                                ExceptionArgumentDelegate argumentOutOfRangeDelegate);

    static void SetPendingApplicationException(string message) {
      SWIGPendingException.Set(new global::System.ApplicationException(message, SWIGPendingException.Retrieve()));
    }
    static void SetPendingArithmeticException(string message) {
      SWIGPendingException.Set(new global::System.ArithmeticException(message, SWIGPendingException.Retrieve()));
    }
    static void SetPendingDivideByZeroException(string message) {
      SWIGPendingException.Set(new global::System.DivideByZeroException(message, SWIGPendingException.Retrieve()));
    }
    static void SetPendingIndexOutOfRangeException(string message) {
      SWIGPendingException.Set(new global::System.IndexOutOfRangeException(message, SWIGPendingException.Retrieve()));
    }
    static void SetPendingInvalidCastException(string message) {
      SWIGPendingException.Set(new global::System.InvalidCastException(message, SWIGPendingException.Retrieve()));
    }
    static void SetPendingInvalidOperationException(string message) {
      SWIGPendingException.Set(new global::System.InvalidOperationException(message, SWIGPendingException.Retrieve()));
    }
    static void SetPendingIOException(string message) {
      SWIGPendingException.Set(new global::System.IO.IOException(message, SWIGPendingException.Retrieve()));
    }
    static void SetPendingNullReferenceException(string message) {
      SWIGPendingException.Set(new global::System.NullReferenceException(message, SWIGPendingException.Retrieve()));
    }
    static void SetPendingOutOfMemoryException(string message) {
      SWIGPendingException.Set(new global::System.OutOfMemoryException(message, SWIGPendingException.Retrieve()));
    }
    static void SetPendingOverflowException(string message) {
      SWIGPendingException.Set(new global::System.OverflowException(message, SWIGPendingException.Retrieve()));
    }
    static void SetPendingSystemException(string message) {
      SWIGPendingException.Set(new global::System.SystemException(message, SWIGPendingException.Retrieve()));
    }

    static void SetPendingArgumentException(string message, string paramName) {
      SWIGPendingException.Set(new global::System.ArgumentException(message, paramName, SWIGPendingException.Retrieve()));
    }
    static void SetPendingArgumentNullException(string message, string paramName) {
      global::System.Exception e = SWIGPendingException.Retrieve();
      if (e != null) message = message + " Inner Exception: " + e.Message;
      SWIGPendingException.Set(new global::System.ArgumentNullException(paramName, message));
    }
    static void SetPendingArgumentOutOfRangeException(string message, string paramName) {
      global::System.Exception e = SWIGPendingException.Retrieve();
      if (e != null) message = message + " Inner Exception: " + e.Message;
      SWIGPendingException.Set(new global::System.ArgumentOutOfRangeException(paramName, message));
    }

    static SWIGExceptionHelper() {
      SWIGRegisterExceptionCallbacks_jclmsCCB2014(
                                applicationDelegate,
                                arithmeticDelegate,
                                divideByZeroDelegate,
                                indexOutOfRangeDelegate,
                                invalidCastDelegate,
                                invalidOperationDelegate,
                                ioDelegate,
                                nullReferenceDelegate,
                                outOfMemoryDelegate,
                                overflowDelegate,
                                systemDelegate);

      SWIGRegisterExceptionCallbacksArgument_jclmsCCB2014(
                                argumentDelegate,
                                argumentNullDelegate,
                                argumentOutOfRangeDelegate);
    }
  }

  protected static SWIGExceptionHelper swigExceptionHelper = new SWIGExceptionHelper();

  public class SWIGPendingException {
    [global::System.ThreadStatic]
    private static global::System.Exception pendingException = null;
    private static int numExceptionsPending = 0;

    public static bool Pending {
      get {
        bool pending = false;
        if (numExceptionsPending > 0)
          if (pendingException != null)
            pending = true;
        return pending;
      } 
    }

    public static void Set(global::System.Exception e) {
      if (pendingException != null)
        throw new global::System.ApplicationException("FATAL: An earlier pending exception from unmanaged code was missed and thus not thrown (" + pendingException.ToString() + ")", e);
      pendingException = e;
      lock(typeof(jclmsCCB2014PINVOKE)) {
        numExceptionsPending++;
      }
    }

    public static global::System.Exception Retrieve() {
      global::System.Exception e = null;
      if (numExceptionsPending > 0) {
        if (pendingException != null) {
          e = pendingException;
          pendingException = null;
          lock(typeof(jclmsCCB2014PINVOKE)) {
            numExceptionsPending--;
          }
        }
      }
      return e;
    }
  }


  protected class SWIGStringHelper {

    public delegate string SWIGStringDelegate(string message);
    static SWIGStringDelegate stringDelegate = new SWIGStringDelegate(CreateString);

    [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="SWIGRegisterStringCallback_jclmsCCB2014")]
    public static extern void SWIGRegisterStringCallback_jclmsCCB2014(SWIGStringDelegate stringDelegate);

    static string CreateString(string cString) {
      return cString;
    }

    static SWIGStringHelper() {
      SWIGRegisterStringCallback_jclmsCCB2014(stringDelegate);
    }
  }

  static protected SWIGStringHelper swigStringHelper = new SWIGStringHelper();


  static jclmsCCB2014PINVOKE() {
  }


  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_ZW_AES_BLOCK_SIZE_get")]
  public static extern int ZW_AES_BLOCK_SIZE_get();

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_ZW_SM3_DGST_SIZE_get")]
  public static extern int ZW_SM3_DGST_SIZE_get();

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_JCMATCH_s_datetime_set")]
  public static extern void JCMATCH_s_datetime_set(global::System.Runtime.InteropServices.HandleRef jarg1, int jarg2);

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_JCMATCH_s_datetime_get")]
  public static extern int JCMATCH_s_datetime_get(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_JCMATCH_s_validity_set")]
  public static extern void JCMATCH_s_validity_set(global::System.Runtime.InteropServices.HandleRef jarg1, int jarg2);

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_JCMATCH_s_validity_get")]
  public static extern int JCMATCH_s_validity_get(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_JCMATCH_s_matchTimes_set")]
  public static extern void JCMATCH_s_matchTimes_set(global::System.Runtime.InteropServices.HandleRef jarg1, int jarg2);

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_JCMATCH_s_matchTimes_get")]
  public static extern int JCMATCH_s_matchTimes_get(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_new_JCMATCH")]
  public static extern global::System.IntPtr new_JCMATCH();

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_delete_JCMATCH")]
  public static extern void delete_JCMATCH(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_NUM_VALIDITY_get")]
  public static extern int NUM_VALIDITY_get();

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_JC_ATMNO_MAXLEN_get")]
  public static extern int JC_ATMNO_MAXLEN_get();

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_JC_LOCKNO_MAXLEN_get")]
  public static extern int JC_LOCKNO_MAXLEN_get();

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_JC_PSK_LEN_get")]
  public static extern int JC_PSK_LEN_get();

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_JC_INVALID_VALUE_get")]
  public static extern int JC_INVALID_VALUE_get();

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_JCINPUT_m_atmno_set")]
  public static extern void JCINPUT_m_atmno_set(global::System.Runtime.InteropServices.HandleRef jarg1, string jarg2);

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_JCINPUT_m_atmno_get")]
  public static extern string JCINPUT_m_atmno_get(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_JCINPUT_m_lockno_set")]
  public static extern void JCINPUT_m_lockno_set(global::System.Runtime.InteropServices.HandleRef jarg1, string jarg2);

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_JCINPUT_m_lockno_get")]
  public static extern string JCINPUT_m_lockno_get(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_JCINPUT_m_psk_set")]
  public static extern void JCINPUT_m_psk_set(global::System.Runtime.InteropServices.HandleRef jarg1, string jarg2);

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_JCINPUT_m_psk_get")]
  public static extern string JCINPUT_m_psk_get(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_JCINPUT_m_datetime_set")]
  public static extern void JCINPUT_m_datetime_set(global::System.Runtime.InteropServices.HandleRef jarg1, int jarg2);

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_JCINPUT_m_datetime_get")]
  public static extern int JCINPUT_m_datetime_get(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_JCINPUT_m_validity_set")]
  public static extern void JCINPUT_m_validity_set(global::System.Runtime.InteropServices.HandleRef jarg1, int jarg2);

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_JCINPUT_m_validity_get")]
  public static extern int JCINPUT_m_validity_get(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_JCINPUT_m_closecode_set")]
  public static extern void JCINPUT_m_closecode_set(global::System.Runtime.InteropServices.HandleRef jarg1, int jarg2);

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_JCINPUT_m_closecode_get")]
  public static extern int JCINPUT_m_closecode_get(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_JCINPUT_m_cmdtype_set")]
  public static extern void JCINPUT_m_cmdtype_set(global::System.Runtime.InteropServices.HandleRef jarg1, int jarg2);

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_JCINPUT_m_cmdtype_get")]
  public static extern int JCINPUT_m_cmdtype_get(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_JCINPUT_m_stepoftime_set")]
  public static extern void JCINPUT_m_stepoftime_set(global::System.Runtime.InteropServices.HandleRef jarg1, int jarg2);

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_JCINPUT_m_stepoftime_get")]
  public static extern int JCINPUT_m_stepoftime_get(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_JCINPUT_m_reverse_time_length_set")]
  public static extern void JCINPUT_m_reverse_time_length_set(global::System.Runtime.InteropServices.HandleRef jarg1, int jarg2);

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_JCINPUT_m_reverse_time_length_get")]
  public static extern int JCINPUT_m_reverse_time_length_get(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_JCINPUT_m_validity_array_set")]
  public static extern void JCINPUT_m_validity_array_set(global::System.Runtime.InteropServices.HandleRef jarg1, global::System.Runtime.InteropServices.HandleRef jarg2);

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_JCINPUT_m_validity_array_get")]
  public static extern global::System.IntPtr JCINPUT_m_validity_array_get(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_new_JCINPUT")]
  public static extern global::System.IntPtr new_JCINPUT();

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_delete_JCINPUT")]
  public static extern void delete_JCINPUT(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_JcLockNew")]
  public static extern int JcLockNew();

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_JcLockCheckInput")]
  public static extern int JcLockCheckInput(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_JcLockGetDynaCode")]
  public static extern int JcLockGetDynaCode(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_JcLockReverseVerifyDynaCode")]
  public static extern global::System.IntPtr JcLockReverseVerifyDynaCode(global::System.Runtime.InteropServices.HandleRef jarg1, int jarg2);

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_JcLockGetVersion")]
  public static extern int JcLockGetVersion();

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_JcLockDebugPrint")]
  public static extern void JcLockDebugPrint(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_ZWEFS_get")]
  public static extern int ZWEFS_get();

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_ZW_EXA_get")]
  public static extern int ZW_EXA_get();

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_zwEciesKeyPairGen")]
  public static extern int zwEciesKeyPairGen(string jarg1, string jarg2, int jarg3, string jarg4, int jarg5);

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_zwEciesEncrypt")]
  public static extern int zwEciesEncrypt(string jarg1, string jarg2, string jarg3, int jarg4, string jarg5, int jarg6, string jarg7, int jarg8);

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_zwEciesDecrypt")]
  public static extern int zwEciesDecrypt(string jarg1, string jarg2, int jarg3, string jarg4, string jarg5, string jarg6);

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_EciesGenKeyPair")]
  public static extern int EciesGenKeyPair();

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_EciesDelete")]
  public static extern void EciesDelete(int jarg1);

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_EciesGetPubKey")]
  public static extern string EciesGetPubKey(int jarg1);

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_EciesGetPriKey")]
  public static extern string EciesGetPriKey(int jarg1);

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_EciesEncrypt")]
  public static extern string EciesEncrypt(string jarg1, string jarg2);

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_EciesDecrypt")]
  public static extern string EciesDecrypt(string jarg1, string jarg2);

  [global::System.Runtime.InteropServices.DllImport("jclmsCCB2014", EntryPoint="CSharp_zwMergePsk")]
  public static extern string zwMergePsk(string jarg1);
}

}
