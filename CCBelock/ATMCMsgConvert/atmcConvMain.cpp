#include "stdafx.h"
#include "zwCcbElockHdr.h"
#include "myConvIntHdr.h"
//20150105.��������Ʋ����õĶ���
#define TMP_MAHAO_TEST_20150105

//��ATMC DLL��XML��JSON��ת���������ڴˣ����ڵ�Ԫ���ԣ�
namespace jcAtmcConvertDLL {
	//Ϊ��ƥ�������б��ı������������ı������ͱ�־λ
	string s_pipeJcCmdDown="";	
	string s_pipeJcCmdUp="";	

#ifdef TMP_MAHAO_TEST_20150105
	//20150105.��������Ʋ����õĶ���
	int G_MAHAO_LOG_COUNT=1;
#endif // TMP_MAHAO_TEST_20150105
	const JC_MSG_TYPE zwCCBxml2JCjson(const string & downXML,
					  string & downJson) {
		//ZWFUNCTRACE
		    //����ATMC�·���XML��ת��Ϊ�м���ʽptree
		    assert(downXML.length() > 42);	//XML��ͷ�Ĺ̶�����38���ַ����������һ����ǩ�����Լ����źϼ�4���ַ�
#ifdef _DEBUG401ZW
		ZWDBGMSG
		    ("******************����ATMC�·�XML��ʼ***\n");
		ZWDBGMSG(downXML.c_str());
		ZWDBGMSG
		    ("******************����ATMC�·�XML����****\n");
#endif // _DEBUG401ZW
		JC_MSG_TYPE msgType = JCMSG_INVALID_TYPE;
		ptree ptCCB;
		 std::stringstream ss;
		 ss << downXML;
		 read_xml(ss, ptCCB);
		//////////////////////////////////////////////////////////////////////////

		 std::stringstream ssccb;
		 write_json(ssccb, ptCCB);
		string ccbJson = ssccb.str();
		//LOG(INFO)<<"����XMLת�����JSON��ʼ\n"<<ccbJson;
		 //ZWDBGWARN("CCBXMLת�����JSON����\n");

		//�ж���Ϣ����
		string transCode = ptCCB.get < string > (CCBSTR_CODE);
		//������Ϣ���͵��ò�ͬ��������  
		//�ӽ��еĽӿ������ֶα�Ϊ���ǵ�JSON�ӿ�
		ptree ptJC;
		if ("0000" == transCode) {	//���߼�������
			ns_ActReqName = ptCCB.get < string > (CCBSTR_NAME);
			msgType = JCMSG_LOCK_ACTIVE_REQUEST;
			zwconvLockActiveDown(ptCCB, ptJC);
		}
		if ("0001" == transCode) {	//�������߼�����Ϣ(��ʼ��)
			ns_LockInitName = ptCCB.get < string > (CCBSTR_NAME);
			msgType = JCMSG_SEND_LOCK_ACTIVEINFO;
			zwconvLockInitDown(ptCCB, ptJC);
		}
		if ("0002" == transCode) {	//��ѯ����״̬
			zwconvQueryLockStatusDown(ptCCB, ptJC);
			msgType = JCMSG_QUERY_LOCK_STATUS;
		}
		if ("0003" == transCode) {	//ʱ��ͬ��
			zwconvTimeSyncDown(ptCCB, ptJC);
			msgType = JCMSG_TIME_SYNC;
		}
		if ("0004" == transCode) {	//��ȡ������
			ns_ReadCloseCodeName =
			    ptCCB.get < string > (CCBSTR_NAME);
			msgType = JCMSG_GET_CLOSECODE;
			zwconvReadCloseCodeDown(ptCCB, ptJC);
		}
		if ("0005" == transCode) {	//��ȡ��־
			msgType = JCMSG_GET_LOCK_LOG;
#ifdef TMP_MAHAO_TEST_20150105
			//ÿ���µĶ�ȡ��־������������¹�1
			G_MAHAO_LOG_COUNT=1;
#endif // TMP_MAHAO_TEST_20150105
			zwconvGetLockLogDown(ptCCB, ptJC);
		}
		if ("1001" == transCode) {	//�����������͸澯
			msgType = JCMSG_PUSH_WARNING;
			zwconvLockPushWarnDown(ptCCB, ptJC);
		}
		if ("1003" == transCode) {	//��������Ҫ��ʱ��ͬ��
			msgType = JCMSG_REQUEST_TIME_SYNC;
			zwconvLockReqTimeSyncDown(ptCCB, ptJC);
		}
		//////////////////////////////////////////////////////////////////////////
		//20141111.����Ҫ�����ӵ��¶Ⱥ��񶯴�����֧�ֱ���
		if ("5000" == transCode) {	//�¶ȴ���������
			msgType = JCMSG_SENSE_TEMPTURE;
			zwconvTemptureSenseDown(ptCCB, ptJC);
		}
		if ("5001" == transCode) {	//�񶯴���������
			msgType = JCMSG_SENSE_SHOCK;
			zwconvShockSenseDown(ptCCB, ptJC);
		}
		if ("5002" == transCode) {	//ATM�������ϴ���Сѭ����������
			msgType = JCMSG_SENSE_SET_INSIDE_LOOP_TIMES;
			zwconvTemptureSetInsideLoopTimesDown(ptCCB, ptJC);
		}
		if ("5003" == transCode) {	//ATM�������ϴ���Сѭ������(��λ��)����
			msgType = JCMSG_SENSE_SET_INSIDE_LOOP_PERIOD;
			zwconvTemptureSetInsideLoopPeriodDown(ptCCB, ptJC);
		}
		if ("5004" == transCode) {	//ATM�������ϴ��Ĵ�ѭ������(��λ����)����
			msgType = JCMSG_SENSE_SET_OUTSIDE_LOOP_PERIOD;
			zwconvTemptureSetOutsideLoopPeriodDown(ptCCB, ptJC);
		}

		//////////////////////////////////////////////////////////////////////////
		//���ߵ����ϴ���Ϣ�����һ��һ�������Ϣ��
		if ("1000" == transCode) {	//���ճ�ʼ������
			msgType = JCMSG_SEND_INITCLOSECODE;
			zwconvRecvInitCloseCodeDown(ptCCB, ptJC);
		}
		if ("1002" == transCode) {	//������֤��
			msgType = JCMSG_SEND_UNLOCK_CERTCODE;
			zwconvRecvVerifyCodeDown(ptCCB, ptJC);
		}
////////////////////////////////20150430//////////////////////////////////////////
		s_pipeJcCmdDown	=
			ptJC.get < string > (jcAtmcConvertDLL::JCSTR_CMDTITLE);



		//���������ΪJson����λ��ʹ��
		std::stringstream ss2;
		write_json(ss2, ptJC);
		downJson = ss2.str();
//#ifdef _DEBUG401
#ifdef _DEBUG
		//printf("��ǰ����ʱ��time(NULL)=%u\n",time(NULL));
#endif // _DEBUG
		LOG(WARNING)<<"��JSON�·�����ʼ\n"		
		<<downJson;
		//printf("%s\n", downJson.c_str());
		//ZWDBGWARN("��JSON�·��������\n");
//#endif // _DEBUG401

		return msgType;
	}
//////////////////////////////////////////////////////////////////////////

	const JC_MSG_TYPE zwJCjson2CCBxml(const string & upJson, string & upXML) {
		//ZWFUNCTRACE
		    //����λ�����ն�����json����ַ���������Ϊ�м���ʽptree
		    assert(upJson.length() > 9);	//json������ķ����������Ҫ9���ַ�����
		ptree ptJC;
		std::stringstream ss;
		ss << upJson;
		read_json(ss, ptJC);
		//////////////////////////////////////////////////////////////////////////
		std::stringstream sst1;
		write_json(sst1, ptJC);
		string jsonJc = sst1.str();
//#ifdef _DEBUG401
		LOG(WARNING)<<"�����߷��ص�JSONӦ��ʼ\n"
		<<jsonJc;
		//printf("%s\n", jsonJc.c_str());
		//ZWDBGWARN("JINCHU���߷��ص�JSONӦ�����\n");
//#endif // _DEBUG401

		//�ж���Ϣ���Ͳ������ǵ�JSON�ӿڱ�Ϊ���еĽӿ������ֶ�
		string jcCmd =
		    ptJC.get < string > (jcAtmcConvertDLL::JCSTR_CMDTITLE);
		s_pipeJcCmdUp=jcCmd;
		ptree ptCCB;
		if (JCSTR_LOCK_ACTIVE_REQUEST == jcCmd) {	//�������߼�������
			zwconvLockActiveUp(ptJC, ptCCB);
		}
		if (JCSTR_LOCK_INIT == jcCmd) {	//�������߼�����Ϣ(���߳�ʼ��)
			zwconvLockInitUp(ptJC, ptCCB);
		}
		if (JCSTR_QUERY_LOCK_STATUS == jcCmd) {	//��ѯ����״̬
			zwconvCheckLockStatusUp(ptJC, ptCCB);
		}
		if (JCSTR_TIME_SYNC == jcCmd) {	//ʱ��ͬ��
			zwconvTimeSyncUp(ptJC, ptCCB);
		}
		if (JCSTR_READ_CLOSECODE == jcCmd) {	//��ȡ������
			zwconvReadCloseCodeUp(ptJC, ptCCB);
		}
		if (JCSTR_GET_LOCK_LOG == jcCmd) {	//��ȡ������
			zwconvGetLockLogUp(ptJC, ptCCB);
		}

		if (JCSTR_SEND_INITCLOSECODE == jcCmd) {	//������λ�����������ĳ�ʼ������
			zwconvRecvInitCloseCodeUp(ptJC, ptCCB);
		}
		if (JCSTR_SEND_UNLOCK_CERTCODE == jcCmd) {	//������λ��������������֤��		
			zwconvRecvVerifyCodeUp(ptJC, ptCCB);
		}

		if (JCSTR_PUSH_WARNING == jcCmd) {	//���������������͵ĸ澯��Ϣ
			zwconvLockPushWarnUp(ptJC, ptCCB);
		}
		if (JCSTR_REQUEST_TIME_SYNC == jcCmd) {	//��������������ʱ��ͬ������
			zwconvLockReqTimeSyncUp(ptJC, ptCCB);
		}
		//////////////////////////////////////////////////////////////////////////
		//20141111����.�¶��񶯴���������֧��
		if (JCSTR_SENSE_TEMPTURE == jcCmd) {	//�¶ȴ�����
			zwconvTemptureSenseUp(ptJC, ptCCB);
		}
		if (JCSTR_SENSE_SHOCK == jcCmd) {	//�񶯴�����
			zwconvShockSenseUp(ptJC, ptCCB);
		}
		if (JCSTR_SENSE_SET_INSIDE_LOOP_TIMES == jcCmd) {	//ATM�������ϴ���Сѭ����������
			zwconvTemptureSetInsideLoopTimesUp(ptCCB, ptJC);
		}
		if (JCSTR_SENSE_SET_INSIDE_LOOP_PERIOD == jcCmd) {	//ATM�������ϴ���Сѭ������(��λ��)����
			zwconvTemptureSetInsideLoopPeriodUp(ptCCB, ptJC);
		}
		if (JCSTR_SENSE_SET_OUTSIDE_LOOP_PERIOD == jcCmd) {	//ATM�������ϴ��Ĵ�ѭ������(��λ����)����
			zwconvTemptureSetOutsideLoopPeriodUp(ptCCB, ptJC);
		}

		//////////////////////////////////////////////////////////////////////////
		std::stringstream sst2;		
		write_json(sst2, ptCCB);		
		string jsonCcb = sst2.str();
		//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG401
		//LOG(INFO)<<"������Ӧ��JSON����Ϊ��������Ԫ�ؿ�ʼ\n"<<jsonCcb.c_str();
		//ZWDBGMSG("������Ӧ��JSON����Ϊ��������Ԫ�ؽ���\n");	
#endif // _DEBUG401
	
		//ת��JSONΪXML��ATMCʹ��
		std::stringstream ss2;		
		write_xml(ss2, ptCCB);
		upXML = ss2.str();
#ifdef _DEBUG401ZW
		ZWDBGMSG("��Ӧ��XML��ʼ\n");
		ZWDBGMSG(upXML.c_str());
		//printf("%s\n",upXML.c_str());
		//ZWDBGMSG("��Ӧ��XML����\n");
#endif // _DEBUG401ZW
#ifdef TMP_MAHAO_TEST_20150105
		//20150105.1623.Ӧ��������Ƶ���ʱ����Ҫ����Ӱ�0005���ķ�������д���ļ���Ҫ����ʱ��ӣ��Ժ���Ҫ��
		if (JCSTR_GET_LOCK_LOG == jcCmd) {	//��ȡ������
			FILE *fp=fopen("mahaoLog20150105.txt","a");
			char buf[64];
			memset(buf,0,64);
			sprintf(buf,"\n��%d��:\n",G_MAHAO_LOG_COUNT++);
			fwrite(buf,strlen(buf),1,fp);
			fwrite(upXML.c_str(),upXML.length(),1,fp);
			fclose(fp);
		}
#endif // TMP_MAHAO_TEST_20150105

		try {			
			string transCode = ptCCB.get < string > (CCBSTR_CODE);			
			if ("0000" == transCode) {
				return JCMSG_LOCK_ACTIVE_REQUEST;
			}
		}
		catch(...) {			
			ZWFATAL("��JSON��ȫΪCCB�����ֶι�������ȱʧ")
			    return JCMSG_INVALID_TYPE;
		}		
		return JCMSG_INVALID_TYPE;
	}
}				//namespace jcAtmcConvertDLL{
