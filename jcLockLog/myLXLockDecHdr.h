#include <cstdint>
namespace zwTools2015{
//十六进制字符串转换为二进制
int myHex2Bin(const string &inHexStr,string &outBinStr);
//把8字节以内的大端结尾二进制数据转换为十进制整数
int64_t beBin2int(const char *inBeBinData,int inDataLen);
void myGetTimeStr1607(time_t inTime,string &outTimeStr);
}	//namespace zwTools2015{

using zwTools2015::myHex2Bin;
using zwTools2015::beBin2int;
using zwTools2015::myGetTimeStr1607;

#pragma pack(push)
#pragma pack(1)
//////////////////////////////黑钥匙日志////////////////////////////////////////////
struct jclxBlackKeyLog1608_t
{
	char lxSerialNo[5];	//离线锁具序列号,大端格式存储；
	int32_t openLockTime;			//开锁时间
	int8_t openLockStatus;			//开锁状态
	int32_t closeCode;				//闭锁码
	int32_t closeLockTime;			//关锁时间
};

////////////////////////////////红钥匙日志//////////////////////////////////////////
//激活日志
struct jclxRedKeyLog1608Active_t{
	int8_t logType;
	int8_t bStatus;
	char redKeySN[6];	//序列号字段是大端格式存储
	int32_t actTime;
	uint8_t CRC;
};

//开锁日志
struct jclxRedKeyLog1608OpenLock_t{
	int8_t logType;
	int8_t bStatus;
	char blackKeySN[6];	//序列号字段是大端格式存储
	int32_t openLockTime;
	uint8_t CRC;
};

//关锁日志
struct jclxRedKeyLog1608CloseLock_t{
	int8_t logType;
	int8_t bStatus;
	char memoryPadding[2];
	int32_t closeCode;
	int32_t closeLockTime;
	uint8_t CRC;
};

//报警日志
struct jclxRedKeyLog1608Alarm_t{
	int8_t logType;
	char memoryPadding[3];
	uint32_t lockStatus;
	int32_t alarmTime;
	uint8_t CRC;
};

//时间同步日志
struct jclxRedKeyLog1608TimeSync_t{
	int8_t logType;
	char memoryPadding[3];
	int32_t timeBeforeSync;
	int32_t timeAfterSync;
	uint8_t CRC;
};

//提取日志的日志
struct jclxRedKeyLog1608ExtractLog_t{
	int8_t logType;
	int8_t bStatus;
	char redKeySN[6];	//序列号字段是大端格式存储
	int32_t extractTime;
	uint8_t CRC;
};

//重置黑钥匙日志
struct jclxRedKeyLog1608ResetBlackKey_t{
	int8_t logType;
	int8_t bStatus;
	char blackKeySN[6];	//序列号字段是大端格式存储
	int32_t resetTime;
	uint8_t CRC;
};
#pragma pack(pop)

string jclxBlackKeyLog2Str(const struct jclxBlackKeyLog1608_t *inBlackLog);
