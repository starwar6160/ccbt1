#include "stdafx.h"
#include "zwCcbElockHdr.h"
#include "zwPocoLog.h"

using namespace boost::property_tree;

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


//��ATMC DLL��XML��JSON��ת���������ڴˣ����ڵ�Ԫ���ԣ�
namespace jcAtmcConvertDLL{
	const char *LOCKMAN_NAME="BeiJing.JinChu";
	//�·�������
	void zwconvLockActiveDown(const ptree &ptccb, ptree &ptjc );
	void zwconvLockInitDown( const ptree &ptccb, ptree &ptjc );
	void zwconvReadCloseCodeDown( const ptree &ptccb, ptree &ptjc );
	void zwconvQueryLockStatusDown( const ptree &ptccb, ptree &ptjc );
	void zwconvTimeSyncDown( const ptree &ptccb, ptree &ptjc );
	void zwconvGetLockLogDown( const ptree &ptccb, ptree &ptjc );
	void zwconvLockPushWarnDown(const ptree &ptccb, ptree &ptjc );
	void zwconvLockReqTimeSyncDown(const ptree &ptccb, ptree &ptjc );
	
	//�ϴ�������
	void zwconvLockActiveUp(const ptree &ptjc, ptree &ptccb );
	void zwconvLockInitUp( const ptree &ptjc, ptree &ptccb );
	void zwconvReadCloseCodeUp( const ptree &ptjc, ptree &ptccb );
	void zwconvTimeSyncUp( const ptree &ptjc, ptree &ptccb );
	void zwconvCheckLockStatusUp( const ptree &ptjc, ptree &ptccb );
	void zwconvGetLockLogUp( const ptree &ptjc, ptree &ptccb );
	//���������������͵ĳ�ʼ�����룬ֻ���ϴ�����
	void zwconvRecvInitCloseCodeDown(const ptree &ptccb, ptree &ptjc );
	void zwconvRecvInitCloseCodeUp(const ptree &ptjc, ptree &ptccb);
	//���������������͵���֤�룬ֻ���ϴ�����
	void zwconvRecvVerifyCodeDown(const ptree &ptccb, ptree &ptjc );
	void zwconvRecvVerifyCodeUp(const ptree &ptjc, ptree &ptccb);
	void zwconvLockPushWarnUp(const ptree &ptjc, ptree &ptccb);
	void zwconvLockReqTimeSyncUp(const ptree &ptjc, ptree &ptccb);
	//����4���ֶΣ�Ϊ����������ת���ڼ䱣�潨�б���������ģ����ǻ������õ��ֱ��뷵�ظ����е��ֶ�
	string ns_ActReqName;
	string ns_LockInitName;
	string ns_ReadCloseCodeName;
	string ns_ccbAtmno;		//ATM���

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

	////////////////////////////ÿһ�����ĵľ��崦������ʼ//////////////////////////////////////////////

	//�������߼�������
	void zwconvLockActiveDown( const ptree &, ptree &ptjc )
	{	
		ZWFUNCTRACE
		ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,JCSTR_LOCK_ACTIVE_REQUEST);
	}


	void zwconvLockActiveUp( const ptree &ptjc, ptree &ptccb )
	{
		ZWFUNCTRACE
		//���õ���ʽ������
		ptccb.put(CCBSTR_CODE,"0000");
		assert("CallForActInfo"==ns_ActReqName);
		ptccb.put(CCBSTR_NAME,ns_ActReqName);
		ptccb.put(CCBSTR_DEVCODE,ns_ccbAtmno);
//////////////////////////////////////////////////////////////////////////
		string zwDate,zwTime;
		zwGetLocalDateTimeString(ptjc.get<time_t>("Lock_Time"),zwDate,zwTime);
		ptccb.put(CCBSTR_DATE,zwDate);
		ptccb.put(CCBSTR_TIME,zwTime);

//////////////////////////////////////////////////////////////////////////
		//���ò���
		
		ptccb.put("root.LockId",ptjc.get<string>("Lock_Serial"));
		ptccb.put("root.LockPubKey",ptjc.get<string>("Lock_Public_Key"));	
		ptccb.put("root.LockMan",LOCKMAN_NAME);
	}
	//////////////////////////////////////////////////////////////////////////

	//�������߼�����Ϣ(���߳�ʼ��)
	void zwconvLockInitDown( const ptree &ptccb, ptree &ptjc )
	{	
		ZWFUNCTRACE
		//���߳�ʼ��

		//>> ��λ���·�
		//	"command": JCSTR_LOCK_INIT,
		//		"Lock_Time"
		//		"Atm_Serial"
		//		"Lock_Init_Info":"ECIES���ܹ���PSK"

		//>> ������ȷ���պ󣬷���
		//	"command": JCSTR_LOCK_INIT,
		//	"Lock_Time"
		//	"Lock_Serial"
		//	"State"
		//	"Lock_Init_Info":"����ECIES���ܳ�����PSK����,����ATMVH�浵"

		ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,JCSTR_LOCK_INIT);
		//�����ֶ�(LMSʱ��)��JC���У�CCBû��,ʱ�����Ӧ���ý��б����е�ʱ����ת��
		ptjc.put("Lock_Time",time(NULL));
		ptjc.put("Atm_Serial",ptccb.get<string>(CCBSTR_DEVCODE));	
		//�˴�1.1�汾�Ѿ�֧��ECIES���ܹ���PSK�����ˣ�ʹ�õ�һ������
		ptjc.put("Lock_Init_Info",ptccb.get<string>("root.ActInfo"));		
	}

	void zwconvLockInitUp( const ptree &ptjc, ptree &ptccb )
	{
		ZWFUNCTRACE
		//���õ���ʽ������
		ptccb.put(CCBSTR_CODE,"0001");
		ptccb.put(CCBSTR_NAME,ns_LockInitName);	//ʹ�û������ڴ��е�ֵ
		assert("SendActInfo"==ns_LockInitName);
		string zwDate,zwTime;
		zwGetLocalDateTimeString(ptjc.get<time_t>("Lock_Time"),zwDate,zwTime);
		ptccb.put(CCBSTR_DATE,zwDate);
		ptccb.put(CCBSTR_TIME,zwTime);
		ptccb.put(CCBSTR_DEVCODE,ns_ccbAtmno);	//ʹ�û������ڴ��е�ֵ
		ptccb.put("root.LockMan",LOCKMAN_NAME);
		ptccb.put("root.LockId",ptjc.get<string>("Lock_Serial"));	
		//���ò���
		int ActiveResult=0;	//���ж�����ֶ�0Ϊ�ɹ���1Ϊʧ�ܣ�
		ActiveResult=ptjc.get<int>("State");
		ptccb.put("root.ActiveResult",ActiveResult);
		//1.1�汾������λ��������ECIES���ܵ�PSK����Lock_Init_Info�ֶη���
		ptccb.put("root.ActInfo",ptjc.get<string>("Lock_Init_Info"));
	}


	
	
	//��ѯ����״̬
	void zwconvQueryLockStatusDown( const ptree &, ptree &ptjc )
	{
		ZWFUNCTRACE
		ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,JCSTR_QUERY_LOCK_STATUS);
	}



	void zwconvCheckLockStatusUp( const ptree &ptjc, ptree &ptccb )
	{
		ZWFUNCTRACE
			//"Command": "Lock_Now_Info",
			//"Lock_Time": "1408434961",
			//"Lock_Serial": "A3KE2OK256EO2SPE",
			//"Lock_Status": "0,0,0,1,100,0,1,20,100,0,0"
			//���õ���ʽ������
		ptccb.put(CCBSTR_CODE,"0002");
		ptccb.put(CCBSTR_NAME,"QueryForLockStatus");
		string zwDate,zwTime;
		zwGetLocalDateTimeString(ptjc.get<time_t>("Lock_Time"),zwDate,zwTime);
		ptccb.put(CCBSTR_DATE,zwDate);
		ptccb.put(CCBSTR_TIME,zwTime);
		//////////////////////////////////////////////////////////////////////////
		ptccb.put(CCBSTR_DEVCODE,ns_ccbAtmno);	//ʹ�û������ڴ��е�ֵ
		ptccb.put("root.LockMan",LOCKMAN_NAME);
		ptccb.put("root.LockId",ptjc.get<string>("Lock_Serial"));	
		string LockStatusStr=ptjc.get<string>("Lock_Status");
		//��ʽ��ActiveStatus,EnableStatus,LockStatus,DoorStatus,
		//BatteryStatus,ShockAlert,TempAlert,PswTryAlert,LockOverTime
		//0,0,0,1,100,0,1,20,100,0,0
		JCLOCKSTATUS lockStatusStrings;
		zwLockStatusDataSplit(LockStatusStr.c_str(),lockStatusStrings);
		
	ptccb.put("root.ActiveStatus",lockStatusStrings.ActiveStatus);
	ptccb.put("root.EnableStatus",lockStatusStrings.EnableStatus);
	ptccb.put("root.LockStatus",lockStatusStrings.LockStatus);
	ptccb.put("root.DoorStatus",lockStatusStrings.DoorStatus);
	ptccb.put("root.BatteryStatus",lockStatusStrings.BatteryStatus);
	ptccb.put("root.ShockAlert",lockStatusStrings.ShockAlert);
	ptccb.put("root.ShockValue",lockStatusStrings.ShockValue);
	ptccb.put("root.TempAlert",lockStatusStrings.TempAlert);
	ptccb.put("root.NodeTemp",lockStatusStrings.nodeTemp);
	ptccb.put("root.PswTryAlert",lockStatusStrings.PswTryAlert);
	ptccb.put("root.LockOverTime",lockStatusStrings.LockOverTime);
	}

	//////////////////////////////////////////////////////////////////////////
	//ʱ��ͬ��
	void zwconvTimeSyncDown( const ptree &, ptree &ptjc )
	{
		ZWFUNCTRACE
		ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,JCSTR_TIME_SYNC);
		ptjc.put<time_t>("Lock_Time",time(NULL));
	}

		//ʱ��ͬ��
	void zwconvTimeSyncUp( const ptree &ptjc, ptree &ptccb )
	{
		ZWFUNCTRACE
		//���õ���ʽ������
		ptccb.put(CCBSTR_CODE,"0003");
		ptccb.put(CCBSTR_NAME,"TimeSync");	//ʹ�û������ڴ��е�ֵ
		string zwDate,zwTime;
		zwGetLocalDateTimeString(ptjc.get<time_t>("Lock_Time"),zwDate,zwTime);
		ptccb.put(CCBSTR_DATE,zwDate);
		ptccb.put(CCBSTR_TIME,zwTime);
		//////////////////////////////////////////////////////////////////////////
		ptccb.put(CCBSTR_DEVCODE,ns_ccbAtmno);	//ʹ�û������ڴ��е�ֵ
		ptccb.put("root.LockMan",LOCKMAN_NAME);
		ptccb.put("root.LockId",ptjc.get<string>("Lock_Serial"));	
		//////////////////////////////////////////////////////////////////////////
		//���ò���
		time_t exTimeValue=ptjc.get<time_t>("Ex_Syn_Time");
		string exDate,exTime;
		zwGetLocalDateTimeString(exTimeValue,exDate,exTime);
		ptccb.put("root.ExSynDate",exDate);
		ptccb.put("root.ExSynTime",exTime);
	}

	//////////////////////////////////////////////////////////////////////////

	//��ȡ������
	void zwconvReadCloseCodeDown( const ptree &, ptree &ptjc )
	{
		ZWFUNCTRACE
		//>> ��ȡ������
		//����
		//	"command": JCSTR_READ_CLOSECODE,
		//	"Lock_Time"	
		//Ӧ��
		//"Command":JCSTR_READ_CLOSECODE


		ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,JCSTR_READ_CLOSECODE);
		ptjc.put("Lock_Time",time(NULL));
	}

	void zwconvReadCloseCodeUp( const ptree &ptjc, ptree &ptccb )
	{
		ZWFUNCTRACE
		//��ȡ������ Ӧ��
		//	���״���	TransCode	��	ֵ��0004
		//	��������	TransName	��	ֵ��ReadShutLockCode
		//	��������	TransDate	��	ֵ��YYYYMMDD����20140401
		//	����ʱ��	TransTime	��	ֵ��hhmmss����134050
		//	ATM�豸���	DevCode	��	ֵ������12λ�豸���
		//	���߳���	LockMan	��	ֵ�������Զ����������̲�ͬ������
		//	���߱��	LockId	��	ֵ�������Զ�������Ψһ���
		//	������	ShutLockcode	��	ֵ��������
		//���õ���ʽ������
		ptccb.put(CCBSTR_CODE,"0004");
		assert("ReadShutLockCode"==ns_ReadCloseCodeName);
		ptccb.put(CCBSTR_NAME,ns_ReadCloseCodeName);
		string zwDate,zwTime;
		zwGetLocalDateTimeString(time(NULL),zwDate,zwTime);
		ptccb.put(CCBSTR_DATE,zwDate);
		ptccb.put(CCBSTR_TIME,zwTime);

		ptccb.put(CCBSTR_DEVCODE,ns_ccbAtmno);
		ptccb.put("root.LockMan",LOCKMAN_NAME);
		ptccb.put("root.LockId",ptjc.get<string>("Lock_Serial"));
		ptccb.put("root.ShutLockcode",ptjc.get<int>("Code"));
	}



	//��ȡ��־
	void zwconvGetLockLogDown( const ptree &ptccb, ptree &ptjc )
	{
		ZWFUNCTRACE
		ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,JCSTR_GET_LOCK_LOG);
		ptjc.put("Begin_No",ptccb.get<int>("root.BeginNo"));
		ptjc.put("End_No",ptccb.get<int>("root.EndNo"));
	}

	void zwconvGetLockLogUp( const ptree &ptjc, ptree &ptccb )
	{
		ZWFUNCTRACE
		ptccb.put(CCBSTR_CODE,"0005");
		ptccb.put(CCBSTR_NAME,"ReadLog");
		string zwDate,zwTime;
		zwGetLocalDateTimeString(ptjc.get<time_t>("Lock_Time"),zwDate,zwTime);
		ptccb.put(CCBSTR_DATE,zwDate);
		ptccb.put(CCBSTR_TIME,zwTime);
		ptccb.put("root.RevResult",ptjc.get<int>("State"));
		string LockStatus=ptjc.get<string>("Journal");
		JCLOCKSTATUS lst,ostr;
		zwLockStatusDataSplit(LockStatus.c_str(),lst);
		zwStatusData2String(lst,ostr);
		string LockStatusHumanReadable=LockStatusStringMerge(ostr);
		ptccb.put("root.Log",LockStatusHumanReadable);
	}


	void zwconvRecvInitCloseCodeDown(const ptree &, ptree &ptjc )
	{
		ZWFUNCTRACE
		ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,"Lock_Close_Code_Lock");
	}


	void zwconvRecvInitCloseCodeUp(const ptree &ptjc, ptree &ptccb)
	{
		ZWFUNCTRACE
		ptccb.put(CCBSTR_CODE,"1000");
		ptccb.put(CCBSTR_NAME,"SendShutLockCode");
		string zwDate,zwTime;
		zwGetLocalDateTimeString(time(NULL),zwDate,zwTime);
		ptccb.put(CCBSTR_DATE,zwDate);
		ptccb.put(CCBSTR_TIME,zwTime);
		//���߷��ͳ�ʼ������ʱ��ATM���Ӧ���Ѿ��ڼ��������л�ã�����
		//1.1�汾��������û�и��������Դ˴����ܻ�������
		ptccb.put(CCBSTR_DEVCODE,ns_ccbAtmno);	
		ptccb.put("root.LockMan",LOCKMAN_NAME);
		ptccb.put("root.ShutLockcode",ptjc.get<int>("Code"));
	}

	void zwconvRecvVerifyCodeDown(const ptree &, ptree &ptjc )
	{
		ZWFUNCTRACE
		ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,"Lock_Open_Ident");
	}

	void zwconvRecvVerifyCodeUp(const ptree &ptjc, ptree &ptccb)
	{
		ZWFUNCTRACE
		ptccb.put(CCBSTR_CODE,"1002");
		ptccb.put(CCBSTR_NAME,"SendUnLockIdent");
		string zwDate,zwTime;
		zwGetLocalDateTimeString(time(NULL),zwDate,zwTime);
		ptccb.put(CCBSTR_DATE,zwDate);
		ptccb.put(CCBSTR_TIME,zwTime);
		//���߷��ͳ�ʼ������ʱ��ATM���Ӧ���Ѿ��ڼ��������л�ã�����
		//1.1�汾��������û�и��������Դ˴����ܻ�������
		ptccb.put(CCBSTR_DEVCODE,ns_ccbAtmno);	
		ptccb.put("root.LockMan",LOCKMAN_NAME);
		ptccb.put("root.LockId",ptjc.get<string>("Lock_Serial"));
		//�ؼ�����֤�뱾��
		ptccb.put("root.UnLockIdentInfo",ptjc.get<int>("Lock_Ident_Info"));
	}

	void zwconvLockPushWarnDown(const ptree &ptccb, ptree &ptjc )
	{
		ZWFUNCTRACE
		ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,jcAtmcConvertDLL::JCSTR_PUSH_WARNING);
		ptjc.put("State",ptccb.get<int>("RevResult"));
	}

	void zwconvLockReqTimeSyncDown(const ptree &, ptree &ptjc )
	{
		ZWFUNCTRACE
		ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,jcAtmcConvertDLL::JCSTR_REQUEST_TIME_SYNC);
		ptjc.put("Lock_Time",time(NULL));
	}

	void zwconvLockPushWarnUp(const ptree &ptjc, ptree &ptccb)
	{
		ZWFUNCTRACE
		ptccb.put(CCBSTR_CODE,"1001");
		ptccb.put(CCBSTR_NAME,"SendAlertStatus");
		string zwDate,zwTime;
		zwGetLocalDateTimeString(time(NULL),zwDate,zwTime);
		ptccb.put(CCBSTR_DATE,zwDate);
		ptccb.put(CCBSTR_TIME,zwTime);
		//���߷��ͳ�ʼ������ʱ��ATM���Ӧ���Ѿ��ڼ��������л�ã�����
		//1.1�汾��������û�и��������Դ˴����ܻ�������
		ptccb.put(CCBSTR_DEVCODE,ns_ccbAtmno);	
		ptccb.put("root.LockMan",LOCKMAN_NAME);
		ptccb.put("root.LockId",ptjc.get<string>("Lock_Serial"));
		string LockStatusStr=ptjc.get<string>("Lock_Status");
		//��ʽ��ActiveStatus,EnableStatus,LockStatus,DoorStatus,
		//BatteryStatus,ShockAlert,TempAlert,PswTryAlert,LockOverTime
		//0,0,0,1,100,0,1,20,100,0,0
		JCLOCKSTATUS lockStatusStrings;
		zwLockStatusDataSplit(LockStatusStr.c_str(),lockStatusStrings);

		ptccb.put("root.ActiveStatus",lockStatusStrings.ActiveStatus);
		ptccb.put("root.EnableStatus",lockStatusStrings.EnableStatus);
		ptccb.put("root.LockStatus",lockStatusStrings.LockStatus);
		ptccb.put("root.DoorStatus",lockStatusStrings.DoorStatus);
		ptccb.put("root.BatteryStatus",lockStatusStrings.BatteryStatus);
		ptccb.put("root.ShockAlert",lockStatusStrings.ShockAlert);
		ptccb.put("root.ShockValue",lockStatusStrings.ShockValue);
		ptccb.put("root.TempAlert",lockStatusStrings.TempAlert);
		ptccb.put("root.NodeTemp",lockStatusStrings.nodeTemp);
		ptccb.put("root.PswTryAlert",lockStatusStrings.PswTryAlert);
		ptccb.put("root.LockOverTime",lockStatusStrings.LockOverTime);
	}

	void zwconvLockReqTimeSyncUp(const ptree &ptjc, ptree &ptccb)
	{
		ZWFUNCTRACE
		ptccb.put(CCBSTR_CODE,"1003");
		ptccb.put(CCBSTR_NAME,"TimeSync");
		string zwDate,zwTime;
		zwGetLocalDateTimeString(time(NULL),zwDate,zwTime);
		ptccb.put(CCBSTR_DATE,zwDate);
		ptccb.put(CCBSTR_TIME,zwTime);
		//���߷��ͳ�ʼ������ʱ��ATM���Ӧ���Ѿ��ڼ��������л�ã�����
		//1.1�汾��������û�и��������Դ˴����ܻ�������
		ptccb.put(CCBSTR_DEVCODE,ns_ccbAtmno);	
		ptccb.put("root.LockMan",LOCKMAN_NAME);
		ptccb.put("root.LockId",ptjc.get<string>("Lock_Serial"));
	}

	////////////////////////////ÿһ�����ĵľ��崦��������//////////////////////////////////////////////
}	//namespace jcAtmcConvertDLLLock_Open_Ident
