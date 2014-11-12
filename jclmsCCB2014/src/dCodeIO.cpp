#include "stdafx.h"
#include "jclmsCCB2014.h"
#include "dCodeHdr.h"

const int ZW_MAXDATA32 = 2048 * ZWMEGA - 3;	//32位有符号整数可能表示的最大时间值
const int ZW_LOWEST_DATE = 1400 * ZWMEGA - 24 * 3600;	//考虑到取整运算可能使得时间值低于1400M，所以把最低点时间提前一整天该足够了
const int ZW_ONE_DAY = 24 * 60 * 60;

//设置命令类型(第一开锁码，初始闭锁码等等)
JCERROR JCLMSCCB2014_API JcLockSetCmdType(const int handle, const JCITYPE mtype,
					  const JCCMD cmd)
{
	assert(handle > 0);
	assert(mtype > JCI_START && mtype < JCI_END);
	assert(cmd > JCCMD_START && cmd < JCCMD_END);
	if (handle <= 0 || mtype <= JCI_START || mtype >= JCI_END
	    || cmd <= JCCMD_START || cmd >= JCCMD_END) {
		return EJC_INPUT_NULL;
	}
	JCINPUT *jcp = (JCINPUT *) handle;

	jcp->m_cmdtype = cmd;

	return EJC_SUSSESS;
}

//}     //end of namespace jclms

//设置字符串类型的值
JCERROR JCLMSCCB2014_API JcLockSetString(const int handle, const JCITYPE mtype,
					 const char *str)
{
	assert(handle > 0);
	assert(mtype > JCI_START && mtype < JCI_END);
	assert(str != NULL && strlen(str) > 0);
	if (handle <= 0 || mtype <= JCI_START || mtype >= JCI_END
	    || str == NULL || strlen(str) == 0) {
		return EJC_INPUT_NULL;
	}
	JCINPUT *jcp = (JCINPUT *) handle;
	zwJcLockDumpJCINPUT(handle);
	switch (mtype) {
	case JCI_ATMNO:
		strncpy(jcp->m_atmno, str, sizeof(jcp->m_atmno));
		break;
	case JCI_LOCKNO:
		strncpy(jcp->m_lockno, str, sizeof(jcp->m_lockno));
		break;
	case JCI_PSK:
		strncpy(jcp->m_psk, str, sizeof(jcp->m_psk));
		break;
	}
	return EJC_SUSSESS;

}

//设置整数类型的值
JCERROR JCLMSCCB2014_API JcLockSetInt(const int handle, const JCITYPE mtype,
				      int num)
{
	assert(handle > 0);
	assert(mtype > JCI_START && mtype < JCI_END);
	assert(num > JC_INVALID_VALUE);
	if (handle <= 0 || mtype <= JCI_START || mtype >= JCI_END
	    || num <= JC_INVALID_VALUE) {
		return EJC_INPUT_NULL;
	}
	JCINPUT *jcp = (JCINPUT *) handle;
	assert(jcp->m_stepoftime >= 6 && jcp->m_stepoftime <= ZW_ONE_DAY);
	zwJcLockDumpJCINPUT(handle);
	switch (mtype) {
	case JCI_DATETIME:
		//时间必须经过规格化
		if (num < (1400 * 1000 * 1000)) {
			return EJC_DATETIME_INVALID;
		}
		jcp->m_datetime = myGetNormalTime(num, jcp->m_stepoftime);
		break;
	case JCI_VALIDITY:
		assert(num > 0 && num <= 1440 * 7);
		if (num <= 0 || num > (1440 * 7)) {
			return EJC_VALIDRANGE_INVALID;
		}
		jcp->m_validity = num;
		break;
	case JCI_CLOSECODE:
		//assert(num>=10000000 && num<=99999999);
		//if (num<10000000 || num>99999999)
		//{
		//      return EJC_CLOSECODE_INVALID;
		//}
		jcp->m_closecode = num;
		break;
	case JCI_TIMESTEP:	//反推时间步长
		assert(num >= 3 && num <= 3600);
		if (num < 0 || num > 3600) {
			return EJC_CMDTYPE_TIMESTEP_INVALID;
		}
		jcp->m_stepoftime = num;
		break;
	}
	return EJC_SUSSESS;
}

JCERROR JCLMSCCB2014_API JcLockCheckInput(const int handle)
{
	zwJcLockDumpJCINPUT(handle);
	const int ZW_DIGI8_LOW = 10 * ZWMEGA;
	const int ZW_DIGI8_HIGH = 100 * ZWMEGA;
	JCINPUT *jcp = (JCINPUT *) handle;
	//假定这些数字字段在二进制层面都是等同于int的长度的，以便通过一个统一的函数进行HASH运算
	assert(sizeof(jcp->m_datetime) == sizeof(int));
	assert(sizeof(jcp->m_validity) == sizeof(int));
	assert(sizeof(jcp->m_closecode) == sizeof(int));
	assert(sizeof(jcp->m_cmdtype) == sizeof(int));

	assert(jcp->m_datetime >= (ZW_LOWEST_DATE)
	       && jcp->m_datetime < ZW_MAXDATA32);
	assert(jcp->m_cmdtype > JCCMD_START && jcp->m_cmdtype < JCCMD_END);
	if (JCCMD_INIT_CLOSECODE != jcp->m_cmdtype && JCCMD_CCB_CLOSECODE != jcp->m_cmdtype) {	//生成初始闭锁码,以及真正闭锁码时，不检查有效期和闭锁码的值
		assert(jcp->m_validity >= 0 && jcp->m_validity <= (24 * 60));
		//10,000,000 8位数，也就是10-100M之间
		assert(jcp->m_closecode >= ZW_DIGI8_LOW
		       && jcp->m_closecode <= ZW_DIGI8_HIGH);
	}

	//限度是小于14开头的时间(1.4G秒)或者快要超出ZW_MAXDATA32秒的话就是非法了
	if (jcp->m_datetime < (ZW_LOWEST_DATE) || jcp->m_datetime > ZW_MAXDATA32) {	//日期时间秒数在2014年的某个1.4G秒之前的日子，或者超过2038年(32位有符号整数最大值)则无效
		return EJC_DATETIME_INVALID;
	}
	if (JCCMD_INIT_CLOSECODE != jcp->m_cmdtype && JCCMD_CCB_CLOSECODE != jcp->m_cmdtype) {	//生成初始闭锁码,以及真正闭锁码时，不检查有效期和闭锁码的值
		if (jcp->m_validity < 0 || jcp->m_validity > (24 * 60)) {	//有效期分钟数为负数或者大于一整天则无效
			return EJC_VALIDRANGE_INVALID;
		}
		if (jcp->m_closecode < ZW_DIGI8_LOW || jcp->m_closecode > ZW_DIGI8_HIGH) {	//闭锁码小于8位或者大于8位则无效
			return EJC_CLOSECODE_INVALID;
		}
	}			//if (JCCMD_INIT_CLOSECODE!=jcp->m_cmdtype)
	if (jcp->m_stepoftime <= 0 || jcp->m_stepoftime >= ZW_ONE_DAY) {	//搜索步长为负数或者大于一整天则无效
		return EJC_CMDTYPE_TIMESTEP_INVALID;
	}
	if (jcp->m_reverse_time_length <= 0 || jcp->m_reverse_time_length >= (365 * ZW_ONE_DAY)) {	//往前搜索时间为负数或者大于一整年则无效
		return EJC_CMDTYPE_TIMELEN_INVALID;
	}

	if (jcp->m_cmdtype <= JCCMD_START || jcp->m_cmdtype >= JCCMD_END) {
		return EJC_CMDTYPE_INVALID;
	}
	return EJC_SUSSESS;
}
