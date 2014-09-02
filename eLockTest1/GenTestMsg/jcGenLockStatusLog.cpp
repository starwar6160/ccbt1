#include "stdafx.h"
#include "..\\eLockTest\\jcElockTestHdr.h"

namespace jcAtmcMsg {
	//���ɲ����õ�0002��ѯ����״̬
	string & myTestMsgCheckLockStatus(string & strXML, ptree & pt) {
		//��ʼ����������
		pt.put(CCBSTR_CODE, "0002");
		pt.put(CCBSTR_NAME, "QueryForLockStatus");
		string zwDate, zwTime;
		 zwGetLocalDateTimeString(time(NULL), zwDate, zwTime);
		 pt.put(CCBSTR_DATE, zwDate);
		 pt.put(CCBSTR_TIME, zwTime);
		 pt.put(CCBSTR_DEVCODE, ATMNO_CCBTEST);
		 pt.put("root.LockMan", jcAtmcMsg::G_LOCKMAN_NAMEG);
		 pt.put("root.LockId", "ZWFAKELOCKNO1548");
		 std::ostringstream ss;
		 write_xml(ss, pt);
		 strXML = ss.str();
		 return strXML;
	}
	//���ɲ����õ�0005��ȡ��־���ģ�Ŀ������ģ�⽨��ATM��������Ϊ
	    string & myTestMsgGetLockLog(string & strXML, ptree & pt) {
		//��ʼ����������
		pt.put(CCBSTR_CODE, "0005");
		pt.put(CCBSTR_NAME, "ReadLog");
		string zwDate, zwTime;
		zwGetLocalDateTimeString(time(NULL), zwDate, zwTime);
		pt.put(CCBSTR_DATE, zwDate);
		pt.put(CCBSTR_TIME, zwTime);
		pt.put("root.BeginNo", 0);
		pt.put("root.EndNo", 23);
		std::ostringstream ss;
		write_xml(ss, pt);
		strXML = ss.str();
		return strXML;
	}

}				//namespace jcAtmcMsg{
