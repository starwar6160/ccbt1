#include "stdafx.h"
#include "zwCcbElockHdr.h"
#include "myConvIntHdr.h"

namespace jcAtmcConvertDLL {
//�������߼�������
	void zwconvLockActiveDown(const ptree &ptccb, ptree & ptjc) {
		ZWFUNCTRACE
		    ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,
			     JCSTR_LOCK_ACTIVE_REQUEST);
			ptjc.put("Atm_Serial", ptccb.get < string > (CCBSTR_DEVCODE));
	} 
	
	void zwconvLockActiveUp(const ptree & ptjc, ptree & ptccb) {
		ZWFUNCTRACE
		    //���õ���ʽ������
		    ptccb.put(CCBSTR_CODE, "0000");
		assert("CallForActInfo" == ns_ActReqName);
		ptccb.put(CCBSTR_NAME, ns_ActReqName);
		ptccb.put(CCBSTR_DEVCODE, ptjc.get < string > ("Atm_Serial"));
		//////////////////////////////////////////////////////////////////////////
		string zwDate, zwTime;
		zwGetLocalDateTimeString(ptjc.get < time_t > ("Lock_Time"),
					 zwDate, zwTime);
		ptccb.put(CCBSTR_DATE, zwDate);
		ptccb.put(CCBSTR_TIME, zwTime);

		//////////////////////////////////////////////////////////////////////////
		//���ò���

		ptccb.put("root.LockId", ptjc.get < string > ("Lock_Serial"));
		ptccb.put("root.LockPubKey",
			  ptjc.get < string > ("Lock_Public_Key"));
		ptccb.put("root.LockMan", LOCKMAN_NAME);
	}
//////////////////////////////////////////////////////////////////////////

//�������߼�����Ϣ(���߳�ʼ��)
	void zwconvLockInitDown(const ptree & ptccb, ptree & ptjc) {
		ZWFUNCTRACE
		    //���߳�ʼ��
		    //>> ��λ���·�
		    //      "command": JCSTR_LOCK_INIT,
		    //              "Lock_Time"
		    //              "Atm_Serial"
		    //              "Lock_Init_Info":"ECIES���ܹ���PSK"
		    //>> ������ȷ���պ󣬷���
		    //      "command": JCSTR_LOCK_INIT,
		    //      "Lock_Time"
		    //      "Lock_Serial"
		    //      "State"
		    //      "Lock_Init_Info":"����ECIES���ܳ�����PSK����,����ATMVH�浵"
		    ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE, JCSTR_LOCK_INIT);
		//�����ֶ�(LMSʱ��)��JC���У�CCBû��,ʱ�����Ӧ���ý��б����е�ʱ����ת��
		//�����ַ�����ʽ�����ں�ʱ���ֶκϳ�ת��ΪUTC����.��ʼ
		string ccbDate=ptccb.get<string>(CCBSTR_DATE);
		string ccbTime=ptccb.get<string>(CCBSTR_TIME);
		time_t ccbUTCSec=0;
		zwCCBDateTime2UTC(ccbDate.c_str(),ccbTime.c_str(),&ccbUTCSec);
		assert(ccbUTCSec>1400*1000*1000);
		//�����ַ�����ʽ�����ں�ʱ���ֶκϳ�ת��ΪUTC����.����

		ptjc.put("Lock_Time", ccbUTCSec);
		ptjc.put("Atm_Serial", ptccb.get < string > (CCBSTR_DEVCODE));
		//�˴�1.1�汾�Ѿ�֧��ECIES���ܹ���PSK�����ˣ�ʹ�õ�һ������
		ptjc.put("Lock_Init_Info",
			 ptccb.get < string > ("root.ActInfo"));
	}

	void zwconvLockInitUp(const ptree & ptjc, ptree & ptccb) {
		ZWFUNCTRACE
		    //���õ���ʽ������
		    ptccb.put(CCBSTR_CODE, "0001");
		ptccb.put(CCBSTR_NAME, ns_LockInitName);	//ʹ�û������ڴ��е�ֵ
		assert("SendActInfo" == ns_LockInitName);
		string zwDate, zwTime;
		zwGetLocalDateTimeString(ptjc.get < time_t > ("Lock_Time"),
					 zwDate, zwTime);
		ptccb.put(CCBSTR_DATE, zwDate);
		ptccb.put(CCBSTR_TIME, zwTime);
		ptccb.put(CCBSTR_DEVCODE, ptjc.get < string > ("Atm_Serial"));	//ʹ�û������ڴ��е�ֵ
		ptccb.put("root.LockMan", LOCKMAN_NAME);
		ptccb.put("root.LockId", ptjc.get < string > ("Lock_Serial"));
		//���ò���
		int ActiveResult = 0;	//���ж�����ֶ�0Ϊ�ɹ���1Ϊʧ�ܣ�
		ActiveResult = ptjc.get < int >("State");
		ptccb.put("root.ActiveResult", ActiveResult);
		//1.1�汾������λ��������ECIES���ܵ�PSK����Lock_Init_Info�ֶη���
		ptccb.put("root.ActInfo",
			  ptjc.get < string > ("Lock_Init_Info"));
	}

}				//namespace jcAtmcConvertDLL{
