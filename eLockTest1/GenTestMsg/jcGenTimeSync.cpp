#include "stdafx.h"
#include "..\\eLockTest\\jcElockTestHdr.h"

namespace jcAtmcMsg{

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

}	//namespace jcAtmcMsg{