#include <cstdint>
namespace zwTools2015{
//十六进制字符串转换为二进制
int myHex2Bin(const string &inHexStr,string &outBinStr);
}	//namespace zwTools2015{

using zwTools2015::myHex2Bin;


#pragma pack(push)
#pragma pack(1)
struct jclxBlackKeyLog1608_t
{
	unsigned char lxSerialNo[5];	//离线锁具序列号
	int32_t openLockTime;			//开锁时间
	int8_t openLockStatus;			//开锁状态
	int32_t closeCode;				//闭锁码
	int32_t closeLockTime;			//关锁时间
};

//激活日志
struct jclxRedKeyLog1608Active_t{
	int8_t logType;
	int8_t bStatus;
	uint8_t redKeySN[6];
	int32_t actTime;
	uint8_t CRC;
};

//开锁日志
struct jclxRedKeyLog1608OpenLock_t{
	int8_t logType;
	int8_t bStatus;
	uint8_t blackKeySN[6];
	int32_t openLockTime;
	uint8_t CRC;
};

//关锁日志
struct jclxRedKeyLog1608CloseLock_t{
	int8_t logType;
	int8_t bStatus;
	uint8_t memoryPadding[2];
	int32_t closeCode;
	int32_t closeLockTime;
	uint8_t CRC;
};

//报警日志
struct jclxRedKeyLog1608Alarm_t{
	int8_t logType;
	uint8_t memoryPadding[3];
	uint32_t lockStatus;
	int32_t alarmTime;
	uint8_t CRC;
};

//时间同步日志
struct jclxRedKeyLog1608TimeSync_t{
	int8_t logType;
	uint8_t memoryPadding[3];
	int32_t timeBeforeSync;
	int32_t timeAfterSync;
	uint8_t CRC;
};

//提取日志的日志
struct jclxRedKeyLog1608ExtractLog_t{
	int8_t logType;
	int8_t bStatus;
	uint8_t redKeySN[6];
	int32_t extractTime;
	uint8_t CRC;
};

//重置黑钥匙日志
struct jclxRedKeyLog1608ResetBlackKey_t{
	int8_t logType;
	int8_t bStatus;
	uint8_t blackKeySN[6];
	int32_t resetTime;
	uint8_t CRC;
};


#pragma pack(pop)