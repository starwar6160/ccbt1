#ifndef zwCcbElockHdr_h__
#define zwCcbElockHdr_h__

/////////////////////////////////�����ú���/////////////////////////////////////////
//��������
typedef enum jcmsg_ccb_elock_t{
	JCMSG_INVALID_TYPE,	//��Ч��������
	//ATMC TO LOCK MSG:	
	JCMSG_LOCK_ACTIVE_REQUEST,	//������������(��ȡ���߱�ź͹�Կ)
	JCMSG_SEND_LOCK_ACTIVEINFO,	//�������߼�����Ϣ(���߳�ʼ��)
	JCMSG_QUERY_LOCK_STATUS,
	JCMSG_TIME_SYNC,
	JCMSG_GET_CLOSECODE,		//��ȡ������
	JCMSG_GET_LOCK_LOG,
	//LOCK TO ATMC MSG:
	JCMSG_SEND_CLOSECODE,
	JCMSG_PUSH_WARNING,
	JCMSG_SEND_UNLOCK_CERTCODE,	//���߷��Ϳ�����֤��
	JCMSG_REQUEST_TIME_SYNC,
	JCMSG_TEST730A1,	//������
}JC_MSG_TYPE;

namespace jcAtmcConvertDLL{
	//JSON�����ַ�������
	extern const char *JCSTR_LOCK_ACTIVE_REQUEST;	//���߼�������
	extern const char *JCSTR_LOCK_INIT;				//���߳�ʼ��
	extern const char *JCSTR_READ_CLOSECODE;		//��ȡ������

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
	void wait(int milliseconds);
void zwStartLockCommThread(void);	//����������֮���ͨѶ�߳�
void zwStopLockCommThread(void);	//ֹͣ������֮���ͨѶ�߳�
const string getString(void);

}	//namespace zwccbthr{

typedef void (cdecl *RecvMsgRotine)(const char *pszMsg);
namespace zwCfg{
	extern RecvMsgRotine g_WarnCallback;
}

//�Ӹ�����ʱ����������ȡ����(YYYYMMDD)��ʱ��(HHMMSS)�ַ���
void zwGetDateTimeString(time_t inTime,string &outDate,string &outTime);


#endif // zwCcbElockHdr_h__
