#ifndef zwSecretBoxCCBcsWrap_h__
#define zwSecretBoxCCBcsWrap_h__


#ifdef __cplusplus
extern "C" {
#endif

	////////////////////////////////C#��װ����//////////////////////////////////////////
	//���ܺ�HIDͨ��,����0����ʧ�ܣ���������ɹ�
	int SecboxHidOpen(void);
	//�ر��ܺ�HIDͨ��������ΪsecboxHidOpen�ķ���ֵ���
	void SecboxHidClose(int handleHid);
	//ͨ��HID������Ȩ��֤�����ܺ�
	void SendAuthReq2SecBox(int handleHid);
	//ͨ��HID�����ܺз�Ӧ����֤���ɹ�����0������ֵ����ʧ��
	int VerifyAuthRspFromSecBox(int handleHid);

	//д�����ݵ��ܺУ����400���ֽڣ������ʽΪbase64������ַ�������Ҫָ��zwSecboxHidOpen�����ľ�����Լ�������
	//�����Ŵ�ԼΪ1-8���ң����廹��Ҫ���Թ�ȷ����
	void WriteData2SecretBox(int handleHid,const int index,const char *dataB64);
	//ָ���ܺ�HID������Լ������ţ���ȡ�ܺе����ݣ�����base64����������ַ���
	const char * ReadDataFromSecretBox(int handleHid,const int index);
#ifdef __cplusplus
}
#endif
#endif // zwSecretBoxCCBcsWrap_h__