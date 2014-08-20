#include "stdafx.h"
#include "zwCcbElockHdr.h"
using boost::ifind_all;
using boost::iterator_range;
using boost::split;
using boost::is_any_of;



void zwLockStatusDataSplit(const char *LockStatus,JCLOCKSTATUS &lst)
{
	string LockStatusStr=LockStatus;
	vector<string>StatusVec;
	split(StatusVec, LockStatusStr, is_any_of(",.:-+")); //ʹ�ñ��ָ�
	// "0,0,0,0,100,0,1,20,100,0,0"
	//�����Ѽ��� | ��������ʹ�� | ���ر� | �Źر� | ���״̬100%% | ������ | �𶯷�ֵ1g 
	// |  | �¶�̽ͷ�¶�100���϶� | ������󲻶� | ����ʱ������ |
	JCLOCKSTATUS ostr;
	lst.ActiveStatus=StatusVec[0];
	lst.EnableStatus=StatusVec[1];
	lst.LockStatus=StatusVec[2];
	lst.DoorStatus=StatusVec[3];
	lst.BatteryStatus=StatusVec[4];
	lst.ShockAlert=StatusVec[5];
	lst.ShockValue=StatusVec[6];
	lst.TempAlert=StatusVec[7];	//����ֵ��20�����ǺϷ�ֵֻ��0,1,2,3��Ϊʲô��
	lst.nodeTemp=StatusVec[8];	//̽ͷ�¶�100���϶ȣ�
	lst.PswTryAlert=StatusVec[9];
	lst.LockOverTime=StatusVec[10];
}

void zwStatusData2String(const JCLOCKSTATUS &lst,JCLOCKSTATUS &ostr)
{
//////////////////////////////////////////////////////////////////////////
	if ("0"==lst.ActiveStatus)
	{
		ostr.ActiveStatus="�����Ѽ���";
	}
	if ("1"==lst.ActiveStatus)
	{
		ostr.ActiveStatus="����δ����";
	}
	//////////////////////////////////////////////////////////////////////////
	if ("0"==lst.EnableStatus)
	{
		ostr.EnableStatus="��������ʹ��";
	}
	if ("1"==lst.EnableStatus)
	{
		ostr.EnableStatus="���߽�ֹʹ��";
	}
	//////////////////////////////////////////////////////////////////////////
	if ("0"==lst.LockStatus)
	{
		ostr.LockStatus="���ر�";
	}
	if ("1"==lst.LockStatus)
	{
		ostr.LockStatus="������";
	}
	//////////////////////////////////////////////////////////////////////////
	if ("0"==lst.DoorStatus)
	{
		ostr.DoorStatus="�Źر�";
	}
	if ("1"==lst.DoorStatus)
	{
		ostr.DoorStatus="�ſ���";
	}
	if ("2"==lst.DoorStatus)
	{
		ostr.DoorStatus="��״̬�޷����";
	}
	if ("3"==lst.DoorStatus)
	{
		ostr.DoorStatus="�ż����";
	}
	//////////////////////////////////////////////////////////////////////////
	ostr.BatteryStatus="���״̬"+lst.BatteryStatus+"%";
	//////////////////////////////////////////////////////////////////////////
	if ("0"==lst.ShockAlert)
	{
		ostr.ShockAlert="������";
	}
	if ("1"==lst.ShockAlert)
	{
		ostr.ShockAlert="�𶯱���";
	}
	if ("2"==lst.ShockAlert)
	{
		ostr.ShockAlert="���޷����";
	}
	if ("3"==lst.ShockAlert)
	{
		ostr.ShockAlert="�𶯴�����";
	}
	//////////////////////////////////////////////////////////////////////////
	ostr.ShockValue="�𶯷�ֵ"+lst.ShockValue+"g";
	//////////////////////////////////////////////////////////////////////////
	if ("0"==lst.TempAlert)
	{
		ostr.TempAlert="�¶�����";
	}
	if ("1"==lst.TempAlert)
	{
		ostr.TempAlert="���±���";
	}
	if ("2"==lst.TempAlert)
	{
		ostr.TempAlert="�¶��޷����";
	}
	if ("3"==lst.TempAlert)
	{
		ostr.TempAlert="�¶ȴ�����";
	}
	//////////////////////////////////////////////////////////////////////////
	ostr.nodeTemp="�¶�̽ͷ�¶�"+lst.nodeTemp+"���϶�";
	//////////////////////////////////////////////////////////////////////////
	if ("0"==lst.PswTryAlert)
	{
		ostr.PswTryAlert="�������û�л��߲���";
	}
	if ("1"==lst.PswTryAlert)
	{
		ostr.PswTryAlert="����������";
	}
	//////////////////////////////////////////////////////////////////////////
	if ("0"==lst.LockOverTime)
	{
		ostr.LockOverTime="����ʱ������";
	}
	if ("1"==lst.LockOverTime)
	{
		ostr.LockOverTime="����ʱ�䳬ʱ";
	}
}	//end of void zwStatusData2String

string LockStatusStringMerge(JCLOCKSTATUS &ostr)
{
	string sep=" | ";
	string ccbStatusStr=ostr.ActiveStatus+sep+ostr.EnableStatus+sep
		+ostr.LockStatus+sep+ostr.DoorStatus+sep
		+ostr.BatteryStatus+sep
		+ostr.ShockAlert+sep+ostr.ShockValue+sep
		+ostr.TempAlert+sep+ostr.nodeTemp+sep
		+ostr.PswTryAlert+sep+ostr.LockOverTime+sep;
	return ccbStatusStr;
}
