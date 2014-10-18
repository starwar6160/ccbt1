/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 3.0.2
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

namespace jclms {

public class ccbElock {
  public static int Open(int lTimeOut) {
    int ret = ccbElockPINVOKE.Open(lTimeOut);
    return ret;
  }

  public static int Close() {
    int ret = ccbElockPINVOKE.Close();
    return ret;
  }

  public static int Notify(string pszMsg) {
    int ret = ccbElockPINVOKE.Notify(pszMsg);
    return ret;
  }

  public static int SetRecvMsgRotine(SWIGTYPE_p_f_p_q_const__char__void pRecvMsgFun) {
    int ret = ccbElockPINVOKE.SetRecvMsgRotine(SWIGTYPE_p_f_p_q_const__char__void.getCPtr(pRecvMsgFun));
    return ret;
  }

  public static void zwPushString(string str) {
    ccbElockPINVOKE.zwPushString(str);
  }

  public static string dbgGetLockReturnXML() {
    string ret = ccbElockPINVOKE.dbgGetLockReturnXML();
    return ret;
  }

  public static int SecboxOpen() {
    int ret = ccbElockPINVOKE.SecboxOpen();
    return ret;
  }

  public static void SecboxClose(int handleHid) {
    ccbElockPINVOKE.SecboxClose(handleHid);
  }

  public static void SecboxSendAuthReq(int handleHid) {
    ccbElockPINVOKE.SecboxSendAuthReq(handleHid);
  }

  public static int SecboxVerifyAuthRsp(int handleHid) {
    int ret = ccbElockPINVOKE.SecboxVerifyAuthRsp(handleHid);
    return ret;
  }

  public static void SecboxWriteData(int handleHid, int index, string dataB64) {
    ccbElockPINVOKE.SecboxWriteData(handleHid, index, dataB64);
  }

  public static string SecboxReadData(int handleHid, int index) {
    string ret = ccbElockPINVOKE.SecboxReadData(handleHid, index);
    return ret;
  }

}

}
