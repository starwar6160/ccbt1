#include "stdafx.h"
#include "CCBelock.h"
using namespace boost::property_tree;
//�����߷������˴���Json�ĺ��������ڴˣ����ڵ�Ԫ����

const JC_MSG_TYPE lockParseJson( const string & inJson, ptree &pt );
void LockOutJson( const ptree &pt, string &outJson );
//�ڲ����������ر�¶�����
void myLockActive(const JC_MSG_TYPE type, ptree &pt );

//�˺���û��ʵ�ʹ��ܣ����ܶ����¼��Ӻ����У����ڵ�Ԫ���ԣ��˺����ڴ������������ĵ�Ԫ���Եģ�
int zwjclms_command_proc(const string &inJson,string &outJson)
{
	//	CCB 1.1�汾�㷨ECIES(��Բ���߼��ɼ��ܹ�Կ�㷨)��ȫ��ʼ����ʾ��ʼ
	//		ECIES PubKey=   BJpnccGKE5muLO3RLOe+hDjUftMJJwpmnuxEir0P3ss5/sxpEKNQ5AXcSsW1CbC/pXlqAk9/NZoquFJXHW3n1Cw=,
	//		ECIES Prikey=   9jcUogAorQy6rvrvTZCxodoKLlDoYj/dwAFpl8bISWg=
	//PlainText1:     myplaintext20140717.0918.012myplaintext20140717.0918.012end920;AAABBB
	//cryptText:      BBkaJFP63beEM+FKVwna/qEMXoRe6KoJmPOKb8c9YGxFJtyImCVc8zZbcMJ3xnIM
	//				r1yY51Azq2YFSh5nWoDcpeM=.0/i40FFzWarIOWc2tYaI4TU7vwMgsL++.8lONn76e78R64gKXu85nCC
	//				6HpoRgtgZpkR9QJCHC6bSlJDUAqSVW5oSufccOjUkIBBJVGIkS9TrhBRsCbqDlwxXgT4aMvs+DXmAyriaw+Ko=
	if (inJson.length()<=9)
	{
		return 0;
	}
	assert(inJson.length()>9);	//json������ķ����������Ҫ9���ַ�����
	ptree pt;
	//��������JSON�������䴦�������pt�У����ͷ�����mtype��
	JC_MSG_TYPE mtype=lockParseJson(inJson, pt);
	//��pt���Ϊjson����
	LockOutJson(pt, outJson);

	return mtype;
}

//���߼�����Ϣ�ľ��崦������
void myLockActive(const JC_MSG_TYPE type, ptree &pt )
{
	pt.put("LockMan","BeiJing JinChu");
	pt.put("LockId","ZWFAKELOCKNO1548");
	pt.put("LockPubKey","BJpnccGKE5muLO3RLOe+hDjUftMJJwpmnuxEir0P3ss5/sxpEKNQ5AXcSsW1CbC/pXlqAk9/NZoquFJXHW3n1Cw=,");
}

//������Json,������࣬Ȼ�����ݽ�����pt�У������ݷ��������Ӧ�ĵײ㺯��������֯��Ӧ�ķ���ֵ�Ĵ������pt�У����ط���
const JC_MSG_TYPE lockParseJson( const string & inJson, ptree &pt )
{
	JC_MSG_TYPE mtype=JCMSG_INVALID_TYPE;
	std::stringstream ss;
	ss<<inJson;
	read_json(ss,pt);

	string transCode=pt.get<string>("TransCode");
	assert(transCode.length()==4);	//���ս��е�Ҫ�����4λ��
	//����transCode�ַ���ȷ����ʲô����
	if ("0000"==transCode)
	{
		mtype=JCMSG_LOCK_ACTIVE_REQUEST;
		myLockActive(mtype,pt);
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
