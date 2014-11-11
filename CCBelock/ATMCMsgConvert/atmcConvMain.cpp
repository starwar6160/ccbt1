#include "stdafx.h"
#include "zwCcbElockHdr.h"
#include "zwPocoLog.h"
#include "myConvIntHdr.h"

//��ATMC DLL��XML��JSON��ת���������ڴˣ����ڵ�Ԫ���ԣ�
namespace jcAtmcConvertDLL {

	const JC_MSG_TYPE zwCCBxml2JCjson(const string & downXML,
					  string & downJson) {
		ZWFUNCTRACE
		    //����ATMC�·���XML��ת��Ϊ�м���ʽptree
		    assert(downXML.length() > 42);	//XML��ͷ�Ĺ̶�����38���ַ����������һ����ǩ�����Լ����źϼ�4���ַ�
		ZWDBGMSG
		    ("******************����ATMC�·�XML��ʼ*********************\n");
		ZWDBGMSG(downXML.c_str());
		ZWDBGMSG
		    ("******************����ATMC�·�XML����*********************\n");
		JC_MSG_TYPE msgType = JCMSG_INVALID_TYPE;
		ptree ptCCB;
		 std::stringstream ss;
		 ss << downXML;
		 read_xml(ss, ptCCB);
		//////////////////////////////////////////////////////////////////////////

		 std::stringstream ssccb;
		 write_json(ssccb, ptCCB);
		string ccbJson = ssccb.str();
		 ZWDBGMSG
		    ("***����XMLת�����JSON��ʼ********************************\n");
		 ZWDBGMSG(ccbJson.c_str());
		 ZWDBGMSG
		    ("***����XMLת�����JSON����********************************\n");

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
		//���������ΪJson����λ��ʹ��
		std::stringstream ss2;
		write_json(ss2, ptJC);
		downJson = ss2.str();
		ZWDBGMSG
		    ("***��JSON�·�����ʼ***********************************\n");
		ZWDBGMSG(downJson.c_str());
		printf("%s\n",downJson.c_str());
		ZWDBGMSG
		    ("***��JSON�·��������***********************************\n");

		return msgType;
	}
//////////////////////////////////////////////////////////////////////////

	const JC_MSG_TYPE zwJCjson2CCBxml(const string & upJson, string & upXML) {
		ZWFUNCTRACE
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
		ZWDBGMSG
		    ("***�����߷��ص�JSONӦ��ʼ*************************####\n");
		ZWDBGMSG(jsonJc.c_str());
		printf("%s\n",jsonJc.c_str());
		ZWDBGMSG
		    ("***�����߷��ص�JSONӦ�����*************************####\n");

		//�ж���Ϣ���Ͳ������ǵ�JSON�ӿڱ�Ϊ���еĽӿ������ֶ�
		string jcCmd =
		    ptJC.get < string > (jcAtmcConvertDLL::JCSTR_CMDTITLE);
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
			OutputDebugStringA("20141017.1116.MaHaoTest1");
			zwconvRecvVerifyCodeUp(ptJC, ptCCB);
			OutputDebugStringA("20141017.1116.MaHaoTest2");
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

		//////////////////////////////////////////////////////////////////////////
		std::stringstream sst2;
		OutputDebugStringA("20141017.1116.MaHaoTest3");
		write_json(sst2, ptCCB);
		OutputDebugStringA("20141017.1116.MaHaoTest4");
		string jsonCcb = sst2.str();
		//////////////////////////////////////////////////////////////////////////
		ZWDBGMSG
		    ("***������Ӧ��JSON����Ϊ��������Ԫ�� ��ʼ************####\n");
		ZWDBGMSG(jsonCcb.c_str());
		ZWDBGMSG
		    ("***������Ӧ��JSON����Ϊ��������Ԫ�� ����************####\n");
		OutputDebugStringA("20141017.1116.MaHaoTest5");
		//ת��JSONΪXML��ATMCʹ��
		std::stringstream ss2;
		OutputDebugStringA("20141017.1116.MaHaoTest6");
		write_xml(ss2, ptCCB);
		OutputDebugStringA("20141017.1116.MaHaoTest7");
		upXML = ss2.str();
		ZWDBGMSG
		    ("*********************��Ӧ��XML��ʼ******************####\n");
		ZWDBGMSG(upXML.c_str());
		printf("%s\n",upXML.c_str());
		ZWDBGMSG
		    ("*********************��Ӧ��XML����******************####\n");
		try {
			OutputDebugStringA("20141017.1116.MaHaoTest8");
			string transCode = ptCCB.get < string > (CCBSTR_CODE);
			OutputDebugStringA("20141017.1116.MaHaoTest9");
			if ("0000" == transCode) {
				return JCMSG_LOCK_ACTIVE_REQUEST;
			}
		}
		catch(...) {
			OutputDebugStringA("20141017.1116.MaHaoTest10");
			ZWFATAL("��JSON��ȫΪCCB�����ֶι�������ȱʧ")
			    return JCMSG_INVALID_TYPE;
		}
		OutputDebugStringA("20141017.1116.MaHaoTest11");
		return JCMSG_INVALID_TYPE;
	}
}				//namespace jcAtmcConvertDLL{
