// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� ATMCPY_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// ATMCPY_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef ATMCPY_EXPORTS
#define ATMCPY_API __declspec(dllexport)
#else
#define ATMCPY_API __declspec(dllimport)
#endif

// �����Ǵ� atmcpy.dll ������
class ATMCPY_API Catmcpy {
public:
	Catmcpy(void);
	// TODO: �ڴ�������ķ�����
};

extern ATMCPY_API int natmcpy;

ATMCPY_API int fnatmcpy(void);
