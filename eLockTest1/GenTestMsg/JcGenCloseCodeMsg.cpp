#include "stdafx.h"
#include "..\\eLockTest\\jcElockTestHdr.h"

namespace jcAtmcMsg {
//���ɶ�ȡ�����뱨��
	string & myAtmcMsgReadCloseCodeInfo(string & strXML, ptree & pt) {
		//��ȡ������ ����
		//      ���״���        TransCode       ��      ֵ��0004
		//      ��������        TransName       ��      ֵ��ReadShutLockCode
		//      ��������        TransDate       ��      ֵ��YYYYMMDD����20140401
		//      ����ʱ��        TransTime       ��      ֵ��hhmmss����134050
		//��ʼ����������
		pt.put(CCBSTR_CODE, "0004");
		pt.put(CCBSTR_NAME, "ReadShutLockCode");
		string zwDate, zwTime;
		zwGetLocalDateTimeString(time(NULL),zwDate, zwTime);
		pt.put(CCBSTR_DATE, zwDate);
		pt.put(CCBSTR_TIME, zwTime);
		//���и����ı�������û������ֶΣ����ǻᵼ�º������̺��Ѵ���
		pt.put(CCBSTR_DEVCODE, ATMNO_CCBTEST);
		pt.put(jcAtmcConvertDLL::JCSTR_CMDTITLE,
		       jcAtmcConvertDLL::JCSTR_READ_CLOSECODE);
		std::ostringstream ss;
		write_xml(ss, pt);
		strXML = ss.str();
		assert(strXML.length() > 42);	//XML��ͷ�Ĺ̶�����38���ַ����������һ����ǩ�����Լ����źϼ�4���ַ�
		return strXML;

		//���ͱ����� 

		//��ȡ������ Ӧ��
		//      ���״���        TransCode       ��      ֵ��0004
		//      ��������        TransName       ��      ֵ��ReadShutLockCode
		//      ��������        TransDate       ��      ֵ��YYYYMMDD����20140401
		//      ����ʱ��        TransTime       ��      ֵ��hhmmss����134050
		//      ATM�豸���     DevCode ��      ֵ������12λ�豸���
		//      ���߳���        LockMan ��      ֵ�������Զ����������̲�ͬ������
		//      ���߱��        LockId  ��      ֵ�������Զ�������Ψһ���
		//      ������  ShutLockcode    ��      ֵ��������

	}
//���ɲ����õĽ��ճ�ʼ�����뱨�ģ�ʵ���Ͻ��в��޴˱��ģ�ֻ��Ϊ���γ�һ��һ�𷽱����
	    string & myTestMsgRecvCloseCode(string & strXML, ptree & pt) {
		//��ʼ����������
		pt.put(CCBSTR_CODE, "1000");
		pt.put(CCBSTR_NAME, "SendShutLockCode");
		std::ostringstream ss;
		write_xml(ss, pt);
		strXML = ss.str();
		return strXML;
	}
}				//namespace jcAtmcMsg{
