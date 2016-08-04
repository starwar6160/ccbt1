#include <cstdint>
namespace zwTools2015{
//ʮ�������ַ���ת��Ϊ������
int myHex2Bin(const string &inHexStr,string &outBinStr);
}	//namespace zwTools2015{

using zwTools2015::myHex2Bin;


#pragma pack(push)
#pragma pack(1)
struct jclxBlackKeyLog1608_t
{
	unsigned char lxSerialNo[5];	//�����������к�
	int32_t openLockTime;			//����ʱ��
	int8_t openLockStatus;			//����״̬
	int32_t closeCode;				//������
	int32_t closeLockTime;			//����ʱ��
};

//������־
struct jclxRedKeyLog1608Active_t{
	int8_t logType;
	int8_t bStatus;
	uint8_t redKeySN[6];
	int32_t actTime;
	uint8_t CRC;
};

//������־
struct jclxRedKeyLog1608OpenLock_t{
	int8_t logType;
	int8_t bStatus;
	uint8_t blackKeySN[6];
	int32_t openLockTime;
	uint8_t CRC;
};

//������־
struct jclxRedKeyLog1608CloseLock_t{
	int8_t logType;
	int8_t bStatus;
	uint8_t memoryPadding[2];
	int32_t closeCode;
	int32_t closeLockTime;
	uint8_t CRC;
};

//������־
struct jclxRedKeyLog1608Alarm_t{
	int8_t logType;
	uint8_t memoryPadding[3];
	uint32_t lockStatus;
	int32_t alarmTime;
	uint8_t CRC;
};

//ʱ��ͬ����־
struct jclxRedKeyLog1608TimeSync_t{
	int8_t logType;
	uint8_t memoryPadding[3];
	int32_t timeBeforeSync;
	int32_t timeAfterSync;
	uint8_t CRC;
};

//��ȡ��־����־
struct jclxRedKeyLog1608ExtractLog_t{
	int8_t logType;
	int8_t bStatus;
	uint8_t redKeySN[6];
	int32_t extractTime;
	uint8_t CRC;
};

//���ú�Կ����־
struct jclxRedKeyLog1608ResetBlackKey_t{
	int8_t logType;
	int8_t bStatus;
	uint8_t blackKeySN[6];
	int32_t resetTime;
	uint8_t CRC;
};


#pragma pack(pop)