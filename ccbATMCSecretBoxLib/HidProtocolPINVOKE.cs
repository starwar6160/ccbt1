/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 3.0.2
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

namespace jclms {

class HidProtocolPINVOKE {

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

    [global::System.Runtime.InteropServices.DllImport("HidProtocol", EntryPoint="SWIGRegisterExceptionCallbacks_HidProtocol")]
    public static extern void SWIGRegisterExceptionCallbacks_HidProtocol(
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

    [global::System.Runtime.InteropServices.DllImport("HidProtocol", EntryPoint="SWIGRegisterExceptionArgumentCallbacks_HidProtocol")]
    public static extern void SWIGRegisterExceptionCallbacksArgument_HidProtocol(
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
      SWIGRegisterExceptionCallbacks_HidProtocol(
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

      SWIGRegisterExceptionCallbacksArgument_HidProtocol(
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
      lock(typeof(HidProtocolPINVOKE)) {
        numExceptionsPending++;
      }
    }

    public static global::System.Exception Retrieve() {
      global::System.Exception e = null;
      if (numExceptionsPending > 0) {
        if (pendingException != null) {
          e = pendingException;
          pendingException = null;
          lock(typeof(HidProtocolPINVOKE)) {
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

    [global::System.Runtime.InteropServices.DllImport("HidProtocol", EntryPoint="SWIGRegisterStringCallback_HidProtocol")]
    public static extern void SWIGRegisterStringCallback_HidProtocol(SWIGStringDelegate stringDelegate);

    static string CreateString(string cString) {
      return cString;
    }

    static SWIGStringHelper() {
      SWIGRegisterStringCallback_HidProtocol(stringDelegate);
    }
  }

  static protected SWIGStringHelper swigStringHelper = new SWIGStringHelper();


  static HidProtocolPINVOKE() {
  }


  [global::System.Runtime.InteropServices.DllImport("HidProtocol", EntryPoint="CSharp_Open")]
  public static extern int Open(int jarg1);

  [global::System.Runtime.InteropServices.DllImport("HidProtocol", EntryPoint="CSharp_Close")]
  public static extern int Close();

  [global::System.Runtime.InteropServices.DllImport("HidProtocol", EntryPoint="CSharp_Notify")]
  public static extern int Notify(string jarg1);

  [global::System.Runtime.InteropServices.DllImport("HidProtocol", EntryPoint="CSharp_SetRecvMsgRotine")]
  public static extern int SetRecvMsgRotine(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("HidProtocol", EntryPoint="CSharp_zwPushString")]
  public static extern void zwPushString(string jarg1);

  [global::System.Runtime.InteropServices.DllImport("HidProtocol", EntryPoint="CSharp_myLuaBridgeSendJsonAPI")]
  public static extern void myLuaBridgeSendJsonAPI(string jarg1);

  [global::System.Runtime.InteropServices.DllImport("HidProtocol", EntryPoint="CSharp_SetReturnDrives")]
  public static extern void SetReturnDrives(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("HidProtocol", EntryPoint="CSharp_ListDrives")]
  public static extern int ListDrives(string jarg1);

  [global::System.Runtime.InteropServices.DllImport("HidProtocol", EntryPoint="CSharp_OpenDrives")]
  public static extern int OpenDrives(string jarg1, string jarg2);

  [global::System.Runtime.InteropServices.DllImport("HidProtocol", EntryPoint="CSharp_CloseDrives")]
  public static extern int CloseDrives(string jarg1, string jarg2);

  [global::System.Runtime.InteropServices.DllImport("HidProtocol", EntryPoint="CSharp_SetReturnMessage")]
  public static extern void SetReturnMessage(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("HidProtocol", EntryPoint="CSharp_inputMessage")]
  public static extern int inputMessage(string jarg1, string jarg2, string jarg3);

  [global::System.Runtime.InteropServices.DllImport("HidProtocol", EntryPoint="CSharp_OpenJson")]
  public static extern int OpenJson(int jarg1);

  [global::System.Runtime.InteropServices.DllImport("HidProtocol", EntryPoint="CSharp_SendToLockJson")]
  public static extern int SendToLockJson(string jarg1);

  [global::System.Runtime.InteropServices.DllImport("HidProtocol", EntryPoint="CSharp_RecvFromLockJson")]
  public static extern string RecvFromLockJson(int jarg1);

  [global::System.Runtime.InteropServices.DllImport("HidProtocol", EntryPoint="CSharp_CloseJson")]
  public static extern int CloseJson();

  [global::System.Runtime.InteropServices.DllImport("HidProtocol", EntryPoint="CSharp_jcMulHidEnum")]
  public static extern void jcMulHidEnum(int jarg1, global::System.Runtime.InteropServices.HandleRef jarg2);

  [global::System.Runtime.InteropServices.DllImport("HidProtocol", EntryPoint="CSharp_G_JCHID_ENUM_DEV2015A_set")]
  public static extern void G_JCHID_ENUM_DEV2015A_set(global::System.Runtime.InteropServices.HandleRef jarg1);

  [global::System.Runtime.InteropServices.DllImport("HidProtocol", EntryPoint="CSharp_G_JCHID_ENUM_DEV2015A_get")]
  public static extern global::System.IntPtr G_JCHID_ENUM_DEV2015A_get();
}

}
