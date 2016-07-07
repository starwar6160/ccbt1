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
CCBELOCK_API void myLuaBridgeSendJsonAPI(const char *luaScriptFile);

#ifdef __cplusplus
}
#endif

#define _USE_FAKEHID_DEV20160705	//�����Ƿ�ʹ��FAKE�ĺ�����������

#endif // CCBelock_h__
