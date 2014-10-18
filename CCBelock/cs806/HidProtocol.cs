/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 3.0.2
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

namespace jclms {

public class HidProtocol {
  public static JC_SECBOX_STATUS SecboxAuth() {
    JC_SECBOX_STATUS ret = (JC_SECBOX_STATUS)HidProtocolPINVOKE.SecboxAuth();
    return ret;
  }

  public static int SecboxGetHandle() {
    int ret = HidProtocolPINVOKE.SecboxGetHandle();
    return ret;
  }

  public static void SecboxWriteData(int handleHid, int index, string dataB64) {
    HidProtocolPINVOKE.SecboxWriteData(handleHid, index, dataB64);
  }

  public static string SecboxReadData(int handleHid, int index) {
    string ret = HidProtocolPINVOKE.SecboxReadData(handleHid, index);
    return ret;
  }

}

}
