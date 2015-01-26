#ifndef CCBelock_h__
#define CCBelock_h__
#include <string>
using std::string;
// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� CCBELOCK_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// CCBELOCK_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef _ZWUSE_AS_JNI
#define CCBELOCK_API
typedef void (*RecvMsgRotine)(const char *pszMsg);
#else
#ifdef CCBELOCK_EXPORTS
#define CCBELOCK_API __declspec(dllexport) 
#else
#define CCBELOCK_API __declspec(dllimport)
#endif

typedef void (cdecl *RecvMsgRotine)(const char *pszMsg);
#endif

#define JCAPISTD
#define ZJY1501STD	__stdcall
//////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif

CCBELOCK_API long	JCAPISTD Open(long lTimeOut);
CCBELOCK_API long	JCAPISTD Close();
CCBELOCK_API long	JCAPISTD Notify(const char *pszMsg);
CCBELOCK_API int	JCAPISTD SetRecvMsgRotine(RecvMsgRotine pRecvMsgFun);

//////////////////////////////////////////////////////////////////////////
CCBELOCK_API void zwPushString(const char *str);
//CCBELOCK_API const char * dbgGetLockReturnXML(void);
CCBELOCK_API void myLuaBridgeSendJsonAPI(const char *luaScriptFile);

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

namespace jcLockJsonCmd_t2015a{

	CCBELOCK_API long OpenJson(long lTimeOut);
	CCBELOCK_API long SendToLockJson(const char *pszJson);
	CCBELOCK_API const char * RecvFromLockJson(const int timeoutMs);
	CCBELOCK_API long CloseJson();	
	CCBELOCK_API void jcMulHidEnum( const int hidPid ,string &jcDevListJson);
	extern ReturnDrives G_JCHID_ENUM_DEV2015A;
	extern const char * G_DEV_LOCK;
	extern const char * G_DEV_SECBOX;
}	//end of namespace jcLockJsonCmd_t2015a{


#endif // CCBelock_h__
