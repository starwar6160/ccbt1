#include "stdafx.h"
#include "CCBelock.h"
using namespace boost::property_tree;
//�����߷������˴���Json�ĺ��������ڴˣ����ڵ�Ԫ����
void myLockActive(const JC_MSG_TYPE type, ptree &pt );

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
	std::stringstream ss;
	ss<<inJson;
	read_json(ss,pt);

	string transCode=pt.get<string>("TransCode");
	assert(transCode.length()==4);	//���ս��е�Ҫ�����4λ��
	if ("0000"==transCode)
	{
		myLockActive(JCMSG_LOCK_ACTIVE_REQUEST,pt);
	}

	std::stringstream ss2;
	write_json(ss2,pt);
	outJson=ss2.str();
	assert(outJson.length()>9);
	return 0;
}

void myLockActive(const JC_MSG_TYPE type, ptree &pt )
{
	pt.put("LockMan","BeiJing JinChu");
	pt.put("LockId","ZWFAKELOCKNO1548");
	pt.put("LockPubKey","BJpnccGKE5muLO3RLOe+hDjUftMJJwpmnuxEir0P3ss5/sxpEKNQ5AXcSsW1CbC/pXlqAk9/NZoquFJXHW3n1Cw=,");
}
