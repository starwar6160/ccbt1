#include "stdafx.h"
#include "zwCcbElockHdr.h"
using namespace boost::property_tree;
//��ATMC DLL��XML��JSON��ת���������ڴˣ����ڵ�Ԫ���ԣ�
namespace jcAtmcConvertDLL{
	const char *LOCKMAN_NAME="BeiJing.JinChu";
	//�·�������
	void zwconvLockActiveDown(const ptree &ptccb, ptree &ptjc );
	void zwconvSendLockActInfoDown( const ptree &ptccb, ptree &ptjc );
	void zwconvReadCloseCodeDown( const ptree &ptccb, ptree &ptjc );
	//�ϴ�������
	void zwconvLockActiveUp(const ptree &ptjc, ptree &ptccb );
	void zwconvLockInitUp( const ptree &ptjc, ptree &ptccb );
	void zwconvReadCloseCodeUp( const ptree &ptjc, ptree &ptccb );
	//����4���ֶΣ�Ϊ����������ת���ڼ䱣�潨�б���������ģ����ǻ������õ��ֱ��뷵�ظ����е��ֶ�
	string ns_ccbTransName;	//��������
	string ns_ccbDate;		//����
	string ns_ccbTime;		//ʱ��
	string ns_ccbAtmno;		//ATM���
	string ns_jcLockno;		//���߱��


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
	//���潨�������ֶ��Ա��ϴ�����ʱ�ṩ������
	ns_ccbTransName=pt.get<string>("TransName");
	ns_ccbDate=pt.get<string>("TransDate");
	ns_ccbTime=pt.get<string>("TransTime");
	ns_ccbAtmno=pt.get<string>("DevCode");
	//������Ϣ���͵��ò�ͬ��������	
	//�ӽ��еĽӿ������ֶα�Ϊ���ǵ�JSON�ӿ�
	ptree pt2;
	if ("0000"==transCode)
	{//���߼�������
		msgType= JCMSG_LOCK_ACTIVE_REQUEST;
		zwconvLockActiveDown(pt,pt2);
	}
	if ("0001"==transCode)
	{//�������߼�����Ϣ(��ʼ��)
		msgType= JCMSG_SEND_LOCK_ACTIVEINFO;
		zwconvSendLockActInfoDown(pt,pt2);
	}
	if ("0004"==transCode)
	{//��ȡ������
		msgType= JCMSG_GET_CLOSECODE;
		zwconvReadCloseCodeDown(pt,pt2);
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
	if ("Lock_Init"==jcCmd)
	{
		zwconvLockInitUp(ptjc,ptccb);
	}
	if ("Lock_Close_code"==jcCmd)
	{
		zwconvReadCloseCodeUp(ptjc,ptccb);
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
{	//�������߼�������
	ptjc.put("command","Lock_Secretkey");
	ptjc.put("State","get");
	ptjc.put("Public_Key","123456");	//������ʵ�����壬����json�ӿ������У���д��
}

void zwconvSendLockActInfoDown( const ptree &ptccb, ptree &ptjc )
{	//�������߼�����Ϣ(���߳�ʼ��)
	//���߳�ʼ��

	//>> ��λ���·�
	//{
	//	"command": "Lock_Init",
	//		"Lock_Init_Info": {
	//			"Atm_Serial": "123456",
	//				"Lms_Clock": "123456", 
	//				"Lock_Psk": "123456"
	//	},
	//	"State": "set"
	//}

	//>> ������ȷ���պ󣬷���
	//{
	//	"command": "Lock_Init",
	//		"State": "ok"
	//}
	//ptjc=ptccb;
	ptjc.put("command","Lock_Init");
	ptjc.put("State","set");
	ptjc.put("Lock_Init_Info.Atm_Serial",ptccb.get<string>("DevCode"));	
	//���ֶΣ�JCҪ��Ӧ����PSK����,CCB������ֻ�ṩ��Կ���ܹ�������ģ����ṩPSK����
	//�˴��õ�ֵ��atmc������Ϣ�����ļ���ͷ��һ��ֵ�����PSK����,ʵ��ʹ��ʱ��ô�죿������ȡ�ã�
	ptjc.put("Lock_Init_Info.Lock_Psk","77498EB7D7CE8B92D871791C99B85AB337FF73235A89E7A20764EFE6EA41E4CE");
	//�����ֶ�(LMSʱ��)��JC���У�CCBû��
	ptjc.put("Lock_Init_Info.Lms_Clock",time(NULL));
	//�����ֶ���CCB�У�JCû��
	ptjc.put("ccb_Init_Info.LmsPubKey",ptccb.get<string>("PswSrvPubKey"));
	ptjc.put("ccb_Init_Info.ActInfo",ptccb.get<string>("ActInfo"));
	
}

//��ȡ������
void zwconvReadCloseCodeDown( const ptree &ptccb, ptree &ptjc )
{
	//>> �������ͱ���������λ��
	//{
	//	"command": "Lock_Close_code",
	//		"Lock_Close_code": "12345678",  //uint64_t
	//		"State": "push"
	//}

	ptjc=ptccb;
}


//�ϴ�������
void zwconvLockActiveUp( const ptree &ptjc, ptree &ptccb )
{
	//���õ���ʽ������
	ptccb.put("TransCode","0000");
	ptccb.put("TransName",ns_ccbTransName);
	ptccb.put("TransDate",ns_ccbDate);
	ptccb.put("TransTime",ns_ccbTime);
	ptccb.put("DevCode",ns_ccbAtmno);
	//���ò���
	ns_jcLockno=ptjc.get<string>("Lock_Serial");
	ptccb.put("LockId",ns_jcLockno);
	ptccb.put("LockPubKey",ptjc.get<string>("Public_Key"));	
	ptccb.put("LockMan",LOCKMAN_NAME);
}

void zwconvLockInitUp( const ptree &ptjc, ptree &ptccb )
{
	//���õ���ʽ������
	ptccb.put("TransCode","0001");
	ptccb.put("TransName",ns_ccbTransName);
	ptccb.put("TransDate",ns_ccbDate);
	ptccb.put("TransTime",ns_ccbTime);
	ptccb.put("DevCode",ns_ccbAtmno);
	ptccb.put("LockMan",LOCKMAN_NAME);
	//�˴�JC�ϴ������޴��ֶΣ���Ҫ���,Ŀǰ�ǴӼ������󷵻ص�ֵ�����Ժ󱣴����ڴ��У�
	//ϣ����������֮����DLL��ж��֮ǰ���������߳�ʼ��������������޴���ȷ������߱�Ŷ��������
	ptccb.put("LockId",ns_jcLockno);	
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
	//���ֶ���λ��ʵ����û�з��أ���ôȡ�û�������
	ptccb.put("ActInfo","77498EB7D7CE8B92D871791C99B85AB337FF73235A89E7A20764EFE6EA41E4CE");
	ptccb.put("SpareString1","SendActInfoCCBReverse1");
	ptccb.put("SpareString2","SendActInfoCCBReverse2");

}

void zwconvReadCloseCodeUp( const ptree &ptjc, ptree &ptccb )
{
	ptccb=ptjc;
}


////////////////////////////ÿһ�����ĵľ��崦��������//////////////////////////////////////////////
}	//namespace jcAtmcConvertDLL