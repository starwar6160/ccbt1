// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� CCBELOCK_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// CCBELOCK_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef CCBELOCK_EXPORTS
#define CCBELOCK_API __declspec(dllexport)
#else
#define CCBELOCK_API __declspec(dllimport)
#endif

typedef void (cdecl *RecvMsgRotine)(const char *pszMsg);

CCBELOCK_API long	Open(long lTimeOut);
CCBELOCK_API long	Close();
CCBELOCK_API long	Notify(const char *pszMsg);
CCBELOCK_API int	SetRecvMsgRotine(RecvMsgRotine pRecvMsgFun);
