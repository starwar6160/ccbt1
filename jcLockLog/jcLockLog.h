// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� JCLOCKLOG_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// JCLOCKLOG_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef JCLOCKLOG_EXPORTS
#define JCLOCKLOG_API __declspec(dllexport)
#else
#define JCLOCKLOG_API __declspec(dllimport)
#endif

// �����Ǵ� jcLockLog.dll ������
class JCLOCKLOG_API CjcLockLog {
public:
	CjcLockLog(void);
	// TODO: �ڴ�������ķ�����
};

#ifdef  __cplusplus
extern "C" {
#endif


extern JCLOCKLOG_API int njcLockLog;

JCLOCKLOG_API int fnjcLockLog(void);

#ifdef  __cplusplus
}
#endif
