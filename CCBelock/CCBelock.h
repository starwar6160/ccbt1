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

// �����Ǵ� CCBelock.dll ������
class CCBELOCK_API CCCBelock {
public:
	CCCBelock(void);
	// TODO: �ڴ�������ķ�����
};

extern CCBELOCK_API int nCCBelock;

CCBELOCK_API int fnCCBelock(void);
