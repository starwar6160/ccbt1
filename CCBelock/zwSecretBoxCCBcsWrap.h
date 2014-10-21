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

	////////////////////////////////C#封装函数//////////////////////////////////////////
	////向密盒发送认证请求，返回成功或者结果
	//CCBELOCK_API JC_SECBOX_STATUS SecboxAuth(void);
	////写入数据到密盒，最大400多字节，输入格式为base64编码的字符串。需要指定zwSecboxHidOpen给出的句柄，以及索引号
	////索引号大约为1-8左右，具体还需要和赵工确定；
	//CCBELOCK_API void SecboxWriteData(const int index,const char *dataB64);
	////指定密盒HID句柄，以及索引号，读取密盒的数据，返回base64编码的数据字符串
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