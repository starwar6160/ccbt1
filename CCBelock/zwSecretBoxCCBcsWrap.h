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

	typedef enum jc_secret_box_status_t{
		JC_SECBOX_SUCCESS,
		JC_SECBOX_FAIL

	}JC_SECBOX_STATUS;

	////////////////////////////////C#��װ����//////////////////////////////////////////
	////���ܺз�����֤���󣬷��سɹ����߽��
	//CCBELOCK_API JC_SECBOX_STATUS SecboxAuth(void);
	////д�����ݵ��ܺУ����400���ֽڣ������ʽΪbase64������ַ�������Ҫָ��zwSecboxHidOpen�����ľ�����Լ�������
	////�����Ŵ�ԼΪ1-8���ң����廹��Ҫ���Թ�ȷ����
	//CCBELOCK_API void SecboxWriteData(const int index,const char *dataB64);
	////ָ���ܺ�HID������Լ������ţ���ȡ�ܺе����ݣ�����base64����������ַ���
	//CCBELOCK_API const char * SecboxReadData(const int index);

	CCBELOCK_API class JcSecBox
	{
	public:
		JcSecBox();
		~JcSecBox();
		JC_SECBOX_STATUS SecboxAuth(void);
		void SecboxWriteData(const int index,const char *dataB64);
		const char * SecboxReadData(const int index);
	private:
		int m_hidHandle;
	};
#ifdef __cplusplus
}
#endif
#endif // zwSecretBoxCCBcsWrap_h__