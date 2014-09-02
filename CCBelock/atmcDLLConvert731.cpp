#include "stdafx.h"
#include "zwCcbElockHdr.h"
#include "zwPocoLog.h"
#include "ATMCMsgConvert\\myConvIntHdr.h"

//把ATMC DLL的XML和JSON互转函数集中于此，便于单元测试；
namespace jcAtmcConvertDLL {
	const char *LOCKMAN_NAME = "BeiJing.JinChu";
	string ns_ActReqName;
	string ns_LockInitName;
	string ns_ReadCloseCodeName;
	string ns_ccbAtmno;	//ATM编号

}				//namespace jcAtmcConvertDLL
