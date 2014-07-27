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
/////////////////////////////////�����ú���/////////////////////////////////////////
CCBELOCK_API int	zwwsTest1(const char *pMsg);
CCBELOCK_API void zwTestWebSocket();

enum JC_CCBELOCK_ERROR_CODE{
	ELOCK_ERROR_SUCCESS=0,		//�����ɹ�
	ELOCK_ERROR_NOTSUPPORT=1,	//��֧�ֵĽӿ�
	ELOCK_ERROR_HARDWAREERROR=2,//Ӳ������
	ELOCK_ERROR_PARAMINVALID=3,	//�����Ƿ�
	ELOCK_ERROR_CONNECTLOST=4,	//ʧȥ�豸����
	ELOCK_ERROR_TIMEOUT=5		//������ʱ
};