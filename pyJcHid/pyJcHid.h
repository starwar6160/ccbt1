// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� PYJCHID_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// PYJCHID_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef PYJCHID_EXPORTS
#define PYJCHID_API __declspec(dllexport)
#else
#define PYJCHID_API __declspec(dllimport)
#endif

// �����Ǵ� pyJcHid.dll ������
class PYJCHID_API CpyJcHid {
public:
	CpyJcHid(void);
	// TODO: �ڴ�������ķ�����
};

extern PYJCHID_API int npyJcHid;

PYJCHID_API int fnpyJcHid(void);
