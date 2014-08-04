#include "stdafx.h"
#include "zwCcbElockHdr.h"
using namespace boost::property_tree;
//�����߷������˴���Json�ĺ��������ڴˣ����ڵ�Ԫ����

const JC_MSG_TYPE lockParseJson( const string & inJson, ptree &pt );
void LockOutJson( const ptree &pt, string &outJson );
//�ڲ����������ر�¶�����
void myLockActive(ptree &pt );
void myLockInit(ptree &pt );
void myReadCloseCode(ptree &pt );

//�˺���û��ʵ�ʹ��ܣ����ܶ����¼��Ӻ����У����ڵ�Ԫ���ԣ��˺����ڴ������������ĵ�Ԫ���Եģ�
int zwjclms_command_proc(const string &inJson,string &outJson)
{
	if (inJson.length()<=9)
	{
		return 0;
	}
	assert(inJson.length()>9);	//json������ķ����������Ҫ9���ַ�����
	cout<<"***ģ�������յ���JSON����ʼ***********************\n";
	cout<<inJson;
	cout<<"***ģ�������յ���JSON�������***********************\n";
	ptree pt;
	//��������JSON�������䴦�������pt�У����ͷ�����mtype��
	JC_MSG_TYPE mtype=lockParseJson(inJson, pt);
	//��pt���Ϊjson����
	LockOutJson(pt, outJson);
	cout<<"***ģ���������ɵ�JSONӦ��ʼ***********************\n";
	cout<<outJson;
	cout<<"***ģ���������ɵ�JSONӦ�����***********************\n";

	return mtype;
}



//������Json,������࣬Ȼ�����ݽ�����pt�У������ݷ��������Ӧ�ĵײ㺯��������֯��Ӧ�ķ���ֵ�Ĵ������pt�У����ط���
const JC_MSG_TYPE lockParseJson( const string & inJson, ptree &pt )
{

	JC_MSG_TYPE mtype=JCMSG_INVALID_TYPE;
	std::stringstream ss;
	ss<<inJson;
	read_json(ss,pt);

	try{
		string sCommand=pt.get<string>("command");	
		if ("Lock_Secretkey"==sCommand)
		{//�����߼������󣬽�����ش���
			myLockActive(pt);
			return JCMSG_LOCK_ACTIVE_REQUEST;
		}
		if ("Lock_Init"==sCommand)
		{//�����߳�ʼ�����󣬽�����ش���
			myLockInit(pt);
			return JCMSG_SEND_LOCK_ACTIVEINFO;
		}
		if ("Lock_Close_code"==sCommand)
		{//�����߳�ʼ�����󣬽�����ش���
			myReadCloseCode(pt);
			return JCMSG_GET_CLOSECODE;
		}
	}
	catch(...)
	{
		OutputDebugStringA("CPPEXECPTION804");
		OutputDebugStringA(__FUNCTION__);
		cout<<"input json not found command item!20140801.1431"<<endl;
		return JCMSG_INVALID_TYPE;
	}


	return mtype;
}

//�Ѵ�����ϵ�pt�е��������ΪJson���
void LockOutJson( const ptree &pt, string &outJson )
{
	std::stringstream ss2;
	write_json(ss2,pt);
	outJson=ss2.str();
	assert(outJson.length()>9);
}


//���߼�����Ϣ�ľ��崦������
void myLockActive(ptree &pt )
{
	cout<<"���߼�������"<<endl;
	//pt.put("LockMan","BeiJing JinChu");
	pt.put("Lock_Serial","ZWFAKELOCKNO1548");
	pt.put("Public_Key","BJpnccGKE5muLO3RLOe+hDjUftMJJwpmnuxEir0P3ss5/sxpEKNQ5AXcSsW1CbC/pXlqAk9/NZoquFJXHW3n1Cw=,");
	pt.put("State","get");

}

//���߳�ʼ���ľ��崦������
void myLockInit(ptree &pt )
{
	cout<<"���߳�ʼ��"<<endl;
	ptree pt2;
	pt2.put("command","Lock_Init");
	pt2.put("State","ok");
	pt=pt2;
}


//��ȡ��������Ϣ�ľ��崦����
void myReadCloseCode(ptree &pt )
{
	cout<<"��ȡ������"<<endl;
	//>> �������ͱ���������λ��
	//{
	//	"command": "Lock_Close_code",
	//		"Lock_Close_code": "12345678",  //uint64_t
	//		"State": "push"
	//}
	ptree pt2;
	pt2.put("command","Lock_Close_code");
	pt2.put("Lock_Close_code","11112222");
	pt2.put("State","push");
	pt=pt2;
}