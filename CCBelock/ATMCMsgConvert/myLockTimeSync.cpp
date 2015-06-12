#include "stdafx.h"
#include "zwCcbElockHdr.h"
#include "myConvIntHdr.h"

namespace jcAtmcConvertDLL {

	time_t myGetCcbUTC(const ptree & ptccb)
	{
		string ccbDate = ptccb.get < string > (CCBSTR_DATE);
		string ccbTime = ptccb.get < string > (CCBSTR_TIME);	
		time_t ccbUTCSec = 0;
		zwCCBDateTime2UTC(ccbDate.c_str(), ccbTime.c_str(),
			&ccbUTCSec);
		return ccbUTCSec;
	}

	//ʱ��ͬ��
	void zwconvTimeSyncDown(const ptree & ptccb, ptree & ptjc) {
		//ZWFUNCTRACE
		    ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE, JCSTR_TIME_SYNC);
		//�����ַ�����ʽ�����ں�ʱ���ֶκϳ�ת��ΪUTC����.��ʼ
		//time_t nowSec = time(NULL);
		time_t ccbUTCSec = myGetCcbUTC(ptccb);
		//string ccbDate = ptccb.get < string > (CCBSTR_DATE);
		//string ccbTime = ptccb.get < string > (CCBSTR_TIME);	
		// zwCCBDateTime2UTC(ccbDate.c_str(), ccbTime.c_str(),
		//		   &ccbUTCSec);
		 assert(ccbUTCSec > 1400 * 1000 * 1000);

		//�����ַ�����ʽ�����ں�ʱ���ֶκϳ�ת��ΪUTC����.����
#ifdef _DEBUG
		 //printf("��ǰ����ʱ��time(NULL)=%u\n",time(NULL));
		 //printf("zwconvTimeSyncDown ccbUTCSec from ATMVH is %u\n",ccbUTCSec);
#endif // _DEBUG
		 ptjc.put < time_t > ("Lock_Time", ccbUTCSec);
		//ptjc.put < time_t > ("Lock_Time", nowSec);
	}
	//ʱ��ͬ��  
	void zwconvTimeSyncUp(const ptree & ptjc, ptree & ptccb) {
		//ZWFUNCTRACE
		    //���õ���ʽ������
		    ptccb.put(CCBSTR_CODE, "0003");
		ptccb.put(CCBSTR_NAME, "TimeSync");	//ʹ�û������ڴ��е�ֵ
		string zwDate, zwTime;
		zwGetLocalDateTimeString(ptjc.get < time_t > ("Lock_Time"),
					 zwDate, zwTime);
		ptccb.put(CCBSTR_DATE, zwDate);
		ptccb.put(CCBSTR_TIME, zwTime);
		//////////////////////////////////////////////////////////////////////////
		ptccb.put(CCBSTR_DEVCODE, ptjc.get < string > ("Atm_Serial"));	//ʹ�û������ڴ��е�ֵ
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

	void zwconvLockReqTimeSyncDown(const ptree & ptccb, ptree & ptjc) {
		ZWFUNCTRACE
		    ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,
			     jcAtmcConvertDLL::JCSTR_REQUEST_TIME_SYNC);
		//�����ַ�����ʽ�����ں�ʱ���ֶκϳ�ת��ΪUTC����.��ʼ
		string ccbDate = ptccb.get < string > (CCBSTR_DATE);
		string ccbTime = ptccb.get < string > (CCBSTR_TIME);
		time_t ccbUTCSec = 0;
		zwCCBDateTime2UTC(ccbDate.c_str(), ccbTime.c_str(), &ccbUTCSec);
		assert(ccbUTCSec > 1400 * 1000 * 1000);
		//�����ַ�����ʽ�����ں�ʱ���ֶκϳ�ת��ΪUTC����.����

		ptjc.put("Lock_Time", ccbUTCSec);
	}

	void zwconvLockReqTimeSyncUp(const ptree & ptjc, ptree & ptccb) {
		ZWFUNCTRACE 
		ptccb.put(CCBSTR_CODE, "1003");
		ptccb.put(CCBSTR_NAME, "TimeSync");
		string zwDate, zwTime;
		zwGetLocalDateTimeString(time(NULL), zwDate, zwTime);
		ptccb.put(CCBSTR_DATE, zwDate);
		ptccb.put(CCBSTR_TIME, zwTime);
		//���߷��ͳ�ʼ������ʱ��ATM���Ӧ���Ѿ��ڼ��������л�ã�����
		//1.1�汾��������û�и��������Դ˴����ܻ�������
		ptccb.put(CCBSTR_DEVCODE, ptjc.get < string > ("Atm_Serial"));
		ptccb.put("root.LockMan", LOCKMAN_NAME);
		ptccb.put("root.LockId", ptjc.get < string > ("Lock_Serial"));
	}
//////////////////////////////////////////////////////////////////////////
//20141111����.�¶��񶯴���������֧��

	void zwconvTemptureSenseDown(const ptree & ptccb, ptree & ptjc) {
		ZWFUNCTRACE
		    ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,
			     jcAtmcConvertDLL::JCSTR_SENSE_TEMPTURE);
		ptjc.put("Temperature", ptccb.get < int >("root.Temperature"));
	}

	void zwconvTemptureSenseUp(const ptree & ptjc, ptree & ptccb) {
		ZWFUNCTRACE ptccb.put(CCBSTR_CODE, "5000");
		ptccb.put(CCBSTR_NAME, "Set_Senser_Temperature");
		string zwDate, zwTime;
		zwGetLocalDateTimeString(time(NULL), zwDate, zwTime);
		ptccb.put(CCBSTR_DATE, zwDate);
		ptccb.put(CCBSTR_TIME, zwTime);
		//ʵ���������ֶ�
		ptccb.put(CCBSTR_DEVCODE, ptjc.get < string > ("Atm_Serial"));
		ptccb.put("root.LockId", ptjc.get < string > ("Lock_Serial"));
		ptccb.put("root.Status", ptjc.get < int >("Status"));
	}

	void zwconvShockSenseDown(const ptree & ptccb, ptree & ptjc) {
		ZWFUNCTRACE
		    ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,
			     jcAtmcConvertDLL::JCSTR_SENSE_SHOCK);
		ptjc.put("Shock", ptccb.get < int >("root.Shock"));
	}

	void zwconvShockSenseUp(const ptree & ptjc, ptree & ptccb) {
		ZWFUNCTRACE ptccb.put(CCBSTR_CODE, "5001");
		ptccb.put(CCBSTR_NAME, "Set_Senser_Shock");
		string zwDate, zwTime;
		zwGetLocalDateTimeString(time(NULL), zwDate, zwTime);
		ptccb.put(CCBSTR_DATE, zwDate);
		ptccb.put(CCBSTR_TIME, zwTime);
		//ʵ���������ֶ�
		ptccb.put(CCBSTR_DEVCODE, ptjc.get < string > ("Atm_Serial"));
		ptccb.put("root.LockId", ptjc.get < string > ("Lock_Serial"));
		ptccb.put("root.Status", ptjc.get < int >("Status"));
	}
//////////////////////////////////////////////////////////////////////////
//20141125.1508.����������5002/3/4��������
	////ATM�������ϴ���Сѭ����������
	void zwconvTemptureSetInsideLoopTimesDown(const ptree & ptccb, ptree & ptjc) {
		ZWFUNCTRACE
			ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,
			jcAtmcConvertDLL::JCSTR_SENSE_SET_INSIDE_LOOP_TIMES);
		ptjc.put("Times", ptccb.get < int >("root.Times"));
	}

	void zwconvTemptureSetInsideLoopTimesUp( const ptree & ptjc, ptree & ptccb )
	{
		ZWFUNCTRACE 
		ptccb.put(CCBSTR_CODE, "5002");
		ptccb.put(CCBSTR_NAME, jcAtmcConvertDLL::JCSTR_SENSE_SET_INSIDE_LOOP_TIMES);
		string zwDate, zwTime;
		zwGetLocalDateTimeString(time(NULL), zwDate, zwTime);
		ptccb.put(CCBSTR_DATE, zwDate);
		ptccb.put(CCBSTR_TIME, zwTime);
		//ʵ���������ֶ�
		ptccb.put(CCBSTR_DEVCODE, ptjc.get < string > ("Atm_Serial"));
		ptccb.put("root.LockId", ptjc.get < string > ("Lock_Serial"));
		ptccb.put("root.Status", ptjc.get < int >("Status"));
	}
	//ATM�������ϴ���Сѭ������(��λ��)����
	void zwconvTemptureSetInsideLoopPeriodDown(const ptree & ptccb, ptree & ptjc) {
		ZWFUNCTRACE
			ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,
			jcAtmcConvertDLL::JCSTR_SENSE_SET_INSIDE_LOOP_PERIOD);
		ptjc.put("Period", ptccb.get < int >("root.Period"));
	}

	void zwconvTemptureSetInsideLoopPeriodUp( const ptree & ptjc, ptree & ptccb )
	{
		ZWFUNCTRACE ptccb.put(CCBSTR_CODE, "5003");
		ptccb.put(CCBSTR_NAME, jcAtmcConvertDLL::JCSTR_SENSE_SET_INSIDE_LOOP_PERIOD);
		string zwDate, zwTime;
		zwGetLocalDateTimeString(time(NULL), zwDate, zwTime);
		ptccb.put(CCBSTR_DATE, zwDate);
		ptccb.put(CCBSTR_TIME, zwTime);
		//ʵ���������ֶ�
		ptccb.put(CCBSTR_DEVCODE, ptjc.get < string > ("Atm_Serial"));
		ptccb.put("root.LockId", ptjc.get < string > ("Lock_Serial"));
		ptccb.put("root.Status", ptjc.get < int >("Status"));
	}
	//ATM�������ϴ��Ĵ�ѭ������(��λ����)����
	void zwconvTemptureSetOutsideLoopPeriodDown(const ptree & ptccb, ptree & ptjc) {
		ZWFUNCTRACE
			ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,
			jcAtmcConvertDLL::JCSTR_SENSE_SET_OUTSIDE_LOOP_PERIOD);
		ptjc.put("Period", ptccb.get < int >("root.Period"));
	}

	void zwconvTemptureSetOutsideLoopPeriodUp( const ptree & ptjc, ptree & ptccb )
	{
		ZWFUNCTRACE ptccb.put(CCBSTR_CODE, "5004");
		ptccb.put(CCBSTR_NAME, jcAtmcConvertDLL::JCSTR_SENSE_SET_OUTSIDE_LOOP_PERIOD);
		string zwDate, zwTime;
		zwGetLocalDateTimeString(time(NULL), zwDate, zwTime);
		ptccb.put(CCBSTR_DATE, zwDate);
		ptccb.put(CCBSTR_TIME, zwTime);
		//ʵ���������ֶ�
		ptccb.put(CCBSTR_DEVCODE, ptjc.get < string > ("Atm_Serial"));
		ptccb.put("root.LockId", ptjc.get < string > ("Lock_Serial"));
		ptccb.put("root.Status", ptjc.get < int >("Status"));
	}

//////////////////////////////////////////////////////////////////////////
	//��ȡXML��������
	string zwGetJcxmlMsgType(const char *jcXML) 
	{
		ptree ptccb;
		std::stringstream ss;
		ss << jcXML;
		read_xml(ss, ptccb);
		string msgType=ptccb.get<string>("root.TransCode");		
		return msgType;
	}


	//��ȡJSON��������
	string zwGetJcJsonMsgType(const char *jcJson) 
	{
		ptree ptjc;
		std::stringstream ss;
		ss << jcJson;
		read_json(ss, ptjc);
		string msgType=ptjc.get<string>("Command");		
		return msgType;
	}



}				//namespace jcAtmcConvertDLL{
