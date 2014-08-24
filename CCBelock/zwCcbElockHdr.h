#ifndef zwCcbElockHdr_h__
#define zwCcbElockHdr_h__
//#include "stdafx.h"
/////////////////////////////////�����ú���/////////////////////////////////////////
//��������
typedef enum jcmsg_ccb_elock_t{
	JCMSG_INVALID_TYPE,	//��Ч��������
	//ATMC TO LOCK MSG:	
	JCMSG_LOCK_ACTIVE_REQUEST,	//������������(��ȡ���߱�ź͹�Կ)
	JCMSG_SEND_LOCK_ACTIVEINFO,	//�������߼�����Ϣ(���߳�ʼ��)
	JCMSG_QUERY_LOCK_STATUS,	//��ѯ����״̬
	JCMSG_TIME_SYNC,			//ʱ��ͬ��
	JCMSG_GET_CLOSECODE,		//��ȡ������
	JCMSG_GET_LOCK_LOG,			//��ȡ��־
	//LOCK TO ATMC MSG:
	JCMSG_SEND_INITCLOSECODE,		//���ͱ�����
	JCMSG_PUSH_WARNING,				//�����������͸澯
	JCMSG_SEND_UNLOCK_CERTCODE,	//���߷��Ϳ�����֤��
	JCMSG_REQUEST_TIME_SYNC,	//��������Ҫ��ʱ��ͬ��
}JC_MSG_TYPE;

typedef struct jcLockStatus_t{
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
}JCLOCKSTATUS;
void zwLockStatusDataSplit(const char *LockStatus,JCLOCKSTATUS &lst);
void zwStatusData2String(const JCLOCKSTATUS &lst,JCLOCKSTATUS &ostr);
string LockStatusStringMerge(JCLOCKSTATUS &ostr);

namespace jcAtmcConvertDLL{
	//JSON�����ַ�������
	extern const char *JCSTR_CMDTITLE;
	extern const char *JCSTR_LOCK_ACTIVE_REQUEST;	//���߼�������
	extern const char *JCSTR_LOCK_INIT;				//���߳�ʼ��
	extern const char *JCSTR_QUERY_LOCK_STATUS;		//��ѯ����״̬
	extern const char *JCSTR_TIME_SYNC;				//ʱ��ͬ��
	extern const char *JCSTR_READ_CLOSECODE;		//��ȡ������
	extern const char *JCSTR_SEND_INITCLOSECODE;	//��λ���������ͳ�ʼ������
	extern const char *JCSTR_SEND_UNLOCK_CERTCODE;	//��λ������������֤��
	extern const char *JCSTR_GET_LOCK_LOG;			//��ȡ��־
	extern const char *JCSTR_PUSH_WARNING;			//�����������͸澯
	extern const char *JCSTR_REQUEST_TIME_SYNC;		//��������Ҫ��ʱ��ͬ��

	extern const char *CCBSTR_CODE;
	extern const char *CCBSTR_NAME;
	extern const char *CCBSTR_DATE;
	extern const char *CCBSTR_TIME;
	extern const char *CCBSTR_DEVCODE;


	const JC_MSG_TYPE zwCCBxml2JCjson(const string &inXML,string &outJson);
	const JC_MSG_TYPE zwJCjson2CCBxml(const string &inJson,string &outXML);
	extern const char *LOCKMAN_NAME;
}	//namespace jcAtmcConvertDLL{

enum JC_CCBELOCK_ERROR_CODE{
	ELOCK_ERROR_SUCCESS=0,		//�����ɹ�
	ELOCK_ERROR_NOTSUPPORT=1,	//��֧�ֵĽӿ�
	ELOCK_ERROR_HARDWAREERROR=2,//Ӳ������
	ELOCK_ERROR_PARAMINVALID=3,	//�����Ƿ�
	ELOCK_ERROR_CONNECTLOST=4,	//ʧȥ�豸����
	ELOCK_ERROR_TIMEOUT=5		//������ʱ
};

//ͨѶ�߳�����״̬����
enum{
	//1145���ֵ���ȡ�ģ������壬ֻ��Ϊ�˱����ⲿ����һ��0����1֮��ǡ�÷���ö��ֵ�Ĵ�������
	ZWTHR_STOP=1145,	
	ZWTHR_RUN,
};

namespace zwccbthr{
void ThreadLockComm();				//������֮���ͨѶ�߳�
void ThreadHeartJump();				//�������������̣߳�
const string getString(void);
void myLoadConfig(const string &cfgFileName);
extern string s_dbgReturn;
}	//namespace zwccbthr{


typedef void (cdecl *RecvMsgRotine)(const char *pszMsg);
namespace zwCfg{
	extern RecvMsgRotine g_WarnCallback;
}

//�Ӹ�����ʱ����������ȡ����(YYYYMMDD)��ʱ��(HHMMSS)�ַ���
void zwGetLocalDateTimeString(time_t inTime,string &outDate,string &outTime);

void ZWTRACE(const char *x);

class zw_trace
{
	string m_str;
	string m_start;
	string m_end;
public:
	zw_trace(const char *funcName);
	~zw_trace();
};
#define ZWFUNCTRACE	zw_trace fntr(__FUNCTION__);



#endif // zwCcbElockHdr_h__
