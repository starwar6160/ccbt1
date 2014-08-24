#include "stdafx.h"
#include "zwCcbElockHdr.h"
#include "eLockTest\\jcElockTestHdr.h"

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


void ZWTRACE(const char *x)
{
	OutputDebugStringA(x);
	//cout<<x<<endl;	
}

namespace jcAtmcMsg{



//���ɲ����õĽ�����֤�뱨�ģ�ʵ���Ͻ��в��޴˱��ģ�ֻ��Ϊ���γ�һ��һ�𷽱����
string & myTestMsgRecvVerifyCode( string & strXML ,ptree &pt )
{
	//��ʼ����������
	pt.put(CCBSTR_CODE,"1002");
	pt.put(CCBSTR_NAME,"SendUnLockIdent");
	std::ostringstream ss;
	write_xml(ss,pt);
	strXML=ss.str();
	return strXML;
}

//���ɲ����õ�0002��ѯ����״̬
string & myTestMsgCheckLockStatus( string & strXML ,ptree &pt )
{
	//��ʼ����������
	pt.put(CCBSTR_CODE,"0002");
	pt.put(CCBSTR_NAME,"QueryForLockStatus");
	string zwDate,zwTime;
	zwGetLocalDateTimeString(time(NULL),zwDate,zwTime);
	pt.put(CCBSTR_DATE,zwDate);
	pt.put(CCBSTR_TIME,zwTime);
	pt.put(CCBSTR_DEVCODE,ATMNO_CCBTEST);
	pt.put("root.LockMan",jcAtmcMsg::G_LOCKMAN_NAMEG);
	pt.put("root.LockId","ZWFAKELOCKNO1548");
	std::ostringstream ss;
	write_xml(ss,pt);
	strXML=ss.str();
	return strXML;
}

//���ɲ����õ�0003ʱ��ͬ�����ģ�Ŀ������ģ�⽨��ATM��������Ϊ
string & myTestMsgTimeSync( string & strXML ,ptree &pt )
{
	//��ʼ����������
	pt.put(CCBSTR_CODE,"0003");
	pt.put(CCBSTR_NAME,"TimeSync");
	string zwDate,zwTime;
	zwGetLocalDateTimeString(time(NULL),zwDate,zwTime);
	pt.put(CCBSTR_DATE,zwDate);
	pt.put(CCBSTR_TIME,zwTime);
	std::ostringstream ss;
	write_xml(ss,pt);
	strXML=ss.str();
	return strXML;
}



//���ɲ����õ�0005��ȡ��־���ģ�Ŀ������ģ�⽨��ATM��������Ϊ
string & myTestMsgGetLockLog( string & strXML ,ptree &pt )
{
	//��ʼ����������
	pt.put(CCBSTR_CODE,"0005");
	pt.put(CCBSTR_NAME,"ReadLog");
	string zwDate,zwTime;
	zwGetLocalDateTimeString(time(NULL),zwDate,zwTime);
	pt.put(CCBSTR_DATE,zwDate);
	pt.put(CCBSTR_TIME,zwTime);
	pt.put("root.BeginNo",0);
	pt.put("root.EndNo",23);
	std::ostringstream ss;
	write_xml(ss,pt);
	strXML=ss.str();
	return strXML;
}



}	//namespace jcAtmcMsg