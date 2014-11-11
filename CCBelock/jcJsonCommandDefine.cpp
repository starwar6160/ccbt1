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
	const char *JCSTR_SENSE_TEMPTURE="Set_Senser_Temperature";
	const char *JCSTR_SENSE_SHOCK="Set_Senser_Shock";

	const char *CCBSTR_CODE = "root.TransCode";
	const char *CCBSTR_NAME = "root.TransName";
	const char *CCBSTR_DATE = "root.TransDate";
	const char *CCBSTR_TIME = "root.TransTime";
	const char *CCBSTR_DEVCODE = "root.DevCode";
}

