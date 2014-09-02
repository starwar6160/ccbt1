#include "stdafx.h"
#include "zwCcbElockHdr.h"
#include "myConvIntHdr.h"

namespace jcAtmcConvertDLL {
	//ʱ��ͬ��
	void zwconvTimeSyncDown(const ptree &, ptree & ptjc) {
		ZWFUNCTRACE
		    ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE, JCSTR_TIME_SYNC);
		ptjc.put < time_t > ("Lock_Time", time(NULL));
	}
	
	//ʱ��ͬ�� 
	void zwconvTimeSyncUp(const ptree & ptjc, ptree & ptccb) {
		ZWFUNCTRACE
		    //���õ���ʽ������
		    ptccb.put(CCBSTR_CODE, "0003");
		ptccb.put(CCBSTR_NAME, "TimeSync");	//ʹ�û������ڴ��е�ֵ
		string zwDate, zwTime;
		zwGetLocalDateTimeString(ptjc.get < time_t > ("Lock_Time"),
					 zwDate, zwTime);
		ptccb.put(CCBSTR_DATE, zwDate);
		ptccb.put(CCBSTR_TIME, zwTime);
		//////////////////////////////////////////////////////////////////////////
		ptccb.put(CCBSTR_DEVCODE, ns_ccbAtmno);	//ʹ�û������ڴ��е�ֵ
		ptccb.put("root.LockMan", LOCKMAN_NAME);
		ptccb.put("root.LockId", ptjc.get < string > ("Lock_Serial"));
		//////////////////////////////////////////////////////////////////////////
		//���ò���
		time_t exTimeValue = ptjc.get < time_t > ("Ex_Syn_Time");
		string exDate, exTime;
		zwGetLocalDateTimeString(exTimeValue, exDate, exTime);
		ptccb.put("root.ExSynDate", exDate);
		ptccb.put("root.ExSynTime", exTime);
	}

	//////////////////////////////////////////////////////////////////////////

	void zwconvLockReqTimeSyncDown(const ptree &, ptree & ptjc) {
		ZWFUNCTRACE
		    ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,
			     jcAtmcConvertDLL::JCSTR_REQUEST_TIME_SYNC);
		ptjc.put("Lock_Time", time(NULL));
	}

	void zwconvLockReqTimeSyncUp(const ptree & ptjc, ptree & ptccb) {
		ZWFUNCTRACE ptccb.put(CCBSTR_CODE, "1003");
		ptccb.put(CCBSTR_NAME, "TimeSync");
		string zwDate, zwTime;
		zwGetLocalDateTimeString(time(NULL), zwDate, zwTime);
		ptccb.put(CCBSTR_DATE, zwDate);
		ptccb.put(CCBSTR_TIME, zwTime);
		//���߷��ͳ�ʼ������ʱ��ATM���Ӧ���Ѿ��ڼ��������л�ã�����
		//1.1�汾��������û�и��������Դ˴����ܻ�������
		ptccb.put(CCBSTR_DEVCODE, ns_ccbAtmno);
		ptccb.put("root.LockMan", LOCKMAN_NAME);
		ptccb.put("root.LockId", ptjc.get < string > ("Lock_Serial"));
	}

}				//namespace jcAtmcConvertDLL{
