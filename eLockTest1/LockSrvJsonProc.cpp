#include "stdafx.h"
#include "zwCcbElockHdr.h"
using namespace boost::property_tree;
//�����߷������˴���Json�ĺ��������ڴˣ����ڵ�Ԫ����

const JC_MSG_TYPE lockParseInJson( const string & inJson, ptree &pt );
void LockGenOutputJson( const ptree &pt, string &outJson );
//�ڲ����������ر�¶�����
void myLockActive(ptree &pt );
void myLockInit(ptree &ptIn );
void myReadCloseCode(ptree &pt );
void myGenInitCloseCodeJson( ptree &ptInOut );
void myGenVerifyCodeJson(ptree &ptInOut);

//�˺���û��ʵ�ʹ��ܣ����ܶ����¼��Ӻ����У����ڵ�Ԫ���ԣ��˺����ڴ������������ĵ�Ԫ���Եģ�
int zwjclms_command_proc(const string &inJson,string &outJson)
{
	if (inJson.length()<=9)
	{
		return 0;
	}
	assert(inJson.length()>9);	//json������ķ����������Ҫ9���ַ�����
	ZWTRACE("***ģ�������յ���JSON����ʼ***********************\n");
	ZWTRACE(inJson.c_str());
	ZWTRACE("***ģ�������յ���JSON�������***********************\n");
	ptree pt;
	//��������JSON�������䴦�������pt�У����ͷ�����mtype��
	JC_MSG_TYPE mtype=lockParseInJson(inJson, pt);
	//��pt���Ϊjson����
	LockGenOutputJson(pt, outJson);
	ZWTRACE("***ģ���������ɵ�JSONӦ��ʼ***********************\n");
	ZWTRACE(outJson.c_str());
	ZWTRACE("***ģ���������ɵ�JSONӦ�����***********************\n");

	return mtype;
}



//������Json,������࣬Ȼ�����ݽ�����pt�У������ݷ��������Ӧ�ĵײ㺯��������֯��Ӧ�ķ���ֵ�Ĵ������pt�У����ط���
const JC_MSG_TYPE lockParseInJson( const string & inJson, ptree &pt )
{

	JC_MSG_TYPE mtype=JCMSG_INVALID_TYPE;
	std::stringstream ss;
	ss<<inJson;
	read_json(ss,pt);
	ptree ptOut;

	try{
		string sCommand=pt.get<string>(jcAtmcConvertDLL::JCSTR_CMDTITLE);	
		if (jcAtmcConvertDLL::JCSTR_LOCK_ACTIVE_REQUEST==sCommand)
		{//�����߼������󣬽�����ش���
			myLockActive(pt);
			return JCMSG_LOCK_ACTIVE_REQUEST;
		}
		if (jcAtmcConvertDLL::JCSTR_LOCK_INIT==sCommand)
		{//�����߳�ʼ�����󣬽�����ش���
			myLockInit(pt);
			return JCMSG_SEND_LOCK_ACTIVEINFO;
		}
		if (jcAtmcConvertDLL::JCSTR_READ_CLOSECODE==sCommand)
		{//�����߳�ʼ�����󣬽�����ش���
			myReadCloseCode(pt);
			return JCMSG_GET_CLOSECODE;
		}
		if (jcAtmcConvertDLL::JCSTR_SEND_INITCLOSECODE==sCommand)
		{//�ǲ����õ�Ҫ�����ߡ����������ͳ�ʼ���������Ϣ
			myGenInitCloseCodeJson(pt);
			return JCMSG_SEND_INITCLOSECODE;
		}
		if (jcAtmcConvertDLL::JCSTR_SEND_UNLOCK_CERTCODE==sCommand)
		{//�ǲ����õ�Ҫ�����ߡ����������ͳ�ʼ���������Ϣ
			myGenVerifyCodeJson(pt);
			return JCMSG_SEND_UNLOCK_CERTCODE;
		}
	}
	catch(...)
	{
		OutputDebugStringA("CPPEXECPTION804C");
		OutputDebugStringA(__FUNCTION__);
		cout<<"input json not found command item!20140801.1431"<<endl;
		return JCMSG_INVALID_TYPE;
	}


	return mtype;
}

//�Ѵ�����ϵ�pt�е��������ΪJson���
void LockGenOutputJson( const ptree &pt, string &outJson )
{
	std::stringstream ss2;
	write_json(ss2,pt);
	outJson=ss2.str();
	assert(outJson.length()>9);
}


//���߼�����Ϣ�ľ��崦������
void myLockActive(ptree &ptInOut )
{
	cout<<"���߼�������"<<endl;
	ptInOut.put("Lock_Serial","ZWFAKELOCKNO1631");
	ptInOut.put("Lock_Public_Key","BJpnccGKE5muLO3RLOe+hDjUftMJJwpmnuxEir0P3ss5/sxpEKNQ5AXcSsW1CbC/pXlqAk9/NZoquFJXHW3n1Cw=,");
	ptInOut.put("Lock_Time",time(NULL));

}

//���߳�ʼ���ľ��崦������
void myLockInit( ptree &ptInOut  )
{
	ptree ptOut;
	cout<<"���߳�ʼ��"<<endl;
	ptOut.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,jcAtmcConvertDLL::JCSTR_LOCK_INIT);
	ptOut.put("Lock_Time",time(NULL));
	ptOut.put("Lock_Serial",ptInOut.get<string>("Atm_Serial"));
	ptOut.put("State","ok");
	//�ڴ��ݲ����㣬ֱ�ӷ���Ԥ�ȼ���õĵ�һ���������������PSK
	ptOut.put("Lock_Init_Info","77498EB7D7CE8B92D871791C99B85AB337FF73235A89E7A20764EFE6EA41E4CE");
	ptInOut=ptOut;
}


//��ȡ��������Ϣ�ľ��崦����
void myReadCloseCode(ptree &ptInOut )
{
	cout<<"��ȡ������"<<endl;
	//>> ���߷��ر���������λ��
	//{
	//	"command": JCSTR_READ_CLOSECODE,
	//		JCSTR_READ_CLOSECODE: "12345678",  //uint64_t
	ptree ptOut;
	ptOut.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,jcAtmcConvertDLL::JCSTR_READ_CLOSECODE);
	ptOut.put("Lock_Time",time(NULL));
	ptOut.put("Lock_Serial","ZWFAKELOCKNO1548");	
	ptOut.put("Code","11112222");	
	ptInOut=ptOut;
}

//���ɳ�ʼ�����뱨��
void myGenInitCloseCodeJson( ptree &ptInOut )
{
	ptree ptOut;
	ptOut.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,"Lock_Close_Code_Lock");
	ptOut.put("Lock_Time",time(NULL));
	ptOut.put("Lock_Serial","ZWFAKELOCKNO1548");	
	ptOut.put("Code","10007777");	
	ptInOut=ptOut;
}

//������֤�뱨��
void myGenVerifyCodeJson(ptree &ptInOut)
{
	ptree ptOut;
	ptOut.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,"Lock_Open_Ident");
	ptOut.put("Lock_Time",time(NULL));
	ptOut.put("Lock_Serial","ZWFAKELOCKNO1548");	
	ptOut.put("Lock_Ident_Info","10028888");	
	ptInOut=ptOut;
}