#include "stdafx.h"
#include "zwCcbElockHdr.h"
using boost::ifind_all;
using boost::iterator_range;
using boost::split;
using boost::is_any_of;

void zwLockStatusDataSplit(const char *LockStatus, JCLOCKSTATUS & lst)
{
	assert(NULL!=LockStatus);
	assert(strlen(LockStatus)>0);
	if (NULL==LockStatus || strlen(LockStatus)==0)
	{
		return;
	}
	string LockStatusStr = LockStatus;
	vector < string > StatusVec;
	split(StatusVec, LockStatusStr, is_any_of(",.:-+"));	//ʹ�ñ��ָ�
	// "0,0,0,0,100,0,1,20,100,0,0"
	//�����Ѽ��� | ��������ʹ�� | ���ر� | �Źر� | ���״̬100%% | ������ | �𶯷�ֵ1g 
	// |  | �¶�̽ͷ�¶�100���϶� | ������󲻶� | ����ʱ������ |
	//2014/9/12 15:14:50 [������] ����  15:10:59	����Ǽ�ʱ������־
	//{"Command":"Lock_System_Journal","Lock_Time":1409023166,"State":"0","Journal":"1409023024,0,0,1,0,100,0,00,0,000,0,0"}
	JCLOCKSTATUS ostr;
	int nIndex=0;
	int vecSize=StatusVec.size();
	if(vecSize==0)return;
	lst.LogGenDate=StatusVec[nIndex];nIndex++;
	if (nIndex>=vecSize) return;	//Ϊ��Ԥ����λ�����ص�������Ŀ�������ʱ��ʩ
	lst.ActiveStatus = StatusVec[nIndex];nIndex++;
	if (nIndex>=vecSize) return;	//Ϊ��Ԥ����λ�����ص�������Ŀ�������ʱ��ʩ
	lst.EnableStatus = StatusVec[nIndex];nIndex++;
	if (nIndex>=vecSize) return;	//Ϊ��Ԥ����λ�����ص�������Ŀ�������ʱ��ʩ
	lst.LockStatus = StatusVec[nIndex];nIndex++;
	if (nIndex>=vecSize) return;	//Ϊ��Ԥ����λ�����ص�������Ŀ�������ʱ��ʩ
	lst.DoorStatus = StatusVec[nIndex];nIndex++;
	if (nIndex>=vecSize) return;	//Ϊ��Ԥ����λ�����ص�������Ŀ�������ʱ��ʩ
	lst.BatteryStatus = StatusVec[nIndex];nIndex++;
	if (nIndex>=vecSize) return;	//Ϊ��Ԥ����λ�����ص�������Ŀ�������ʱ��ʩ
	lst.ShockAlert = StatusVec[nIndex];nIndex++;
	if (nIndex>=vecSize) return;	//Ϊ��Ԥ����λ�����ص�������Ŀ�������ʱ��ʩ
	lst.ShockValue = StatusVec[nIndex];nIndex++;
	//20141011.1438.����4���жϣ�Ԥ����λ�����ص�������Ŀ����Ӧ��Ҳ�����4��ٶ�Ļ���Ȼ�����
	if (nIndex>=vecSize) return;	//Ϊ��Ԥ����λ�����ص�������Ŀ�������ʱ��ʩ
	lst.TempAlert = StatusVec[nIndex];nIndex++;	//����ֵ��20�����ǺϷ�ֵֻ��0,1,2,3��Ϊʲô��
	if (nIndex>=vecSize) return;	//Ϊ��Ԥ����λ�����ص�������Ŀ�������ʱ��ʩ
	lst.nodeTemp = StatusVec[nIndex];nIndex++;	//̽ͷ�¶�100���϶ȣ�
	if (nIndex>=vecSize) return;	//Ϊ��Ԥ����λ�����ص�������Ŀ�������ʱ��ʩ
	lst.PswTryAlert = StatusVec[nIndex];nIndex++;
	//20141011.�ṹ����12����Ŀ��������λ�����ص�ֻ��11�����ݣ���Ҫ�����޸���
	if (nIndex>=vecSize) return;	//Ϊ��Ԥ����λ�����ص�������Ŀ�������ʱ��ʩ
	lst.LockOverTime = StatusVec[nIndex];
}

void zwStatusData2String(const JCLOCKSTATUS & lst, JCLOCKSTATUS & ostr)
{
try{
	time_t logGenData=boost::lexical_cast<time_t>(lst.LogGenDate);
	string exDate, exTime;
	zwGetLocalDateTimeString(logGenData, exDate, exTime);
	ostr.LogGenDate=exDate+"."+exTime;
	}
	catch(...)
	{
		cout<<__FUNCTION__<<"error!20141011"<<endl;
		return;
	}
//////////////////////////////////////////////////////////////////////////
	if ("0" == lst.ActiveStatus) {
		ostr.ActiveStatus = "�����Ѽ���";
	}
	if ("1" == lst.ActiveStatus) {
		ostr.ActiveStatus = "����δ����";
	}
	//////////////////////////////////////////////////////////////////////////
	if ("0" == lst.EnableStatus) {
		ostr.EnableStatus = "��������ʹ��";
	}
	if ("1" == lst.EnableStatus) {
		ostr.EnableStatus = "���߽�ֹʹ��";
	}
	//////////////////////////////////////////////////////////////////////////
	if ("0" == lst.LockStatus) {
		ostr.LockStatus = "���ر�";
	}
	if ("1" == lst.LockStatus) {
		ostr.LockStatus = "������";
	}
	//////////////////////////////////////////////////////////////////////////
	if ("0" == lst.DoorStatus) {
		ostr.DoorStatus = "�Źر�";
	}
	if ("1" == lst.DoorStatus) {
		ostr.DoorStatus = "�ſ���";
	}
	if ("2" == lst.DoorStatus) {
		ostr.DoorStatus = "��״̬�޷����";
	}
	if ("3" == lst.DoorStatus) {
		ostr.DoorStatus = "�ż����";
	}
	//////////////////////////////////////////////////////////////////////////
	ostr.BatteryStatus = "���״̬" + lst.BatteryStatus + "%";
	//////////////////////////////////////////////////////////////////////////
	if ("0" == lst.ShockAlert) {
		ostr.ShockAlert = "������";
	}
	if ("1" == lst.ShockAlert) {
		ostr.ShockAlert = "�𶯱���";
	}
	if ("2" == lst.ShockAlert) {
		ostr.ShockAlert = "���޷����";
	}
	if ("3" == lst.ShockAlert) {
		ostr.ShockAlert = "�𶯴�����";
	}
	//////////////////////////////////////////////////////////////////////////
	ostr.ShockValue = "�𶯷�ֵ" + lst.ShockValue + "g";
	//////////////////////////////////////////////////////////////////////////
	if ("0" == lst.TempAlert) {
		ostr.TempAlert = "�¶�����";
	}
	if ("1" == lst.TempAlert) {
		ostr.TempAlert = "���±���";
	}
	if ("2" == lst.TempAlert) {
		ostr.TempAlert = "�¶��޷����";
	}
	if ("3" == lst.TempAlert) {
		ostr.TempAlert = "�¶ȴ�����";
	}
	//////////////////////////////////////////////////////////////////////////
	ostr.nodeTemp = "�¶�̽ͷ�¶�" + lst.nodeTemp + "���϶�";
	//////////////////////////////////////////////////////////////////////////
	if ("0" == lst.PswTryAlert) {
		ostr.PswTryAlert = "�������û�л��߲���";
	}
	if ("1" == lst.PswTryAlert) {
		ostr.PswTryAlert = "����������";
	}
	//////////////////////////////////////////////////////////////////////////
	if ("0" == lst.LockOverTime) {
		ostr.LockOverTime = "����ʱ������";
	}
	if ("1" == lst.LockOverTime) {
		ostr.LockOverTime = "����ʱ�䳬ʱ";
	}
}				//end of void zwStatusData2String

string LockStatusStringMerge(JCLOCKSTATUS & ostr)
{
	string sep = " | ";
	//20140912.1552.Ӧ������Ҫ���������־���ɵ�ʱ�䣬����Ϊ�˷�ֹ�����ֶ����
	//���к�̨�ָ����Щ�ַ��������⣬��ʱ���ý�ʱ���뼤��״̬����ƴ����һ��ķ���
	string ccbStatusStr = ostr.LogGenDate+":"
		+ostr.ActiveStatus + sep + ostr.EnableStatus + sep
	    + ostr.LockStatus + sep + ostr.DoorStatus + sep
	    + ostr.BatteryStatus + sep
	    + ostr.ShockAlert + sep + ostr.ShockValue + sep
	    + ostr.TempAlert + sep + ostr.nodeTemp + sep
	    + ostr.PswTryAlert + sep + ostr.LockOverTime + sep;
	return ccbStatusStr;
}
