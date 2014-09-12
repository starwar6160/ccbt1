#include "stdafx.h"
#include "zwCcbElockHdr.h"
using boost::ifind_all;
using boost::iterator_range;
using boost::split;
using boost::is_any_of;

void zwLockStatusDataSplit(const char *LockStatus, JCLOCKSTATUS & lst)
{
	string LockStatusStr = LockStatus;
	vector < string > StatusVec;
	split(StatusVec, LockStatusStr, is_any_of(",.:-+"));	//使用标点分割
	// "0,0,0,0,100,0,1,20,100,0,0"
	//锁具已激活 | 锁具允许使用 | 锁关闭 | 门关闭 | 电池状态100%% | 震动正常 | 震动幅值1g 
	// |  | 温度探头温度100摄氏度 | 密码错误不多 | 开锁时间正常 |
	//2014/9/12 15:14:50 [星期五] 万敏  15:10:59	这个是加时间后的日志
	//{"Command":"Lock_System_Journal","Lock_Time":1409023166,"State":"0","Journal":"1409023024,0,0,1,0,100,0,00,0,000,0,0"}
	JCLOCKSTATUS ostr;
	int nIndex=0;
	lst.LogGenDate=StatusVec[nIndex];nIndex++;
	lst.ActiveStatus = StatusVec[nIndex];nIndex++;
	lst.EnableStatus = StatusVec[nIndex];nIndex++;
	lst.LockStatus = StatusVec[nIndex];nIndex++;
	lst.DoorStatus = StatusVec[nIndex];nIndex++;
	lst.BatteryStatus = StatusVec[nIndex];nIndex++;
	lst.ShockAlert = StatusVec[nIndex];nIndex++;
	lst.ShockValue = StatusVec[nIndex];nIndex++;
	lst.TempAlert = StatusVec[nIndex];nIndex++;	//这里值是20，但是合法值只有0,1,2,3，为什么？
	lst.nodeTemp = StatusVec[nIndex];nIndex++;	//探头温度100摄氏度？
	lst.PswTryAlert = StatusVec[nIndex];nIndex++;
	lst.LockOverTime = StatusVec[nIndex];
}

void zwStatusData2String(const JCLOCKSTATUS & lst, JCLOCKSTATUS & ostr)
{
	time_t logGenData=boost::lexical_cast<time_t>(lst.LogGenDate);
	string exDate, exTime;
	zwGetLocalDateTimeString(logGenData, exDate, exTime);
	ostr.LogGenDate=exDate+"."+exTime;

//////////////////////////////////////////////////////////////////////////
	if ("0" == lst.ActiveStatus) {
		ostr.ActiveStatus = "锁具已激活";
	}
	if ("1" == lst.ActiveStatus) {
		ostr.ActiveStatus = "锁具未激活";
	}
	//////////////////////////////////////////////////////////////////////////
	if ("0" == lst.EnableStatus) {
		ostr.EnableStatus = "锁具允许使用";
	}
	if ("1" == lst.EnableStatus) {
		ostr.EnableStatus = "锁具禁止使用";
	}
	//////////////////////////////////////////////////////////////////////////
	if ("0" == lst.LockStatus) {
		ostr.LockStatus = "锁关闭";
	}
	if ("1" == lst.LockStatus) {
		ostr.LockStatus = "锁开启";
	}
	//////////////////////////////////////////////////////////////////////////
	if ("0" == lst.DoorStatus) {
		ostr.DoorStatus = "门关闭";
	}
	if ("1" == lst.DoorStatus) {
		ostr.DoorStatus = "门开启";
	}
	if ("2" == lst.DoorStatus) {
		ostr.DoorStatus = "门状态无法检测";
	}
	if ("3" == lst.DoorStatus) {
		ostr.DoorStatus = "门检测器";
	}
	//////////////////////////////////////////////////////////////////////////
	ostr.BatteryStatus = "电池状态" + lst.BatteryStatus + "%";
	//////////////////////////////////////////////////////////////////////////
	if ("0" == lst.ShockAlert) {
		ostr.ShockAlert = "震动正常";
	}
	if ("1" == lst.ShockAlert) {
		ostr.ShockAlert = "震动报警";
	}
	if ("2" == lst.ShockAlert) {
		ostr.ShockAlert = "震动无法检测";
	}
	if ("3" == lst.ShockAlert) {
		ostr.ShockAlert = "震动传感器";
	}
	//////////////////////////////////////////////////////////////////////////
	ostr.ShockValue = "震动幅值" + lst.ShockValue + "g";
	//////////////////////////////////////////////////////////////////////////
	if ("0" == lst.TempAlert) {
		ostr.TempAlert = "温度正常";
	}
	if ("1" == lst.TempAlert) {
		ostr.TempAlert = "高温报警";
	}
	if ("2" == lst.TempAlert) {
		ostr.TempAlert = "温度无法检测";
	}
	if ("3" == lst.TempAlert) {
		ostr.TempAlert = "温度传感器";
	}
	//////////////////////////////////////////////////////////////////////////
	ostr.nodeTemp = "温度探头温度" + lst.nodeTemp + "摄氏度";
	//////////////////////////////////////////////////////////////////////////
	if ("0" == lst.PswTryAlert) {
		ostr.PswTryAlert = "密码错误没有或者不多";
	}
	if ("1" == lst.PswTryAlert) {
		ostr.PswTryAlert = "密码错误过多";
	}
	//////////////////////////////////////////////////////////////////////////
	if ("0" == lst.LockOverTime) {
		ostr.LockOverTime = "开锁时间正常";
	}
	if ("1" == lst.LockOverTime) {
		ostr.LockOverTime = "开锁时间超时";
	}
}				//end of void zwStatusData2String

string LockStatusStringMerge(JCLOCKSTATUS & ostr)
{
	string sep = " | ";
	//20140912.1552.应万敏的要求，添加了日志生成的时间，但是为了防止新增字段造成
	//建行后台分割处理这些字符串出问题，暂时采用将时间与激活状态两者拼接在一起的方法
	string ccbStatusStr = ostr.LogGenDate+":"
		+ostr.ActiveStatus + sep + ostr.EnableStatus + sep
	    + ostr.LockStatus + sep + ostr.DoorStatus + sep
	    + ostr.BatteryStatus + sep
	    + ostr.ShockAlert + sep + ostr.ShockValue + sep
	    + ostr.TempAlert + sep + ostr.nodeTemp + sep
	    + ostr.PswTryAlert + sep + ostr.LockOverTime + sep;
	return ccbStatusStr;
}
