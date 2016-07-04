#include "stdafx.h"
#include "..\\eLockTest\\jcElockTestHdr.h"
//#include "zwecies529.h"


extern string s_repActReqXML;	//�������յ��ļ�������ķ��ر���
//��0�ű��ķ���XML��ȡ��Կ������
string myGetPubKeyFromMsg0000Rep(const string msg0000RepXML);

namespace jcAtmcMsg {
	//�������߼���XML���ģ���ȡ���߱�ź͹�Կ
	string & myAtmcMsgLockActive(string & strXML, ptree & pt) {
		//����
		//      ���״���        TransCode       ��      ֵ��0000
		//      ��������        TransName       ��      ֵ��CallForActInfo
		//      ��������        TransDate       ��      ֵ��YYYYMMDD����20140401
		//      ����ʱ��        TransTime       ��      ֵ��hhmmss����134050
		//      ATM�豸���     DevCode ��      ֵ������12λ�豸���
		//��ʼ����������
		pt.put(CCBSTR_CODE, "0000");
		pt.put(CCBSTR_NAME, "CallForActInfo");
		string zwDate, zwTime;
		zwGetLocalDateTimeString(time(NULL),zwDate, zwTime);
		pt.put(CCBSTR_DATE, zwDate);
		pt.put(CCBSTR_TIME, zwTime);
		//�𴢴˱��ĸ���û��ATMNO�ֶΣ�����д����ֵҲ�������κ�Ӱ��
		pt.put(CCBSTR_DEVCODE, ATMNO_CCBTEST);	

		std::ostringstream ss;
		write_xml(ss, pt);
		strXML = ss.str();
		assert(strXML.length() > 42);	//XML��ͷ�Ĺ̶�����38���ַ����������һ����ǩ�����Լ����źϼ�4���ַ�
		return strXML;
		//Ӧ��
		//      ���״���        TransCode       ��      ֵ��0000
		//      ��������        TransName       ��      ֵ��CallForActInfo
		//      ��������        TransDate       ��      ֵ��YYYYMMDD����20140401
		//      ����ʱ��        TransTime       ��      ֵ��hhmmss����134050
		//      ATM�豸���     DevCode ��      ֵ������12λ�豸���
		//      ���߳���        LockMan ��      ֵ�������Զ����������̲�ͬ������
		//      ���߱��        LockId  ��      ֵ�������Զ�������Ψһ���
		//      ���߹�Կ        LockPubKey      ��      ֵ��ÿ���������ɵĹ�˽Կ���еĹ�Կ 16���ַ�
	}

//20150326��Ϊ��Ѹ�ٵ���Ҧ�������USB�ε���֮���ܳɹ�Open��������ʱ���ε��ⲿ�ִ���
//��������Ϊecies�㷨��zwBaseLib������ȡ��������jclms DLL�ŵ��µģ�
#ifdef _DEBUG326
	//���ɷ������߼�����Ϣ���ģ����������߹�Կ���ܹ����PSK������
	    string & myAtmcMsgSendActiveInfo(string & strXML, ptree & pt) {

		//����
		//      ���״���        TransCode       ��      ֵ��0001
		//      ��������        TransName       ��      ֵ��SendActInfo
		//      ��������        TransDate       ��      ֵ��YYYYMMDD����20140401
		//      ����ʱ��        TransTime       ��      ֵ��hhmmss����134050
		//      ATM�豸���     DevCode ��      ֵ������12λ�豸���
		//      ���߳���        LockMan ��      ֵ�������Զ����������̲�ͬ������
		//      ���߱��        LockId  ��      ֵ�������Զ�������Ψһ���
		//      ������Ϣ        ActInfo ��      ����ͼ��ܷ�������ɢ�������������߹�Կ���ܵļ�����Ϣ ����96���ַ�
		//��ʼ����������
		pt.put(CCBSTR_CODE, "0001");
		pt.put(CCBSTR_NAME, "SendActInfo");
		string zwDate, zwTime;
		zwGetLocalDateTimeString(time(NULL), zwDate, zwTime);
		pt.put(CCBSTR_DATE, zwDate);
		pt.put(CCBSTR_TIME, zwTime);
		pt.put(CCBSTR_DEVCODE, ATMNO_CCBTEST);
		pt.put("root.LockMan", jcAtmcMsg::G_LOCKMAN_NAMEG);
		pt.put("root.LockId", "ZWFAKELOCKNO1548");

		string pubKeyFromLock=myGetPubKeyFromMsg0000Rep(s_repActReqXML);
		assert(pubKeyFromLock.length()>0);
		string pskinput="zhouwei20140912.1014FAKEPSK"+zwDate+zwTime;
		string psk=zwMergePsk(pskinput.c_str());
		string actInfo=EciesEncrypt(pubKeyFromLock.c_str(),psk.c_str());
		assert(actInfo.length()>0);
		cout<<"PSK912 FROM ATMC IS "<<psk<<endl;
		cout<<"ACTINFO912\n"<<actInfo<<endl;
		pt.put("root.ActInfo",actInfo);

		std::ostringstream ss;
		write_xml(ss, pt);
		strXML = ss.str();
		assert(strXML.length() > 42);	//XML��ͷ�Ĺ̶�����38���ַ����������һ����ǩ�����Լ����źϼ�4���ַ�
		return strXML;

		//Ӧ��
		//���״���      TransCode       ��      ֵ��0001
		//��������      TransName       ��      ֵ��SendActInfo
		//��������      TransDate       ��      ֵ��YYYYMMDD����20140401
		//����ʱ��      TransTime       ��      ֵ��hhmmss����134050
		//ATM�豸���   DevCode ��      ֵ������12λ�豸���
		//���߳���      LockMan ��      ֵ�������Զ����������̲�ͬ������
		//���߱��      LockId  ��      ֵ�������Զ�������Ψһ���
		//�����־      ActiveResult    ��      ֵ��0�ɹ���1ʧ��
		//������Ϣ      ActInfo ��      ֵ�������еļ�����Ϣ��96λ���������߽��ܣ�����ATMVH���Ժ����ڼ��㿪�����룬������ɺ��ύ������
	}
#endif // _DEBUG326

}				//namespace jcAtmcMsg{
