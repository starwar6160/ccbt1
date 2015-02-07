#ifndef zwHidMulHeader_h__
#define zwHidMulHeader_h__

#ifdef __cplusplus
extern "C" {
#endif

	////////////////////////������Json����ӿ�//////////////////////////////////////////////////
	//һ���豸�б�ӿ� 1���ص���������
	typedef void (ZJY1501STD *ReturnDrives)(const char* DrivesTypePID,const char * DrivesIDListJson);

	CCBELOCK_API void ZJY1501STD SetReturnDrives(ReturnDrives _DrivesListFun);
	//3����ȡ�豸�б��ָ��
	//˵����
	//	��1��DrivesType��ʾ�豸���ͣ����������ǣ�����=Lock���ܺ�=Encryption
	//	��2������int���ͺ����ķ���ֵ��0��ʾ�ɹ��������ʾ������ͬ��
	//	��3��DrivesIDListΪjson��ʽ�ַ��������г������з���Ҫ���豸��DrivesID��HID�豸���кţ�
	//	��4��������ListDrivesʱ���ӿں���Ӧ���ص�ǰ���д����豸���б�
	//	��5���������豸�����γ�ʱ���ӿں���Ӧ�÷��ز����豸�������͵������б��ظ���������λ������ȥ�أ�
	CCBELOCK_API int ZJY1501STD ListDrives(const char * DrivesTypePID);
	//1�����豸
	CCBELOCK_API int ZJY1501STD OpenDrives(const char* DrivesTypePID,const char * DrivesIdSN);
	//	2���ر��豸
	CCBELOCK_API int ZJY1501STD CloseDrives(const char* DrivesTypePID,const char * DrivesIdSN);
	
	//extern ReturnDrives G_JCHID_ENUM_DEV2015A;

	//�����豸��Ϣ�ӿ� ˵������1��DrivesMessage��AnyMessage��Ϊjson��ʽ�ַ�
	//1���ص���������
	typedef void (ZJY1501STD *ReturnMessage)(const char* DrivesIdSN,char* DrivesMessageJson);
	//2�������豸��Ϣ���صĻص�����
	CCBELOCK_API void ZJY1501STD SetReturnMessage(ReturnMessage _MessageHandleFun);
	//3�����豸����ָ��ĺ���
	CCBELOCK_API int ZJY1501STD InputMessage(const char * DrivesTypePID,const char * DrivesIdSN,const char * AnyMessageJson);
	CCBELOCK_API void myHidSerialTest126(void);
#ifdef __cplusplus
}
#endif

/////////////////////////////Google Test In DLL/////////////////////////////////////////////
CCBELOCK_API int zwStartGtestInDLL(void);

namespace jcLockJsonCmd_t2015a21{
	CCBELOCK_API long OpenJson(long lTimeOut);
	CCBELOCK_API long SendToLockJson(const char *pszJson);
	CCBELOCK_API const char * RecvFromLockJson(const int timeoutMs);
	CCBELOCK_API long CloseJson();	
	CCBELOCK_API void jcMulHidEnum( const int hidPid ,string &jcDevListJson);
	extern ReturnDrives G_JCHID_ENUM_DEV2015A;
	extern ReturnMessage G_JCHID_RECVMSG_CB;
	extern const char * G_DEV_LOCK;
	extern const char * G_DEV_SECBOX;
	extern const int G_SUSSESS;
	extern const int G_FAIL;
	class zwJcHidDbg15A;
	uint32_t myHidSerialToInt(char *hidSerial);
	void zwCleanJchidVec(void);
}	//end of namespace jcLockJsonCmd_t2015a21{
namespace jch=jcLockJsonCmd_t2015a21;

namespace jcLockJsonCmd_t2015a27{
	CCBELOCK_API long OpenJson(long lTimeOut);
	CCBELOCK_API long JCAPISTD CloseJson();
	CCBELOCK_API long SendToLockJson(const char *pszJson);
	CCBELOCK_API const char * RecvFromLockJson( const int timeoutMs );
}	//namespace jcLockJsonCmd_t2015a27{
namespace jlua=jcLockJsonCmd_t2015a27;


#endif // zwHidMulHeader_h__
