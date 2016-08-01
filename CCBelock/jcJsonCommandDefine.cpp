#include "stdafx.h"
#include "zwCcbElockHdr.h"

namespace jcAtmcConvertDLL {
	//JSON命令字符串
	const char *JCSTR_CMDTITLE = "Command";
	const char *JCSTR_LOCK_ACTIVE_REQUEST = "Lock_Secret_Key";
	const char *JCSTR_LOCK_INIT = "Lock_System_Init";
	const char *JCSTR_QUERY_LOCK_STATUS = "Lock_Now_Info";
	const char *JCSTR_TIME_SYNC = "Lock_Time_Sync_ATM";
	const char *JCSTR_READ_CLOSECODE = "Lock_Close_Code_ATM";
	const char *JCSTR_SEND_INITCLOSECODE = "Lock_Close_Code_Lock";
	const char *JCSTR_SEND_UNLOCK_CERTCODE = "Lock_Open_Ident";
	const char *JCSTR_GET_LOCK_LOG = "Lock_System_Journal";
	const char *JCSTR_PUSH_WARNING = "Lock_Alarm_Info";
	const char *JCSTR_REQUEST_TIME_SYNC = "Lock_Time_Sync_Lock";
	//////////////////////////////////////////////////////////////////////////
	//20141111万敏.温度振动传感器报文支持
	const char *JCSTR_SENSE_TEMPTURE = "Set_Senser_Temperature";
	const char *JCSTR_SENSE_SHOCK = "Set_Senser_Shock";
	//20141125.万敏.温度振动传感器新增3条命令报文支持
	const char *JCSTR_SENSE_SET_INSIDE_LOOP_TIMES="Set_Inside_Loop_Times";
	const char *JCSTR_SENSE_SET_INSIDE_LOOP_PERIOD="Set_Inside_Loop_Period";
	const char *JCSTR_SENSE_SET_OUTSIDE_LOOP_PERIOD="Set_Outside_Loop_Period";
#ifdef _JINCHU_DEV1608
	//20160801.张靖钰让我增加的金储内部调试方便卸载锁具免得一旦初始化就要
	// 重新刷机的卸载命令，绝不能出现在给建行的版本里面；
	const char *JCSTR_PRV_LOCKUNINSTALL="Lock_Uninstall";
#endif // _JINCHU_DEV1608

	const char *CCBSTR_CODE = "root.TransCode";
	const char *CCBSTR_NAME = "root.TransName";
	const char *CCBSTR_DATE = "root.TransDate";
	const char *CCBSTR_TIME = "root.TransTime";
	const char *CCBSTR_DEVCODE = "root.DevCode";
}
