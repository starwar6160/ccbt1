#ifndef CCBelock_h__
#define CCBelock_h__

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

#ifdef __cplusplus
extern "C" {
#endif

CCBELOCK_API long	JCAPISTD Open(long lTimeOut);
CCBELOCK_API long	JCAPISTD Close();
CCBELOCK_API long	JCAPISTD Notify(const char *pszMsg);
CCBELOCK_API int	JCAPISTD SetRecvMsgRotine(RecvMsgRotine pRecvMsgFun);

//////////////////////////////////////////////////////////////////////////
CCBELOCK_API void zwPushString(const char *str);
CCBELOCK_API const char * dbgGetLockReturnXML(void);

////////////////////////////////C#��װ����//////////////////////////////////////////
//���ܺ�HIDͨ��,����0����ʧ�ܣ���������ɹ�
CCBELOCK_API int SecboxHidOpen(void);
//�ر��ܺ�HIDͨ��������ΪsecboxHidOpen�ķ���ֵ���
CCBELOCK_API void SecboxHidClose(int handleHid);
//ͨ��HID������Ȩ��֤�����ܺ�
CCBELOCK_API void SendAuthReq2SecBox(int handleHid);
//ͨ��HID�����ܺз�Ӧ����֤���ɹ�����0������ֵ����ʧ��
CCBELOCK_API int VerifyAuthRspFromSecBox(int handleHid);

//д�����ݵ��ܺУ����400���ֽڣ������ʽΪbase64������ַ�������Ҫָ��zwSecboxHidOpen�����ľ�����Լ�������
//�����Ŵ�ԼΪ1-8���ң����廹��Ҫ���Թ�ȷ����
CCBELOCK_API void WriteData2SecretBox(int handleHid,const int index,const char *dataB64);
//ָ���ܺ�HID������Լ������ţ���ȡ�ܺе����ݣ�����base64����������ַ���
CCBELOCK_API const char * ReadDataFromSecretBox(int handleHid,const int index);



#ifdef __cplusplus
}
#endif
#endif // CCBelock_h__
