#include "stdafx.h"
#include "zwCcbElockHdr.h"
#include "zwPocoLog.h"
#include "ATMCMsgConvert\\myConvIntHdr.h"

//��ATMC DLL��XML��JSON��ת���������ڴˣ����ڵ�Ԫ���ԣ�
namespace jcAtmcConvertDLL{
	const char *LOCKMAN_NAME="BeiJing.JinChu";
	string ns_ActReqName;
	string ns_LockInitName;
	string ns_ReadCloseCodeName;
	string ns_ccbAtmno;		//ATM���

	////////////////////////////ÿһ�����ĵľ��崦������ʼ//////////////////////////////////////////////



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
