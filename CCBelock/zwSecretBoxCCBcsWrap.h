#ifndef zwSecretBoxCCBcsWrap_h__
#define zwSecretBoxCCBcsWrap_h__

#ifdef _ZWUSE_AS_JNI
#define CCBELOCK_API
#else
#ifdef CCBELOCK_EXPORTS
#define CCBELOCK_API __declspec(dllexport) 
#else
#define CCBELOCK_API __declspec(dllimport)
#endif
#endif


#ifdef __cplusplus
extern "C" {
#endif

	////////////////////////////////C#��װ����//////////////////////////////////////////
	//���ܺ�HIDͨ��,����0����ʧ�ܣ���������ɹ�
	CCBELOCK_API int SecboxOpen(void);
	//�ر��ܺ�HIDͨ��������ΪsecboxHidOpen�ķ���ֵ���
	CCBELOCK_API void SecboxClose(int handleHid);
	//ͨ��HID������Ȩ��֤�����ܺ�
	CCBELOCK_API void SecboxSendAuthReq(int handleHid);
	//ͨ��HID�����ܺз�Ӧ����֤���ɹ�����0������ֵ����ʧ��
	CCBELOCK_API int SecboxVerifyAuthRsp(int handleHid);

	//д�����ݵ��ܺУ����400���ֽڣ������ʽΪbase64������ַ�������Ҫָ��zwSecboxHidOpen�����ľ�����Լ�������
	//�����Ŵ�ԼΪ1-8���ң����廹��Ҫ���Թ�ȷ����
	CCBELOCK_API void SecboxWriteData(int handleHid,const int index,const char *dataB64);
	//ָ���ܺ�HID������Լ������ţ���ȡ�ܺе����ݣ�����base64����������ַ���
	CCBELOCK_API const char * SecboxReadData(int handleHid,const int index);


#ifdef __cplusplus
}
#endif
#endif // zwSecretBoxCCBcsWrap_h__