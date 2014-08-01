#include "stdafx.h"
#include "CCBelock.h"
using namespace boost::property_tree;
//��ATMC DLL��XML��JSON��ת���������ڴˣ����ڵ�Ԫ���ԣ�
namespace jcAtmcConvertDLL{
	const char *LOCKMAN_NAME="BeiJing.JinChu";

	void zwconvLockActive( ptree &pt2 );
const JC_MSG_TYPE zwXML2Json( const string &downXML,string &downJson )
{
	//����ATMC�·���XML��ת��Ϊ�м���ʽptree
	assert(downXML.length()>42);	//XML��ͷ�Ĺ̶�����38���ַ����������һ����ǩ�����Լ����źϼ�4���ַ�
	cout<<"*********************ATMC Lock Active DOWN XML Start	*********************\n";
	cout<<downXML<<endl;
	cout<<"*********************ATMC Lock Active DOWN XML End	*********************\n";

	JC_MSG_TYPE msgType=JCMSG_INVALID_TYPE;
	ptree pt;
	std::stringstream ss;
	ss<<downXML;
	read_xml(ss,pt);
	//�ж���Ϣ����
	string transCode=pt.get<string>("TransCode");
	//������Ϣ���͵��ò�ͬ��������		
	ptree pt2;
	if ("0000"==transCode)
	{//���߼���
		msgType= JCMSG_LOCK_ACTIVE_REQUEST;
		zwconvLockActive(pt2);
	}
	//���������ΪJson����λ��ʹ��
	std::stringstream ss2;
	write_json(ss2,pt2);
	downJson= ss2.str();
	cout<<"***ATMC DLL json down after convert by DLL Start********************\n"<<downJson;
	cout<<"***ATMC DLL json down after convert by DLL End**********************\n";

	return msgType;
}

const JC_MSG_TYPE zwJson2XML( const string &upJson,string &upXML )
{
	//����λ�����ն�����json����ַ���������Ϊ�м���ʽptree
	assert(upJson.length()>9);	//json������ķ����������Ҫ9���ַ�����
	ptree pt;
	std::stringstream ss;
	ss<<upJson;
	read_json(ss,pt);
//////////////////////////////////////////////////////////////////////////
	pt.put("TransCode","0000");
	pt.put("LockMan",LOCKMAN_NAME);
	pt.erase("State");
	std::stringstream sst1;
	write_json(sst1,pt);
	string afterJson=sst1.str();
//////////////////////////////////////////////////////////////////////////
	cout<<"***ATMC DLL json up after convert by DLL Start********************\n"<<afterJson;
	cout<<"***ATMC DLL json up after convert by DLL End**********************\n";

	std::stringstream ss2;
	write_xml(ss2,pt);
	upXML= ss2.str();

	cout<<"*********************ATMC Lock Active UP XML Start	*********************\n";
	cout<<upXML<<endl;
	cout<<"*********************ATMC Lock Active UP XML End	    *********************\n";

	string transCode=pt.get<string>("TransCode");
	if ("0000"==transCode)
	{
		return JCMSG_LOCK_ACTIVE_REQUEST;
	}
	return JCMSG_INVALID_TYPE;
}

void zwconvLockActive( ptree &pt2 )
{
	pt2.put("command","Lock_Secretkey");
	pt2.put("State","get");
	pt2.put("Public_Key","123456");	//������ʵ�����壬����json�ӿ������У���д��
}

}	//namespace jcAtmcConvertDLL