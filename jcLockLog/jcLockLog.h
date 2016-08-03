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

#ifdef  __cplusplus
extern "C" {
#endif

//��Կ����־����
//��Կ����־����ص�������
//TouchKeyLogItem��HEX��ʽԭ�ģ�TouchKeyLogFactor�����������ɶ��ַ�����
typedef void (CDECL *ReturnTouchKeyLog)(char* TouchKeyLogItem, char* TouchKeyLogFactor);
//�ֽ��Կ����־�ĺ�����(����ֵ��ʲô���壿)
//�ú���ͨ������Ļص��������ش�����
JCLOCKLOG_API int SwapTouchKeyLog(char * TouchKeyLogItem);
//���ûص������ĺ�����
JCLOCKLOG_API void SetReturnTouchKeyLog(ReturnTouchKeyLog _TouchKeyLogHandleFun);


//��Կ����־����
typedef void (CDECL *ReturnLockLog)(char* LockLogItem,char* LockLogEventTime,char* LockLogFactor);
JCLOCKLOG_API void SetReturnLockLog(ReturnLockLog _LockLogHandleFun);
JCLOCKLOG_API int SwapLockLog(char * LockLogItem,int IsFinish);



#ifdef  __cplusplus
}
#endif
