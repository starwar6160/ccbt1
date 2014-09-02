#include "stdafx.h"
#include "zwCcbElockHdr.h"
#include "zwPocoLog.h"
#include "myConvIntHdr.h"


//��ATMC DLL��XML��JSON��ת���������ڴˣ����ڵ�Ԫ���ԣ�
namespace jcAtmcConvertDLL{



	const JC_MSG_TYPE zwCCBxml2JCjson( const string &downXML,string &downJson )
	{
		ZWFUNCTRACE
			//����ATMC�·���XML��ת��Ϊ�м���ʽptree
			assert(downXML.length()>42);	//XML��ͷ�Ĺ̶�����38���ַ����������һ����ǩ�����Լ����źϼ�4���ַ�
		ZWTRACE("******************����ATMC�·�XML��ʼ*********************\n");
		ZWTRACE(downXML.c_str());
		ZWTRACE("******************����ATMC�·�XML����*********************\n");
		JC_MSG_TYPE msgType=JCMSG_INVALID_TYPE;
		ptree ptCCB;
		std::stringstream ss;
		ss<<downXML;
		read_xml(ss,ptCCB);
		//////////////////////////////////////////////////////////////////////////

		std::stringstream ssccb;
		write_json(ssccb,ptCCB);
		string ccbJson= ssccb.str();
		ZWTRACE("***����XMLת�����JSON��ʼ********************************\n");
		ZWTRACE(ccbJson.c_str());
		ZWTRACE("***����XMLת�����JSON����********************************\n");


		//�ж���Ϣ����
		string transCode=ptCCB.get<string>(CCBSTR_CODE);
		//���潨�������ֶ��Ա��ϴ�����ʱ�ṩ������			
		ns_ccbAtmno=ptCCB.get(CCBSTR_DEVCODE,"CCBATMFAKE88");
		//������Ϣ���͵��ò�ͬ��������	
		//�ӽ��еĽӿ������ֶα�Ϊ���ǵ�JSON�ӿ�
		ptree ptJC;
		if ("0000"==transCode)
		{//���߼�������
			ns_ActReqName=ptCCB.get<string>(CCBSTR_NAME);
			msgType= JCMSG_LOCK_ACTIVE_REQUEST;
			zwconvLockActiveDown(ptCCB,ptJC);
		}
		if ("0001"==transCode)
		{//�������߼�����Ϣ(��ʼ��)
			ns_LockInitName=ptCCB.get<string>(CCBSTR_NAME);
			msgType= JCMSG_SEND_LOCK_ACTIVEINFO;
			zwconvLockInitDown(ptCCB,ptJC);
		}
		if ("0002"==transCode)
		{//��ѯ����״̬
			zwconvQueryLockStatusDown(ptCCB,ptJC);
			msgType=JCMSG_QUERY_LOCK_STATUS;
		}
		if ("0003"==transCode)
		{//ʱ��ͬ��
			zwconvTimeSyncDown(ptCCB,ptJC);
			msgType=JCMSG_TIME_SYNC;
		}
		if ("0004"==transCode)
		{//��ȡ������
			ns_ReadCloseCodeName=ptCCB.get<string>(CCBSTR_NAME);
			msgType= JCMSG_GET_CLOSECODE;
			zwconvReadCloseCodeDown(ptCCB,ptJC);
		}
		if ("0005"==transCode)
		{//��ȡ��־
			msgType= JCMSG_GET_LOCK_LOG;
			zwconvGetLockLogDown(ptCCB,ptJC);
		}
		if ("1001"==transCode)
		{//�����������͸澯
			msgType= JCMSG_PUSH_WARNING;
			zwconvLockPushWarnDown(ptCCB,ptJC);
		}
		if ("1003"==transCode)
		{//��������Ҫ��ʱ��ͬ��
			msgType= JCMSG_REQUEST_TIME_SYNC;
			zwconvLockReqTimeSyncDown(ptCCB,ptJC);
		}
		//////////////////////////////////////////////////////////////////////////
		//���ߵ����ϴ���Ϣ�����һ��һ�������Ϣ��
		if ("1000"==transCode)
		{//���ճ�ʼ������
			msgType= JCMSG_SEND_INITCLOSECODE;
			zwconvRecvInitCloseCodeDown(ptCCB,ptJC);
		}
		if ("1002"==transCode)
		{//������֤��
			msgType= JCMSG_SEND_UNLOCK_CERTCODE;
			zwconvRecvVerifyCodeDown(ptCCB,ptJC);
		}
		//���������ΪJson����λ��ʹ��
		std::stringstream ss2;
		write_json(ss2,ptJC);
		downJson= ss2.str();
		ZWTRACE("***��JSON�·�����ʼ***********************************\n");
		ZWTRACE(downJson.c_str());
		ZWTRACE("***��JSON�·��������***********************************\n");

		return msgType;
	}
//////////////////////////////////////////////////////////////////////////

	const JC_MSG_TYPE zwJCjson2CCBxml( const string &upJson,string &upXML )
	{
		ZWFUNCTRACE
			//����λ�����ն�����json����ַ���������Ϊ�м���ʽptree
			assert(upJson.length()>9);	//json������ķ����������Ҫ9���ַ�����
		ptree ptJC;
		std::stringstream ss;
		ss<<upJson;
		read_json(ss,ptJC);
		//////////////////////////////////////////////////////////////////////////
		std::stringstream sst1;
		write_json(sst1,ptJC);
		string jsonJc= sst1.str();
		ZWTRACE("***�����߷��ص�JSONӦ��ʼ*************************####\n");
		ZWTRACE(jsonJc.c_str());
		ZWTRACE("***�����߷��ص�JSONӦ�����*************************####\n");


		//�ж���Ϣ���Ͳ������ǵ�JSON�ӿڱ�Ϊ���еĽӿ������ֶ�
		string jcCmd=ptJC.get<string>(jcAtmcConvertDLL::JCSTR_CMDTITLE);
		ptree ptCCB;
		if (JCSTR_LOCK_ACTIVE_REQUEST==jcCmd)
		{//�������߼�������
			zwconvLockActiveUp(ptJC,ptCCB);
		}
		if (JCSTR_LOCK_INIT==jcCmd)
		{//�������߼�����Ϣ(���߳�ʼ��)
			zwconvLockInitUp(ptJC,ptCCB);
		}
		if (JCSTR_QUERY_LOCK_STATUS==jcCmd)
		{//��ѯ����״̬
			zwconvCheckLockStatusUp(ptJC,ptCCB);
		}	
		if (JCSTR_TIME_SYNC==jcCmd)
		{//ʱ��ͬ��
			zwconvTimeSyncUp(ptJC,ptCCB);
		}
		if (JCSTR_READ_CLOSECODE==jcCmd)
		{//��ȡ������
			zwconvReadCloseCodeUp(ptJC,ptCCB);
		}
		if (JCSTR_GET_LOCK_LOG==jcCmd)
		{//��ȡ������
			zwconvGetLockLogUp(ptJC,ptCCB);
		}

		if (JCSTR_SEND_INITCLOSECODE==jcCmd)
		{//������λ�����������ĳ�ʼ������
			zwconvRecvInitCloseCodeUp(ptJC,ptCCB);
		}		
		if (JCSTR_SEND_UNLOCK_CERTCODE==jcCmd)
		{//������λ��������������֤��
			zwconvRecvVerifyCodeUp(ptJC,ptCCB);
		}

		if (JCSTR_PUSH_WARNING==jcCmd)
		{//���������������͵ĸ澯��Ϣ
			zwconvLockPushWarnUp(ptJC,ptCCB);
		}
		if (JCSTR_REQUEST_TIME_SYNC==jcCmd)
		{//��������������ʱ��ͬ������
			zwconvLockReqTimeSyncUp(ptJC,ptCCB);
		}

		//////////////////////////////////////////////////////////////////////////
		std::stringstream sst2;
		write_json(sst2,ptCCB);
		string jsonCcb=sst2.str();
		//////////////////////////////////////////////////////////////////////////
		ZWTRACE("***������Ӧ��JSON����Ϊ��������Ԫ�� ��ʼ************####\n");
		ZWTRACE(jsonCcb.c_str());
		ZWTRACE("***������Ӧ��JSON����Ϊ��������Ԫ�� ����************####\n");

		//ת��JSONΪXML��ATMCʹ��
		std::stringstream ss2;
		write_xml(ss2,ptCCB);
		upXML= ss2.str();
		ZWTRACE("*********************��Ӧ��XML��ʼ******************####\n");
		ZWTRACE(upXML.c_str());
		ZWTRACE("*********************��Ӧ��XML����******************####\n");
		try{
			string transCode=ptCCB.get<string>(CCBSTR_CODE);
			if ("0000"==transCode)
			{
				return JCMSG_LOCK_ACTIVE_REQUEST;
			}
		}
		catch(...)
		{
			ZWFATAL("��JSON��ȫΪCCB�����ֶι�������ȱʧ")
				return JCMSG_INVALID_TYPE;
		}
		return JCMSG_INVALID_TYPE;
	}



}	//namespace jcAtmcConvertDLL{

#ifdef _DEBUG_TEST819
void testjson819(void)
{
	const char *jstest1="{\"Command\": \"Lock_Time_Sync_ATM\",\"Lock_Time\": 1408443318 }";
	ptree ptt;
	std::stringstream ss;
	ss<<jstest1;
	read_json(ss,ptt);
	write_json(ss,ptt);
	cout<<__FUNCTION__<<" "<<ss.str()<<endl;
}
#endif // _DEBUG_TEST819
