#include "stdafx.h"
#include "CCBelock.h"
using namespace boost::property_tree;
void myLockActive(const JC_MSG_TYPE type, ptree &pt );

int zwjclms_command_proc(const string &inJson,string &outJson)
{
//	CCB 1.1版本算法ECIES(椭圆曲线集成加密公钥算法)安全初始化演示开始
//		ECIES PubKey=   BJpnccGKE5muLO3RLOe+hDjUftMJJwpmnuxEir0P3ss5/sxpEKNQ5AXcSsW1CbC/pXlqAk9/NZoquFJXHW3n1Cw=,
//		ECIES Prikey=   9jcUogAorQy6rvrvTZCxodoKLlDoYj/dwAFpl8bISWg=
//PlainText1:     myplaintext20140717.0918.012myplaintext20140717.0918.012end920;AAABBB
//cryptText:      BBkaJFP63beEM+FKVwna/qEMXoRe6KoJmPOKb8c9YGxFJtyImCVc8zZbcMJ3xnIM
//				r1yY51Azq2YFSh5nWoDcpeM=.0/i40FFzWarIOWc2tYaI4TU7vwMgsL++.8lONn76e78R64gKXu85nCC
//				6HpoRgtgZpkR9QJCHC6bSlJDUAqSVW5oSufccOjUkIBBJVGIkS9TrhBRsCbqDlwxXgT4aMvs+DXmAyriaw+Ko=
	ptree pt;
	std::stringstream ss;
	ss<<inJson;
	read_json(ss,pt);

	pt.put("LockMan","BeiJing JinChu");
	pt.put("LockId","ZWFAKELOCKNO1548");
	pt.put("LockPubKey","BJpnccGKE5muLO3RLOe+hDjUftMJJwpmnuxEir0P3ss5/sxpEKNQ5AXcSsW1CbC/pXlqAk9/NZoquFJXHW3n1Cw=,");
	std::stringstream ss2;
	write_json(ss2,pt);
	outJson=ss2.str();
	return 0;
}

void myLockActive(const JC_MSG_TYPE type, ptree &pt )
{
	cout<<__FUNCTION__<<endl;
	string myInStr=pt.get<string>("SpareString1");
	//测试性的修改一个值,使得上面能看到效果
	pt.put("SpareString1", myInStr+"ADD BY EMU SRV 730.1443");
	pt.put("LockMan","BeiJing JinChu");
	pt.put("LockId","ZWFAKELOCKNO1548");
	pt.put("LockPubKey","BJpnccGKE5muLO3RLOe+hDjUftMJJwpmnuxEir0P3ss5/sxpEKNQ5AXcSsW1CbC/pXlqAk9/NZoquFJXHW3n1Cw=,");
}
