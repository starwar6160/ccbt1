#include "stdafx.h"
#include "zwCcbElockHdr.h"
#include "myConvIntHdr.h"
extern Poco::LogStream * pocoLog;
namespace jcAtmcConvertDLL {
	//��ȡ������
	void zwconvReadCloseCodeDown(const ptree & ptccb, ptree & ptjc) {
		ZWFUNCTRACE
		    //>> ��ȡ������
		    //����
		    //      "command": JCSTR_READ_CLOSECODE,
		    //      "Lock_Time"     
		    //Ӧ��
		    //"Command":JCSTR_READ_CLOSECODE
		    ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,
			     JCSTR_READ_CLOSECODE);
		//�����ַ�����ʽ�����ں�ʱ���ֶκϳ�ת��ΪUTC����.��ʼ
		string ccbDate = ptccb.get < string > (CCBSTR_DATE);
		string ccbTime = ptccb.get < string > (CCBSTR_TIME);
		time_t ccbUTCSec = 0;
		zwCCBDateTime2UTC(ccbDate.c_str(), ccbTime.c_str(), &ccbUTCSec);
		assert(ccbUTCSec > 1400 * 1000 * 1000);
		//�����ַ�����ʽ�����ں�ʱ���ֶκϳ�ת��ΪUTC����.����

		ptjc.put("Lock_Time", ccbUTCSec);
	} void zwconvReadCloseCodeUp(const ptree & ptjc, ptree & ptccb) {
		ZWFUNCTRACE
		    //��ȡ������ Ӧ��
		    //      ���״���        TransCode       ��      ֵ��0004
		    //      ��������        TransName       ��      ֵ��ReadShutLockCode
		    //      ��������        TransDate       ��      ֵ��YYYYMMDD����20140401
		    //      ����ʱ��        TransTime       ��      ֵ��hhmmss����134050
		    //      ATM�豸���     DevCode ��      ֵ������12λ�豸���
		    //      ���߳���        LockMan ��      ֵ�������Զ����������̲�ͬ������
		    //      ���߱��        LockId  ��      ֵ�������Զ�������Ψһ���
		    //      ������  ShutLockcode    ��      ֵ��������
		    //���õ���ʽ������
		    ptccb.put(CCBSTR_CODE, "0004");
		assert("ReadShutLockCode" == ns_ReadCloseCodeName);
		ptccb.put(CCBSTR_NAME, ns_ReadCloseCodeName);
		string zwDate, zwTime;
		zwGetLocalDateTimeString(time(NULL), zwDate, zwTime);
		ptccb.put(CCBSTR_DATE, zwDate);
		ptccb.put(CCBSTR_TIME, zwTime);

		ptccb.put(CCBSTR_DEVCODE, ptjc.get < string > ("Atm_Serial"));
		ptccb.put("root.LockMan", LOCKMAN_NAME);
		ptccb.put("root.LockId", ptjc.get < string > ("Lock_Serial"));
		ptccb.put("root.ShutLockcode", ptjc.get < int >("Code"));
	}

	void zwconvRecvInitCloseCodeDown(const ptree &, ptree & ptjc) {
		ZWFUNCTRACE
		    ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,
			     "Lock_Close_Code_Lock");
	}

	void zwconvRecvInitCloseCodeUp(const ptree & ptjc, ptree & ptccb) {
		ZWFUNCTRACE ptccb.put(CCBSTR_CODE, "1000");
		ptccb.put(CCBSTR_NAME, "SendShutLockCode");
		string zwDate, zwTime;
		zwGetLocalDateTimeString(time(NULL), zwDate, zwTime);
		ptccb.put(CCBSTR_DATE, zwDate);
		ptccb.put(CCBSTR_TIME, zwTime);
		//���߷��ͳ�ʼ������ʱ��ATM���Ӧ���Ѿ��ڼ��������л�ã�����
		//1.1�汾��������û�и��������Դ˴����ܻ�������
		ptccb.put(CCBSTR_DEVCODE, ptjc.get < string > ("Atm_Serial"));
		ptccb.put("root.LockMan", LOCKMAN_NAME);
		ptccb.put("root.ShutLockcode", ptjc.get < int >("Code"));
	}

	void zwconvRecvVerifyCodeDown(const ptree &, ptree & ptjc) {
		ZWFUNCTRACE
		    ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,
			     "Lock_Open_Ident");
	}

	void zwconvRecvVerifyCodeUp(const ptree & ptjc, ptree & ptccb) {
		try {
			OutputDebugStringA
			    ("20141017.1116.zwconvRecvVerifyCodeUp.MaHaoTest1");
			ZWFUNCTRACE ptccb.put(CCBSTR_CODE, "1002");
			OutputDebugStringA
			    ("20141017.1116.zwconvRecvVerifyCodeUp.MaHaoTest2");
			ptccb.put(CCBSTR_NAME, "SendUnLockIdent");
			OutputDebugStringA
			    ("20141017.1116.zwconvRecvVerifyCodeUp.MaHaoTest3");
			string zwDate, zwTime;
			zwGetLocalDateTimeString(time(NULL), zwDate, zwTime);
			ptccb.put(CCBSTR_DATE, zwDate);
			ptccb.put(CCBSTR_TIME, zwTime);
			//���߷��ͳ�ʼ������ʱ��ATM���Ӧ���Ѿ��ڼ��������л�ã�����
			//1.1�汾��������û�и��������Դ˴����ܻ�������
			OutputDebugStringA
			    ("20141017.1116.zwconvRecvVerifyCodeUp.MaHaoTest4");
			ptccb.put(CCBSTR_DEVCODE,
				  ptjc.get < string > ("Atm_Serial"));
			ptccb.put("root.LockMan", LOCKMAN_NAME);
			OutputDebugStringA
			    ("20141017.1116.zwconvRecvVerifyCodeUp.MaHaoTest5");
			ptccb.put("root.LockId",
				  ptjc.get < string > ("Lock_Serial"));
			OutputDebugStringA
			    ("20141017.1116.zwconvRecvVerifyCodeUp.MaHaoTest6");
			//�ؼ�����֤�뱾��
			ptccb.put("root.UnLockIdentInfo",
				  ptjc.get < int >("Lock_Ident_Info"));
			OutputDebugStringA
			    ("20141017.1116.zwconvRecvVerifyCodeUp.MaHaoTest7");
		}
		catch(...) {
			string errmsg =
			    "zwconvRecvVerifyCodeUp�������쳣�����ԭ����JcEloc���ص�Json��ȱ��ĳЩ�ؼ��ֶα���Atm_Serial��Lock_Serial��Lock_Ident_Info";
			OutputDebugStringA(errmsg.c_str());
			pocoLog->error() << errmsg << endl;
		}
	}

}				//namespace jcAtmcConvertDLL{
