#include <cstdint>
namespace zwTools2015{
//ʮ�������ַ���ת��Ϊ������
int myHex2Bin(const string &inHexStr,string &outBinStr);
//��8�ֽ����ڵĴ�˽�β����������ת��Ϊʮ��������
int64_t beBin2int(const char *inBeBinData,int inDataLen);
void myGetTimeStr1607(time_t inTime,string &outTimeStr);
}	//namespace zwTools2015{

using zwTools2015::myHex2Bin;
using zwTools2015::beBin2int;
using zwTools2015::myGetTimeStr1607;

#pragma pack(push)
#pragma pack(1)
//////////////////////////////��Կ����־////////////////////////////////////////////
struct jclxBlackKeyLog1608_t
{
	char lxSerialNo[5];	//�����������к�,��˸�ʽ�洢��
	int32_t openLockTime;			//����ʱ��
	int8_t openLockStatus;			//����״̬
	int32_t closeCode;				//������
	int32_t closeLockTime;			//����ʱ��
};

////////////////////////////////��Կ����־//////////////////////////////////////////
//������־
struct jclxRedKeyLog1608Active_t{
	int8_t logType;
	int8_t bStatus;
	char redKeySN[6];	//���к��ֶ��Ǵ�˸�ʽ�洢
	int32_t actTime;
	uint8_t CRC;
};

//������־
struct jclxRedKeyLog1608OpenLock_t{
	int8_t logType;
	int8_t bStatus;
	char blackKeySN[6];	//���к��ֶ��Ǵ�˸�ʽ�洢
	int32_t openLockTime;
	uint8_t CRC;
};

//������־
struct jclxRedKeyLog1608CloseLock_t{
	int8_t logType;
	int8_t bStatus;
	char memoryPadding[2];
	int32_t closeCode;
	int32_t closeLockTime;
	uint8_t CRC;
};

//������־
struct jclxRedKeyLog1608Alarm_t{
	int8_t logType;
	char memoryPadding[3];
	uint32_t lockStatus;
	int32_t alarmTime;
	uint8_t CRC;
};

//ʱ��ͬ����־
struct jclxRedKeyLog1608TimeSync_t{
	int8_t logType;
	char memoryPadding[3];
	int32_t timeBeforeSync;
	int32_t timeAfterSync;
	uint8_t CRC;
};

//��ȡ��־����־
struct jclxRedKeyLog1608ExtractLog_t{
	int8_t logType;
	int8_t bStatus;
	char redKeySN[6];	//���к��ֶ��Ǵ�˸�ʽ�洢
	int32_t extractTime;
	uint8_t CRC;
};

//���ú�Կ����־
struct jclxRedKeyLog1608ResetBlackKey_t{
	int8_t logType;
	int8_t bStatus;
	char blackKeySN[6];	//���к��ֶ��Ǵ�˸�ʽ�洢
	int32_t resetTime;
	uint8_t CRC;
};
#pragma pack(pop)

string jclxBlackKeyLog2Str(const struct jclxBlackKeyLog1608_t *inBlackLog);
