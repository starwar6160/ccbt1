// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� JCMULTIHID15_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// JCMULTIHID15_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef JCMULTIHID15_EXPORTS
#define JCMULTIHID15_API __declspec(dllexport)
#else
#define JCMULTIHID15_API __declspec(dllimport)
#endif

// �����Ǵ� jcMultiHID15.dll ������
class JCMULTIHID15_API CjcMultiHID15 {
public:
	CjcMultiHID15(void);
	// TODO: �ڴ�������ķ�����
};

extern JCMULTIHID15_API int njcMultiHID15;

JCMULTIHID15_API int fnjcMultiHID15(void);
