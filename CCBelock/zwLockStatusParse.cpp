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
	split(StatusVec, LockStatusStr, is_any_of(",.:-+")); //使用标点分割
	// "0,0,0,0,100,0,1,20,100,0,0"
	//锁具已激活 | 锁具允许使用 | 锁关闭 | 门关闭 | 电池状态100%% | 震动正常 | 震动幅值1g 
	// |  | 温度探头温度100摄氏度 | 密码错误不多 | 开锁时间正常 |
	JCLOCKSTATUS ostr;
	lst.ActiveStatus=StatusVec[0];
	lst.EnableStatus=StatusVec[1];
	lst.LockStatus=StatusVec[2];
	lst.DoorStatus=StatusVec[3];
	lst.BatteryStatus=StatusVec[4];
	lst.ShockAlert=StatusVec[5];
	lst.ShockValue=StatusVec[6];
	lst.TempAlert=StatusVec[7];	//这里值是20，但是合法值只有0,1,2,3，为什么？
	lst.nodeTemp=StatusVec[8];	//探头温度100摄氏度？
	lst.PswTryAlert=StatusVec[9];
	lst.LockOverTime=StatusVec[10];
}

void zwStatusData2String(const JCLOCKSTATUS &lst,JCLOCKSTATUS &ostr)
{
//////////////////////////////////////////////////////////////////////////
	if ("0"==lst.ActiveStatus)
	{
		ostr.ActiveStatus="锁具已激活";
	}
	if ("1"==lst.ActiveStatus)
	{
		ostr.ActiveStatus="锁具未激活";
	}
	//////////////////////////////////////////////////////////////////////////
	if ("0"==lst.EnableStatus)
	{
		ostr.EnableStatus="锁具允许使用";
	}
	if ("1"==lst.EnableStatus)
	{
		ostr.EnableStatus="锁具禁止使用";
	}
	//////////////////////////////////////////////////////////////////////////
	if ("0"==lst.LockStatus)
	{
		ostr.LockStatus="锁关闭";
	}
	if ("1"==lst.LockStatus)
	{
		ostr.LockStatus="锁开启";
	}
	//////////////////////////////////////////////////////////////////////////
	if ("0"==lst.DoorStatus)
	{
		ostr.DoorStatus="门关闭";
	}
	if ("1"==lst.DoorStatus)
	{
		ostr.DoorStatus="门开启";
	}
	if ("2"==lst.DoorStatus)
	{
		ostr.DoorStatus="门状态无法检测";
	}
	if ("3"==lst.DoorStatus)
	{
		ostr.DoorStatus="门检测器";
	}
	//////////////////////////////////////////////////////////////////////////
	ostr.BatteryStatus="电池状态"+lst.BatteryStatus+"%";
	//////////////////////////////////////////////////////////////////////////
	if ("0"==lst.ShockAlert)
	{
		ostr.ShockAlert="震动正常";
	}
	if ("1"==lst.ShockAlert)
	{
		ostr.ShockAlert="震动报警";
	}
	if ("2"==lst.ShockAlert)
	{
		ostr.ShockAlert="震动无法检测";
	}
	if ("3"==lst.ShockAlert)
	{
		ostr.ShockAlert="震动传感器";
	}
	//////////////////////////////////////////////////////////////////////////
	ostr.ShockValue="震动幅值"+lst.ShockValue+"g";
	//////////////////////////////////////////////////////////////////////////
	if ("0"==lst.TempAlert)
	{
		ostr.TempAlert="温度正常";
	}
	if ("1"==lst.TempAlert)
	{
		ostr.TempAlert="高温报警";
	}
	if ("2"==lst.TempAlert)
	{
		ostr.TempAlert="温度无法检测";
	}
	if ("3"==lst.TempAlert)
	{
		ostr.TempAlert="温度传感器";
	}
	//////////////////////////////////////////////////////////////////////////
	ostr.nodeTemp="温度探头温度"+lst.nodeTemp+"摄氏度";
	//////////////////////////////////////////////////////////////////////////
	if ("0"==lst.PswTryAlert)
	{
		ostr.PswTryAlert="密码错误没有或者不多";
	}
	if ("1"==lst.PswTryAlert)
	{
		ostr.PswTryAlert="密码错误过多";
	}
	//////////////////////////////////////////////////////////////////////////
	if ("0"==lst.LockOverTime)
	{
		ostr.LockOverTime="开锁时间正常";
	}
	if ("1"==lst.LockOverTime)
	{
		ostr.LockOverTime="开锁时间超时";
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
