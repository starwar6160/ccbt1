#include "stdafx.h"
using namespace boost::property_tree;

int zwjclms_command_proc(const string &inJson,string &outJson)
{
	ptree pt;
	std::stringstream ss;
	ss<<inJson;
	read_json(ss,pt);
	string myInStr=pt.get<string>("SpareString1");
	//�����Ե��޸�һ��ֵ,ʹ�������ܿ���Ч��
	pt.put("SpareString1", myInStr+"ADD BY EMU SRV 730.1443");
	std::stringstream ss2;
	write_json(ss2,pt);
	outJson=ss2.str();
	return 0;
}