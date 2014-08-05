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
		ptree pt;
		std::stringstream ss;
		ss<<downXML;
		read_xml(ss,pt);
		//////////////////////////////////////////////////////////////////////////

		std::stringstream ssccb;
		write_json(ssccb,pt);
		string ccbJson= ssccb.str();
		cout<<"***����XMLת�����JSON��ʼ********************************\n"<<ccbJson;
		cout<<"***����XMLת�����JSON����********************************\n";


		//�ж���Ϣ����
		string transCode=pt.get<string>("TransCode");
		//���潨�������ֶ��Ա��ϴ�����ʱ�ṩ������		
		ns_ccbAtmno=pt.get<string>("DevCode");
		//������Ϣ���͵��ò�ͬ��������	
		//�ӽ��еĽӿ������ֶα�Ϊ���ǵ�JSON�ӿ�
		ptree pt2;
		if ("0000"==transCode)
		{//���߼�������
		ns_ActReqName=pt.get<string>("TransName");
			msgType= JCMSG_LOCK_ACTIVE_REQUEST;
			zwconvLockActiveDown(pt,pt2);
		}
		if ("0001"==transCode)
		{//�������߼�����Ϣ(��ʼ��)
			ns_LockInitName=pt.get<string>("TransName");
			msgType= JCMSG_SEND_LOCK_ACTIVEINFO;
			zwconvLockInitDown(pt,pt2);
		}
		if ("0004"==transCode)
		{//��ȡ������
			ns_ReadCloseCodeName=pt.get<string>("TransName");
			msgType= JCMSG_GET_CLOSECODE;
			zwconvReadCloseCodeDown(pt,pt2);
		}
		//���������ΪJson����λ��ʹ��
		std::stringstream ss2;
		write_json(ss2,pt2);
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
		cout<<"***�����߷��ص�JSONӦ��ʼ*****************************\n"<<jsonJc;
		cout<<"***�����߷��ص�JSONӦ�����*****************************\n";


		//�ж���Ϣ���Ͳ������ǵ�JSON�ӿڱ�Ϊ���еĽӿ������ֶ�
		string jcCmd=ptJC.get<string>(jcAtmcConvertDLL::JCSTR_CMDTITLE);
		ptree ptCCB;
		ptCCB=ptJC;	//��������������κ�һ�����򱣳�ԭ��
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

		//////////////////////////////////////////////////////////////////////////

		std::stringstream sst2;
		write_json(sst2,ptCCB);
		string jsonCcb=sst2.str();
		//////////////////////////////////////////////////////////////////////////
		cout<<"***������Ӧ��JSON����Ϊ��������Ԫ�� ��ʼ****************\n"<<jsonCcb;
		cout<<"***������Ӧ��JSON����Ϊ��������Ԫ�� ����****************\n";

		//ת��JSONΪXML��ATMCʹ��
		std::stringstream ss2;
		write_xml(ss2,ptCCB);
		upXML= ss2.str();

		cout<<"*********************��Ӧ��XML��ʼ**********************\n";
		cout<<upXML<<endl;
		cout<<"*********************��Ӧ��XML����**********************\n";

		string transCode=ptCCB.get<string>("TransCode");
		if ("0000"==transCode)
		{
			return JCMSG_LOCK_ACTIVE_REQUEST;
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
		ptccb.put("TransCode","0000");
		assert("CallForActInfo"==ns_ActReqName);
		ptccb.put("TransName",ns_ActReqName);
		ptccb.put("DevCode",ns_ccbAtmno);
//////////////////////////////////////////////////////////////////////////
		string zwDate,zwTime;
		zwGetDateTimeString(ptjc.get<time_t>("Lock_Time"),zwDate,zwTime);
		ptccb.put("TransDate",zwDate);
		ptccb.put("TransTime",zwTime);

//////////////////////////////////////////////////////////////////////////
		//���ò���
		
		ptccb.put("LockId",ptjc.get<string>("Lock_Serial"));
		ptccb.put("LockPubKey",ptjc.get<string>("Lock_Public_Key"));	
		ptccb.put("LockMan",LOCKMAN_NAME);
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
		ptjc.put("Atm_Serial",ptccb.get<string>("DevCode"));	
		//�˴�1.1�汾�Ѿ�֧��ECIES���ܹ���PSK�����ˣ�ʹ�õ�һ������
		ptjc.put("Lock_Init_Info",ptccb.get<string>("ActInfo"));
		//�����ֶ���CCB�У�JCû��
		ptjc.put("PswSrvPubKey",ptccb.get<string>("PswSrvPubKey"));
		
	}

	void zwconvLockInitUp( const ptree &ptjc, ptree &ptccb )
	{
		//���õ���ʽ������
		ptccb.put("TransCode","0001");
		ptccb.put("TransName",ns_LockInitName);	//ʹ�û������ڴ��е�ֵ
		assert("SendActInfo"==ns_LockInitName);
		string zwDate,zwTime;
		zwGetDateTimeString(ptjc.get<time_t>("Lock_Time"),zwDate,zwTime);
		ptccb.put("TransDate",zwDate);
		ptccb.put("TransTime",zwTime);
		ptccb.put("DevCode",ns_ccbAtmno);	//ʹ�û������ڴ��е�ֵ
		ptccb.put("LockMan",LOCKMAN_NAME);
		ptccb.put("LockId",ptjc.get<string>("Lock_Serial"));	
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
		ptccb.put("ActiveResult",ActiveResult);
		//1.1�汾������λ��������ECIES���ܵ�PSK����Lock_Init_Info�ֶη���
		ptccb.put("ActInfo",ptjc.get<string>("Lock_Init_Info"));
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
		ptccb.put("TransCode","0004");
		assert("ReadShutLockCode"==ns_ReadCloseCodeName);
		ptccb.put("TransName",ns_ReadCloseCodeName);
		string zwDate,zwTime;
		zwGetDateTimeString(time(NULL),zwDate,zwTime);
		ptccb.put("TransDate",zwDate);
		ptccb.put("TransTime",zwTime);

		ptccb.put("DevCode",ns_ccbAtmno);
		ptccb.put("LockMan",LOCKMAN_NAME);
		ptccb.put("LockId",ptjc.get<string>("Lock_Serial"));
		ptccb.put("ShutLockcode",ptjc.get<int>("Code"));
	}


	////////////////////////////ÿһ�����ĵľ��崦��������//////////////////////////////////////////////
}	//namespace jcAtmcConvertDLL