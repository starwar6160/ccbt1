#include "stdafx.h"
#include "zwCcbElockHdr.h"



using namespace boost::property_tree;
//��ATMC DLL��XML��JSON��ת���������ڴˣ����ڵ�Ԫ���ԣ�
namespace jcAtmcConvertDLL{
	const char *LOCKMAN_NAME="BeiJing.JinChu";
	//�·�������
	void zwconvLockActiveDown(const ptree &ptccb, ptree &ptjc );
	void zwconvLockInitDown( const ptree &ptccb, ptree &ptjc );
	void zwconvReadCloseCodeDown( const ptree &ptccb, ptree &ptjc );
	//�ϴ�������
	void zwconvLockActiveUp(const ptree &ptjc, ptree &ptccb );
	void zwconvLockInitUp( const ptree &ptjc, ptree &ptccb );
	void zwconvReadCloseCodeUp( const ptree &ptjc, ptree &ptccb );
	//���������������͵ĳ�ʼ�����룬ֻ���ϴ�����
	void zwconvRecvInitCloseCodeDown(const ptree &ptccb, ptree &ptjc );
	void zwconvRecvInitCloseCodeUp(const ptree &ptjc, ptree &ptccb);
	//���������������͵���֤�룬ֻ���ϴ�����
	void zwconvRecvVerifyCodeDown(const ptree &ptccb, ptree &ptjc );
	void zwconvRecvVerifyCodeUp(const ptree &ptjc, ptree &ptccb);
	//����4���ֶΣ�Ϊ����������ת���ڼ䱣�潨�б���������ģ����ǻ������õ��ֱ��뷵�ظ����е��ֶ�
	string ns_ActReqName;
	string ns_LockInitName;
	string ns_ReadCloseCodeName;
	string ns_ccbAtmno;		//ATM���


	const JC_MSG_TYPE zwCCBxml2JCjson( const string &downXML,string &downJson )
	{
		//����ATMC�·���XML��ת��Ϊ�м���ʽptree
		assert(downXML.length()>42);	//XML��ͷ�Ĺ̶�����38���ַ����������һ����ǩ�����Լ����źϼ�4���ַ�
		cout<<"******************����ATMC�·�XML��ʼ*********************\n";
		cout<<downXML<<endl;
		cout<<"******************����ATMC�·�XML����*********************\n";

		JC_MSG_TYPE msgType=JCMSG_INVALID_TYPE;
		ptree ptCCB;
		std::stringstream ss;
		ss<<downXML;
		read_xml(ss,ptCCB);
		//////////////////////////////////////////////////////////////////////////

		std::stringstream ssccb;
		write_json(ssccb,ptCCB);
		string ccbJson= ssccb.str();
		cout<<"***����XMLת�����JSON��ʼ********************************\n"<<ccbJson;
		cout<<"***����XMLת�����JSON����********************************\n";


		//�ж���Ϣ����
		string transCode=ptCCB.get<string>("root.TransCode");
		//���潨�������ֶ��Ա��ϴ�����ʱ�ṩ������	
			 //ns_ccbAtmno
			//optionl<ptree> nsk_atmno	 =
			//	ptCCB.get_optional("root.DevCode");		
		//boost::optional<string> v = ptCCB.get_optional<string>("root.DevCode");
		
		ns_ccbAtmno=ptCCB.get("root.DevCode","CCBATMFAKE88");
		//������Ϣ���͵��ò�ͬ��������	
		//�ӽ��еĽӿ������ֶα�Ϊ���ǵ�JSON�ӿ�
		ptree ptJC;
		if ("0000"==transCode)
		{//���߼�������
		ns_ActReqName=ptCCB.get<string>("root.TransName");
			msgType= JCMSG_LOCK_ACTIVE_REQUEST;
			zwconvLockActiveDown(ptCCB,ptJC);
		}
		if ("0001"==transCode)
		{//�������߼�����Ϣ(��ʼ��)
			ns_LockInitName=ptCCB.get<string>("root.TransName");
			msgType= JCMSG_SEND_LOCK_ACTIVEINFO;
			zwconvLockInitDown(ptCCB,ptJC);
		}
		if ("0004"==transCode)
		{//��ȡ������
			ns_ReadCloseCodeName=ptCCB.get<string>("root.TransName");
			msgType= JCMSG_GET_CLOSECODE;
			zwconvReadCloseCodeDown(ptCCB,ptJC);
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
		cout<<"***��JSON�·�����ʼ***********************************\n"<<downJson;
		cout<<"***��JSON�·��������***********************************\n";

		return msgType;
	}

	const JC_MSG_TYPE zwJCjson2CCBxml( const string &upJson,string &upXML )
	{
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
		cout<<"***�����߷��ص�JSONӦ��ʼ*************************####\n"<<jsonJc;
		cout<<"***�����߷��ص�JSONӦ�����*************************####\n";


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
		if (JCSTR_READ_CLOSECODE==jcCmd)
		{//��ȡ������
			zwconvReadCloseCodeUp(ptJC,ptCCB);
		}
		
		if (JCSTR_SEND_INITCLOSECODE==jcCmd)
		{//������λ�����������ĳ�ʼ������
			zwconvRecvInitCloseCodeUp(ptJC,ptCCB);
		}		
		if (JCSTR_SEND_UNLOCK_CERTCODE==jcCmd)
		{//������λ��������������֤��
			zwconvRecvVerifyCodeUp(ptJC,ptCCB);
		}

		//////////////////////////////////////////////////////////////////////////

		std::stringstream sst2;
		write_json(sst2,ptCCB);
		string jsonCcb=sst2.str();
		//////////////////////////////////////////////////////////////////////////
		cout<<"***������Ӧ��JSON����Ϊ��������Ԫ�� ��ʼ************####\n"<<jsonCcb;
		cout<<"***������Ӧ��JSON����Ϊ��������Ԫ�� ����************####\n";

		//ת��JSONΪXML��ATMCʹ��
		std::stringstream ss2;
		write_xml(ss2,ptCCB);
		upXML= ss2.str();

		cout<<"*********************��Ӧ��XML��ʼ******************####\n";
		cout<<upXML<<endl;
		cout<<"*********************��Ӧ��XML����******************####\n";
try{
		string transCode=ptCCB.get<string>("root.TransCode");
		if ("0000"==transCode)
		{
			return JCMSG_LOCK_ACTIVE_REQUEST;
		}
}
catch(...)
{
	return JCMSG_INVALID_TYPE;
}
		return JCMSG_INVALID_TYPE;
	}

	////////////////////////////ÿһ�����ĵľ��崦������ʼ//////////////////////////////////////////////

	//�������߼�������
	void zwconvLockActiveDown( const ptree &ptccb, ptree &ptjc )
	{	
		ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,JCSTR_LOCK_ACTIVE_REQUEST);
//		ptjc.put("State","get");
//		ptjc.put("Public_Key","123456");	//������ʵ�����壬����json�ӿ������У���д��
	}


	void zwconvLockActiveUp( const ptree &ptjc, ptree &ptccb )
	{
		//���õ���ʽ������
		ptccb.put("root.TransCode","0000");
		assert("CallForActInfo"==ns_ActReqName);
		ptccb.put("root.TransName",ns_ActReqName);
		ptccb.put("root.DevCode",ns_ccbAtmno);
//////////////////////////////////////////////////////////////////////////
		string zwDate,zwTime;
		zwGetDateTimeString(ptjc.get<time_t>("Lock_Time"),zwDate,zwTime);
		ptccb.put("root.TransDate",zwDate);
		ptccb.put("root.TransTime",zwTime);

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
		ptjc.put("Atm_Serial",ptccb.get<string>("root.DevCode"));	
		//�˴�1.1�汾�Ѿ�֧��ECIES���ܹ���PSK�����ˣ�ʹ�õ�һ������
		ptjc.put("Lock_Init_Info",ptccb.get<string>("root.ActInfo"));		
	}

	void zwconvLockInitUp( const ptree &ptjc, ptree &ptccb )
	{
		//���õ���ʽ������
		ptccb.put("root.TransCode","0001");
		ptccb.put("root.TransName",ns_LockInitName);	//ʹ�û������ڴ��е�ֵ
		assert("SendActInfo"==ns_LockInitName);
		string zwDate,zwTime;
		zwGetDateTimeString(ptjc.get<time_t>("Lock_Time"),zwDate,zwTime);
		ptccb.put("root.TransDate",zwDate);
		ptccb.put("root.TransTime",zwTime);
		ptccb.put("root.DevCode",ns_ccbAtmno);	//ʹ�û������ڴ��е�ֵ
		ptccb.put("root.LockMan",LOCKMAN_NAME);
		ptccb.put("root.LockId",ptjc.get<string>("Lock_Serial"));	
		//���ò���
		int ActiveResult=0;	//���ж�����ֶ�0Ϊ�ɹ���1Ϊʧ�ܣ�
		string strState=ptjc.get<string>("State");
		if ("ok"==strState)
		{
			ActiveResult=0;
		}
		else
		{
			ActiveResult=1;
		}
		ptccb.put("root.ActiveResult",ActiveResult);
		//1.1�汾������λ��������ECIES���ܵ�PSK����Lock_Init_Info�ֶη���
		ptccb.put("root.ActInfo",ptjc.get<string>("Lock_Init_Info"));
	}

	//////////////////////////////////////////////////////////////////////////

	//��ȡ������
	void zwconvReadCloseCodeDown( const ptree &ptccb, ptree &ptjc )
	{
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
		ptccb.put("root.TransCode","0004");
		assert("ReadShutLockCode"==ns_ReadCloseCodeName);
		ptccb.put("root.TransName",ns_ReadCloseCodeName);
		string zwDate,zwTime;
		zwGetDateTimeString(time(NULL),zwDate,zwTime);
		ptccb.put("root.TransDate",zwDate);
		ptccb.put("root.TransTime",zwTime);

		ptccb.put("root.DevCode",ns_ccbAtmno);
		ptccb.put("root.LockMan",LOCKMAN_NAME);
		ptccb.put("root.LockId",ptjc.get<string>("Lock_Serial"));
		ptccb.put("root.ShutLockcode",ptjc.get<int>("Code"));
	}

	void zwconvRecvInitCloseCodeDown(const ptree &ptccb, ptree &ptjc )
	{
		ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,"Lock_Close_Code_Lock");
	}


	void zwconvRecvInitCloseCodeUp(const ptree &ptjc, ptree &ptccb)
	{
		ptccb.put("root.TransCode","1000");
		ptccb.put("root.TransName","SendShutLockCode");
		string zwDate,zwTime;
		zwGetDateTimeString(time(NULL),zwDate,zwTime);
		ptccb.put("root.TransDate",zwDate);
		ptccb.put("root.TransTime",zwTime);
		//���߷��ͳ�ʼ������ʱ��ATM���Ӧ���Ѿ��ڼ��������л�ã�����
		//1.1�汾��������û�и��������Դ˴����ܻ�������
		ptccb.put("root.DevCode",ns_ccbAtmno);	
		ptccb.put("root.LockMan",LOCKMAN_NAME);
		ptccb.put("root.ShutLockcode",ptjc.get<int>("Code"));
	}

	void zwconvRecvVerifyCodeDown(const ptree &ptccb, ptree &ptjc )
	{
		ptjc.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,"Lock_Open_Ident");
	}

	

	void zwconvRecvVerifyCodeUp(const ptree &ptjc, ptree &ptccb)
	{
		ptccb.put("root.TransCode","1002");
		ptccb.put("root.TransName","SendUnLockIdent");
		string zwDate,zwTime;
		zwGetDateTimeString(time(NULL),zwDate,zwTime);
		ptccb.put("root.TransDate",zwDate);
		ptccb.put("root.TransTime",zwTime);
		//���߷��ͳ�ʼ������ʱ��ATM���Ӧ���Ѿ��ڼ��������л�ã�����
		//1.1�汾��������û�и��������Դ˴����ܻ�������
		ptccb.put("root.DevCode",ns_ccbAtmno);	
		ptccb.put("root.LockMan",LOCKMAN_NAME);
		ptccb.put("root.LockId",ptjc.get<string>("Lock_Serial"));
		//�ؼ�����֤�뱾��
		ptccb.put("root.UnLockIdentInfo",ptjc.get<int>("Lock_Ident_Info"));
	}

	

	////////////////////////////ÿһ�����ĵľ��崦��������//////////////////////////////////////////////
}	//namespace jcAtmcConvertDLLLock_Open_Ident
