#include "stdafx.h"
#include "zwCcbElockHdr.h"
using jcAtmcConvertDLL::CCBSTR_CODE;
//��ATMC�˵�XML��������XML���������ڴˣ����ڵ�Ԫ����
//��һ�׼�����Ϣ
//CCB 1.1�汾�㷨ECIES(��Բ���߼��ɼ��ܹ�Կ�㷨)��ȫ��ʼ����ʾ��ʼ
//	ECIES PubKey=   BK4gbEtRRx+W3CVn96/V0zYzZOnhRrBAYGgNgBeRVRnWz1vaU4i4b0GdvA/yVR1i
//	JBEHmHyDBN8W93f+1kIx4jM=,
//	ECIES Prikey=   lI+akxkuWIkTq4yphAG5h72I0yTNQr25W9lHavJOCMM=
//PSK:
//77498EB7D7CE8B92D871791C99B85AB337FF73235A89E7A20764EFE6EA41E4CE
//cryptText:      BG3j9JZxpssY0bdb1oMwg4obKmZ93GTvbbnY8VZnQIglLGO8m7JvhTlnvKPnsuBv
//				1vAySJell1QrrkiMhsob1oc=.StjumFvZi4W4j2n/NUliN/72PY72IHjT.7tnYku3DAGnbsAas0+E98i
//				Ql2mr+CoJbZcc2uQS3oVEFBbwtAgspY+oC+lSJcKI62395wPYkSG+F+Rd1Bj66CaVyBk8I7KvO/R+ofR
//				5BeeM=
//decryptPSK:
//77498EB7D7CE8B92D871791C99B85AB337FF73235A89E7A20764EFE6EA41E4CE

//ͬ����PSK����һ�鹫Կ˽Կ
//CCB 1.1�汾�㷨ECIES(��Բ���߼��ɼ��ܹ�Կ�㷨)��ȫ��ʼ����ʾ��ʼ
//	ECIES PubKey=   BAnTxOLq5q+bEPJlJJ8uvJf5mC6j4z5DkdCssb+7zSvjeZPuNHnyvd4yPoY1As5T
//	H9YBtRtCqtULO61+X2L8miM=,
//	ECIES Prikey=   te1ieS34jNV+pAZ1yRl8bPtSTyM/86sADoDSKIJtUlM=
//PSK:
//77498EB7D7CE8B92D871791C99B85AB337FF73235A89E7A20764EFE6EA41E4CE
//cryptText:      BHxPVBhRdyjSHRnj01c/d3VSRFLXcgwHQ4v7jmazmGIcQrv9snTBaw3J4R+9Kl/7
//				dQInhtkb5f/R0a9GoIlicDk=.7yR0xGdEn13xO0cxv4ulCcNIZ4mrteYi.0pmz+bQNIizFIaOM/n27B3
//				xHQlR9z8jFiB6Pm8Qapm/J9Fsh4GyeXBeeSAgT2gIsgAxr7y3Wk6ZkCX2x1qj6sMAlpTW4dN0/bLbSM5
//				0EMTY=
//decryptPSK:
//77498EB7D7CE8B92D871791C99B85AB337FF73235A89E7A20764EFE6EA41E4CE

using namespace boost::property_tree;
namespace jcAtmcMsg{
	const char *ATMNO_CCBTEST="CCBATMNO1234";
	string & myAtmcMsgLockActive( string & strXML ,ptree &pt);
	string & myAtmcMsgSendActiveInfo( string & strXML ,ptree &pt );
	string & myAtmcMsgReadCloseCodeInfo( string & strXML ,ptree &pt );
	string & myTestMsgRecvCloseCode( string & strXML ,ptree &pt );
	string & myTestMsgRecvVerifyCode( string & strXML ,ptree &pt );
//����ģ���ATMC XML��Ϣ������ڣ�����ö��������Ӧ��һ����XML��Ϣ
void zwAtmcMsgGen( const JC_MSG_TYPE type,string &strXML,ptree &pt )
{
	switch (type)
	{
	case JCMSG_LOCK_ACTIVE_REQUEST:
		strXML = myAtmcMsgLockActive(strXML, pt);
		assert(strXML.length()>42);	//XML��ͷ�Ĺ̶�����38���ַ����������һ����ǩ�����Լ����źϼ�4���ַ�
		break;
	case JCMSG_SEND_LOCK_ACTIVEINFO:
		strXML=myAtmcMsgSendActiveInfo(strXML,pt);
		assert(strXML.length()>42);
		break;
	case JCMSG_GET_CLOSECODE:
		strXML=myAtmcMsgReadCloseCodeInfo(strXML,pt);
		assert(strXML.length()>42);
		break;
//������������������ϴ���Ϣ�Ĳ������·���Ϣ,���в�����Щ�·�����
	case JCMSG_SEND_INITCLOSECODE:
		strXML=myTestMsgRecvCloseCode(strXML,pt);
		assert(strXML.length()>42);
		break;
	case JCMSG_SEND_UNLOCK_CERTCODE:
		strXML=myTestMsgRecvVerifyCode(strXML,pt);
		assert(strXML.length()>42);
		break;

	}
}
	


//�������߼���XML���ģ���ȡ���߱�ź͹�Կ
string & myAtmcMsgLockActive( string & strXML ,ptree &pt )
{
	//����
	//	���״���	TransCode	��	ֵ��0000
	//	��������	TransName	��	ֵ��CallForActInfo
	//	��������	TransDate	��	ֵ��YYYYMMDD����20140401
	//	����ʱ��	TransTime	��	ֵ��hhmmss����134050
	//	ATM�豸���	DevCode	��	ֵ������12λ�豸���
	//��ʼ����������
	pt.put(CCBSTR_CODE,"0000");
	pt.put("root.TransName","CallForActInfo");
	pt.put("root.TransDate","20140730");
	pt.put("root.TransTime","142248");
	pt.put("root.DevCode",ATMNO_CCBTEST);
	//pt.put("root.DevCode","12345678");

	std::ostringstream ss;
	write_xml(ss,pt);
	strXML=ss.str();
	assert(strXML.length()>42);	//XML��ͷ�Ĺ̶�����38���ַ����������һ����ǩ�����Լ����źϼ�4���ַ�
	return strXML;
	//Ӧ��
	//	���״���	TransCode	��	ֵ��0000
	//	��������	TransName	��	ֵ��CallForActInfo
	//	��������	TransDate	��	ֵ��YYYYMMDD����20140401
	//	����ʱ��	TransTime	��	ֵ��hhmmss����134050
	//	ATM�豸���	DevCode	��	ֵ������12λ�豸���
	//	���߳���	LockMan	��	ֵ�������Զ����������̲�ͬ������
	//	���߱��	LockId	��	ֵ�������Զ�������Ψһ���
	//	���߹�Կ	LockPubKey	��	ֵ��ÿ���������ɵĹ�˽Կ���еĹ�Կ 16���ַ�
}

//���ɷ������߼�����Ϣ���ģ����������߹�Կ���ܹ����PSK������
string & myAtmcMsgSendActiveInfo( string & strXML ,ptree &pt )
{
	//����
	//	���״���	TransCode	��	ֵ��0001
	//	��������	TransName	��	ֵ��SendActInfo
	//	��������	TransDate	��	ֵ��YYYYMMDD����20140401
	//	����ʱ��	TransTime	��	ֵ��hhmmss����134050
	//	ATM�豸���	DevCode	��	ֵ������12λ�豸���
	//	���߳���	LockMan	��	ֵ�������Զ����������̲�ͬ������
	//	���߱��	LockId	��	ֵ�������Զ�������Ψһ���
	//	������Ϣ	ActInfo	��	����ͼ��ܷ�������ɢ�������������߹�Կ���ܵļ�����Ϣ ����96���ַ�
	//��ʼ����������
	pt.put(CCBSTR_CODE,"0001");
	pt.put("root.TransName","SendActInfo");
	string zwDate,zwTime;
	zwGetDateTimeString(time(NULL),zwDate,zwTime);

	pt.put("root.TransDate",zwDate);
	pt.put("root.TransTime",zwTime);
	pt.put("root.DevCode",ATMNO_CCBTEST);
	pt.put("root.LockMan",jcAtmcConvertDLL::LOCKMAN_NAME);
	pt.put("root.LockId","ZWFAKELOCKNO1548");
	//ʹ�õ�һ�׼�����Ϣ,�ṩ"ActInfo"��ֵ
	pt.put("root.ActInfo","BG3j9JZxpssY0bdb1oMwg4obKmZ93GTvbbnY8VZnQIglLGO8m7JvhTlnvKPnsuBv"
	"1vAySJell1QrrkiMhsob1oc=.StjumFvZi4W4j2n/NUliN/72PY72IHjT.7tnYku3DAGnbsAas0+E98i"
	"Ql2mr+CoJbZcc2uQS3oVEFBbwtAgspY+oC+lSJcKI62395wPYkSG+F+Rd1Bj66CaVyBk8I7KvO/R+ofR5BeeM=");

	std::ostringstream ss;
	write_xml(ss,pt);
	strXML=ss.str();
	assert(strXML.length()>42);	//XML��ͷ�Ĺ̶�����38���ַ����������һ����ǩ�����Լ����źϼ�4���ַ�
	return strXML;

	//Ӧ��
	//���״���	TransCode	��	ֵ��0001
	//��������	TransName	��	ֵ��SendActInfo
	//��������	TransDate	��	ֵ��YYYYMMDD����20140401
	//����ʱ��	TransTime	��	ֵ��hhmmss����134050
	//ATM�豸���	DevCode	��	ֵ������12λ�豸���
	//���߳���	LockMan	��	ֵ�������Զ����������̲�ͬ������
	//���߱��	LockId	��	ֵ�������Զ�������Ψһ���
	//�����־	ActiveResult	��	ֵ��0�ɹ���1ʧ��
	//������Ϣ	ActInfo	��	ֵ�������еļ�����Ϣ��96λ���������߽��ܣ�����ATMVH���Ժ����ڼ��㿪�����룬������ɺ��ύ������
}


//���ɶ�ȡ�����뱨��
string & myAtmcMsgReadCloseCodeInfo( string & strXML ,ptree &pt )
{
	//��ȡ������ ����
	//	���״���	TransCode	��	ֵ��0004
	//	��������	TransName	��	ֵ��ReadShutLockCode
	//	��������	TransDate	��	ֵ��YYYYMMDD����20140401
	//	����ʱ��	TransTime	��	ֵ��hhmmss����134050
	//��ʼ����������
	pt.put(CCBSTR_CODE,"0004");
	pt.put("root.TransName","ReadShutLockCode");
	pt.put("root.TransDate","20140802");
	pt.put("root.TransTime","140826");
	//���и����ı�������û������ֶΣ����ǻᵼ�º������̺��Ѵ���
	pt.put("root.DevCode",ATMNO_CCBTEST);
	pt.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,jcAtmcConvertDLL::JCSTR_READ_CLOSECODE);
	std::ostringstream ss;
	write_xml(ss,pt);
	strXML=ss.str();
	assert(strXML.length()>42);	//XML��ͷ�Ĺ̶�����38���ַ����������һ����ǩ�����Լ����źϼ�4���ַ�
	return strXML;
	
	
	//���ͱ����� 

	//��ȡ������ Ӧ��
	//	���״���	TransCode	��	ֵ��0004
	//	��������	TransName	��	ֵ��ReadShutLockCode
	//	��������	TransDate	��	ֵ��YYYYMMDD����20140401
	//	����ʱ��	TransTime	��	ֵ��hhmmss����134050
	//	ATM�豸���	DevCode	��	ֵ������12λ�豸���
	//	���߳���	LockMan	��	ֵ�������Զ����������̲�ͬ������
	//	���߱��	LockId	��	ֵ�������Զ�������Ψһ���
	//	������	ShutLockcode	��	ֵ��������

}

//���ɲ����õĽ��ճ�ʼ�����뱨�ģ�ʵ���Ͻ��в��޴˱��ģ�ֻ��Ϊ���γ�һ��һ�𷽱����
string & myTestMsgRecvCloseCode( string & strXML ,ptree &pt )
{
	//��ʼ����������
	pt.put(CCBSTR_CODE,"1000");
	pt.put("root.TransName","SendShutLockCode");
	std::ostringstream ss;
	write_xml(ss,pt);
	strXML=ss.str();
	return strXML;
}

//���ɲ����õĽ�����֤�뱨�ģ�ʵ���Ͻ��в��޴˱��ģ�ֻ��Ϊ���γ�һ��һ�𷽱����
string & myTestMsgRecvVerifyCode( string & strXML ,ptree &pt )
{
	//��ʼ����������
	pt.put(CCBSTR_CODE,"1002");
	pt.put("root.TransName","SendUnLockIdent");
	std::ostringstream ss;
	write_xml(ss,pt);
	strXML=ss.str();
	return strXML;
}




}	//namespace jcAtmcMsg