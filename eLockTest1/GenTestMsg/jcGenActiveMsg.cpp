#include "stdafx.h"
#include "..\\eLockTest\\jcElockTestHdr.h"

namespace jcAtmcMsg{
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
		pt.put(CCBSTR_NAME,"CallForActInfo");
		pt.put(CCBSTR_DATE,"20140730");
		pt.put(CCBSTR_TIME,"142248");
		pt.put(CCBSTR_DEVCODE,ATMNO_CCBTEST);
		//pt.put(CCBSTR_DEVCODE,"12345678");

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
		pt.put(CCBSTR_NAME,"SendActInfo");
		string zwDate,zwTime;
		zwGetLocalDateTimeString(time(NULL),zwDate,zwTime);

		pt.put(CCBSTR_DATE,zwDate);
		pt.put(CCBSTR_TIME,zwTime);
		pt.put(CCBSTR_DEVCODE,ATMNO_CCBTEST);
		pt.put("root.LockMan",jcAtmcMsg::G_LOCKMAN_NAMEG);
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




}	//namespace jcAtmcMsg{