#include "stdafx.h"
#include "CCBelock.h"
using namespace boost::property_tree;
//��ATMC DLL��XML��JSON��ת���������ڴˣ����ڵ�Ԫ���ԣ�
namespace jcAtmcConvertDLL{
	const char *LOCKMAN_NAME="BeiJing.JinChu";
	//�·�������
	void zwconvLockActiveDown(const ptree &ptccb, ptree &ptjc );
	//�ϴ�������
	void zwconvLockActiveUp(const ptree &ptjc, ptree &ptccb );

const JC_MSG_TYPE zwCCBxml2JCjson( const string &downXML,string &downJson )
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
	//�ӽ��еĽӿ������ֶα�Ϊ���ǵ�JSON�ӿ�
	ptree pt2;
	if ("0000"==transCode)
	{//���߼���
		msgType= JCMSG_LOCK_ACTIVE_REQUEST;
		zwconvLockActiveDown(pt,pt2);
	}
	//���������ΪJson����λ��ʹ��
	std::stringstream ss2;
	write_json(ss2,pt2);
	downJson= ss2.str();
	cout<<"***ATMC DLL json down after convert by DLL Start********************\n"<<downJson;
	cout<<"***ATMC DLL json down after convert by DLL End**********************\n";

	return msgType;
}

const JC_MSG_TYPE zwJCjson2CCBxml( const string &upJson,string &upXML )
{
	//����λ�����ն�����json����ַ���������Ϊ�м���ʽptree
	assert(upJson.length()>9);	//json������ķ����������Ҫ9���ַ�����
	ptree ptjc;
	std::stringstream ss;
	ss<<upJson;
	read_json(ss,ptjc);
	//�ж���Ϣ���Ͳ������ǵ�JSON�ӿڱ�Ϊ���еĽӿ������ֶ�
	string jcCmd=ptjc.get<string>("command");
	ptree ptccb;
	if ("Lock_Secretkey"==jcCmd)
	{
		zwconvLockActiveUp(ptjc,ptccb);
	}

//////////////////////////////////////////////////////////////////////////
	
	std::stringstream sst1;
	write_json(sst1,ptccb);
	string jsonCcb=sst1.str();
//////////////////////////////////////////////////////////////////////////
	cout<<"***ATMC DLL jsonCcb after convert by DLL Start********************\n"<<jsonCcb;
	cout<<"***ATMC DLL jsonCcb after convert by DLL End**********************\n";

	//ת��JSONΪXML��ATMCʹ��
	std::stringstream ss2;
	write_xml(ss2,ptccb);
	upXML= ss2.str();

	cout<<"*********************ATMC Lock Active UP XML Start	*********************\n";
	cout<<upXML<<endl;
	cout<<"*********************ATMC Lock Active UP XML End	    *********************\n";

	string transCode=ptccb.get<string>("TransCode");
	if ("0000"==transCode)
	{
		return JCMSG_LOCK_ACTIVE_REQUEST;
	}
	return JCMSG_INVALID_TYPE;
}

////////////////////////////ÿһ�����ĵľ��崦������ʼ//////////////////////////////////////////////

//�·�������
void zwconvLockActiveDown( const ptree &ptccb, ptree &ptjc )
{
	ptjc.put("command","Lock_Secretkey");
	ptjc.put("State","get");
	ptjc.put("Public_Key","123456");	//������ʵ�����壬����json�ӿ������У���д��
}

//�ϴ�������
void zwconvLockActiveUp( const ptree &ptjc, ptree &ptccb )
{
	//���õ���ʽ������,δ��
	ptccb.put("TransCode","0000");
	//���ò���
	ptccb.put("LockId",ptjc.get<string>("Lock_Serial"));
	ptccb.put("LockPubKey",ptjc.get<string>("Public_Key"));	
	ptccb.put("LockMan",LOCKMAN_NAME);
}
////////////////////////////ÿһ�����ĵľ��崦��������//////////////////////////////////////////////
}	//namespace jcAtmcConvertDLL