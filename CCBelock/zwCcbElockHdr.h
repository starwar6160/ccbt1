/////////////////////////////////�����ú���/////////////////////////////////////////
//��������
typedef enum jcmsg_ccb_elock_t{
	JCMSG_INVALID_TYPE,	//��Ч��������
	//ATMC TO LOCK MSG:	
	JCMSG_LOCK_ACTIVE_REQUEST,
	JCMSG_GET_LOCK_ACTIVEINFO,
	JCMSG_QUERY_LOCK_STATUS,
	JCMSG_TIME_SYNC,
	JCMSG_GET_CLOSECODE,
	JCMSG_GET_LOCK_LOG,
	//LOCK TO ATMC MSG:
	JCMSG_SEND_CLOSECODE,
	JCMSG_PUSH_WARNING,
	JCMSG_SEND_UNLOCK_CERTCODE,
	JCMSG_REQUEST_TIME_SYNC,
	JCMSG_TEST730A1,	//������
}JC_MSG_TYPE;

namespace jcAtmcConvertDLL{
	const JC_MSG_TYPE zwCCBxml2JCjson(const string &inXML,string &outJson);
	const JC_MSG_TYPE zwJCjson2CCBxml(const string &inJson,string &outXML);
}	//namespace jcAtmcConvertDLL{

enum JC_CCBELOCK_ERROR_CODE{
	ELOCK_ERROR_SUCCESS=0,		//�����ɹ�
	ELOCK_ERROR_NOTSUPPORT=1,	//��֧�ֵĽӿ�
	ELOCK_ERROR_HARDWAREERROR=2,//Ӳ������
	ELOCK_ERROR_PARAMINVALID=3,	//�����Ƿ�
	ELOCK_ERROR_CONNECTLOST=4,	//ʧȥ�豸����
	ELOCK_ERROR_TIMEOUT=5		//������ʱ
};

