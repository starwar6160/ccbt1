#ifndef zwCcbElockHdr_h__
#define zwCcbElockHdr_h__
//��������
typedef enum jcmsg_ccb_elock_t {
	JCMSG_INVALID_TYPE,	//��Ч��������
	//ATMC TO LOCK MSG:     
	JCMSG_LOCK_ACTIVE_REQUEST,	//������������(��ȡ���߱�ź͹�Կ)
	JCMSG_SEND_LOCK_ACTIVEINFO,	//�������߼�����Ϣ(���߳�ʼ��)
	JCMSG_QUERY_LOCK_STATUS,	//��ѯ����״̬
	JCMSG_TIME_SYNC,	//ʱ��ͬ��
	JCMSG_GET_CLOSECODE,	//��ȡ������
	JCMSG_GET_LOCK_LOG,	//��ȡ��־
	//LOCK TO ATMC MSG:
	JCMSG_SEND_INITCLOSECODE,	//���ͱ�����
	JCMSG_PUSH_WARNING,	//�����������͸澯
	JCMSG_SEND_UNLOCK_CERTCODE,	//���߷��Ϳ�����֤��
	JCMSG_REQUEST_TIME_SYNC,	//��������Ҫ��ʱ��ͬ��
	//////////////////////////////////////////////////////////////////////////
	//20141111.����Ҫ�����ӵ��¶Ⱥ��񶯴�����֧�ֱ���
	JCMSG_SENSE_TEMPTURE,	//�¶ȴ�����
	JCMSG_SENSE_SHOCK,	//�񶯴�����
	JCMSG_SENSE_SET_INSIDE_LOOP_TIMES,//ATM�������ϴ���Сѭ����������
	JCMSG_SENSE_SET_INSIDE_LOOP_PERIOD,//ATM�������ϴ���Сѭ������(��λ��)����
	JCMSG_SENSE_SET_OUTSIDE_LOOP_PERIOD,//ATM�������ϴ��Ĵ�ѭ������(��λ����)����
} JC_MSG_TYPE;

//Ϊ����־����������;��Ƶ����ݽṹ
typedef struct jcLockStatus_t {
	string LogGenDate;	//20140912��1540,Ӧ����Ҫ����ϵ���־���ɵ�ʱ���ֶ�
	string ActiveStatus;
	string EnableStatus;
	string LockStatus;
	string DoorStatus;
	string BatteryStatus;
	string ShockAlert;
	string ShockValue;
	string TempAlert;
	string nodeTemp;
	string PswTryAlert;
	string LockOverTime;
} JCLOCKSTATUS;

namespace jcAtmcConvertDLL {
	//JSON�����ַ�������
	extern const char *JCSTR_CMDTITLE;
	extern const char *JCSTR_LOCK_ACTIVE_REQUEST;	//���߼�������
	extern const char *JCSTR_LOCK_INIT;	//���߳�ʼ��
	extern const char *JCSTR_QUERY_LOCK_STATUS;	//��ѯ����״̬
	extern const char *JCSTR_TIME_SYNC;	//ʱ��ͬ��
	extern const char *JCSTR_READ_CLOSECODE;	//��ȡ������
	extern const char *JCSTR_SEND_INITCLOSECODE;	//��λ���������ͳ�ʼ������
	extern const char *JCSTR_SEND_UNLOCK_CERTCODE;	//��λ������������֤��
	extern const char *JCSTR_GET_LOCK_LOG;	//��ȡ��־
	extern const char *JCSTR_PUSH_WARNING;	//�����������͸澯
	extern const char *JCSTR_REQUEST_TIME_SYNC;	//��������Ҫ��ʱ��ͬ��
	//20141111����.�¶��񶯴���������֧��
	extern const char *JCSTR_SENSE_TEMPTURE;	//�¶ȴ�����
	extern const char *JCSTR_SENSE_SHOCK;	//�񶯴�����
	extern const char *JCSTR_SENSE_SET_INSIDE_LOOP_TIMES;//ATM�������ϴ���Сѭ����������
	extern const char *JCSTR_SENSE_SET_INSIDE_LOOP_PERIOD;//ATM�������ϴ���Сѭ������(��λ��)����
	extern const char *JCSTR_SENSE_SET_OUTSIDE_LOOP_PERIOD;//ATM�������ϴ��Ĵ�ѭ������(��λ����)����


	extern const char *CCBSTR_CODE;
	extern const char *CCBSTR_NAME;
	extern const char *CCBSTR_DATE;
	extern const char *CCBSTR_TIME;
	extern const char *CCBSTR_DEVCODE;

	const JC_MSG_TYPE zwCCBxml2JCjson(const string & inXML,
					  string & outJson);
	const JC_MSG_TYPE zwJCjson2CCBxml(const string & inJson,
					  string & outXML);
	extern const char *LOCKMAN_NAME;
} //namespace jcAtmcConvertDLL{ 

enum JC_CCBELOCK_ERROR_CODE {
	ELOCK_ERROR_SUCCESS = 0,	//�����ɹ�
	ELOCK_ERROR_NOTSUPPORT = 1,	//��֧�ֵĽӿ�
	ELOCK_ERROR_HARDWAREERROR = 2,	//Ӳ������
	ELOCK_ERROR_PARAMINVALID = 3,	//�����Ƿ�
	ELOCK_ERROR_CONNECTLOST = 4,	//ʧȥ�豸����
	ELOCK_ERROR_TIMEOUT = 5	//������ʱ
};

class zw_trace {
	string m_str;
	string m_start;
	string m_end;
      public:
	 zw_trace(const char *funcName);
	~zw_trace();
};
#define ZWFUNCTRACE	zw_trace fntr(__FUNCTION__);

void zwLockStatusDataSplit(const char *LockStatus, JCLOCKSTATUS & lst);
void zwStatusData2String(const JCLOCKSTATUS & lst, JCLOCKSTATUS & ostr);
string LockStatusStringMerge(JCLOCKSTATUS & ostr);
void zwGetDLLPath(HMODULE hDLL, char *pDllPath, const int dllPathLen);

//�Ӹ�����ʱ����������ȡ����(YYYYMMDD)��ʱ��(HHMMSS)�ַ���
namespace zwTimeFunc {
	//�Ӹ�����ʱ��GMT��������ȡ��������(YYYYMMDD)��ʱ��(HHMMSS)�ַ���
	void zwGetLocalDateTimeString(time_t inTimeUTC, string & outDateLocal,
				      string & outTimeLocal);
	//�ѽ��еı������ں�ʱ������ϳ�֮��ת��Ϊһ��UTCʱ��(��������ʱ��,Э������ʱ)����
	//���е�������YYYYMMDD��ʽ8λ��ʱ����HHMMSS6λ
	void zwCCBDateTime2UTC(const char *ccbDateLocal,
			       const char *ccbTimeLocal, time_t * outUTC);
} //namespace zwTimeFunc{ 
using zwTimeFunc::zwGetLocalDateTimeString;
using zwTimeFunc::zwCCBDateTime2UTC;

typedef void (cdecl * RecvMsgRotine) (const char *pszMsg);
namespace zwCfg {
	extern RecvMsgRotine g_WarnCallback;
	extern bool s_hidOpened;
} 
void ZWDBGMSG(const char *x);
void ZWDBGWARN(const char *x);
#define ZWUSE_HID_MSG_SPLIT
extern HMODULE G_DLL_HMODULE;

//�Ϊ128�ֽ�,���ڲ���Ŀ�ľ���ﵽ���Ʊ�¶����
//��ԭ����namespace zwCfg���浥��������ģ���Ϊ�ñ����е��������⣬�ȵ����������
#define JC_MSG_MAXLEN	(1*1024)
#define JC_CCBDLL_TIMEOUT	(30)

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif


unsigned char crc8Short( const void *inputData,const int inputLen );
unsigned long Crc32_ComputeBuf(unsigned long inCrc32, const void *buf, size_t bufLen);


namespace jchidDevice2015{

}	//namespace jchidDevice2015{

#endif // zwCcbElockHdr_h__
