#include "stdafx.h"
#include "zwSecretBoxCCBcsWrap.h"

void zwSecboxWDXtest20141023(void)
{
	const int ZWPAUSE = 1500;
	//����һ���ܺж���ʹ�øö����3����������֤����ȡ��д�룬����Open/Close�ɸö����ڲ��Զ���ɣ�            

	//for (int i = 0; i < 2; i++)
	while(true)
	{
		JcSecBox secBox;
		printf("Secret Box Open###############################################TestByCPP\n");
		//���ܺ�                
		int status =
			secBox.SecboxAuth();

		if (0==status)
		{
			printf("Good Secret Box\n");
		}
		if (1==status)
		{
			printf("Fake Secret Box\n");
			continue;
		}
		//////////////////////////////////////////////////////////
		//�����һ�αȽϳ������־���base64�����γɵ���������д�д���base64����
		//ʵ���У������ö��������ݱ���֮���Ϊbase64�ַ���д�룻
		//�ڶ��������������ţ���������0��10���ң����廹�ú��Թ�ȷ��
		//������������Ҳ�������ݣ������Ͽ��Դﵽ���400����ֽڣ�������ٻ��ú��Թ�ȷ��
		const char  *myLongB64Str1 = "emhvdXdlaXRlc3RPdXRwdXREZWJ1Z1N0cmluZ0FuZEppbkNodUVMb2NraW5kZXg9MFRvdGFsQmxvY2s9MkN1ckJsb2NrTGVuPTU4U2VkaW5nIERhdGEgQmxvY2sgIzBSZWNldmVkIERhdGEgRnJvbSBKQ0VMb2NrIGlzOg==";
		//ͨ������������ţ���ȡ�ܺ����ݣ����ص�Ҳ��Base64��������ַ����������������ı���Ҳ�����Ƕ���������
		//Console.Out.WriteLine("Secret Box WriteData");
		secBox.SecboxWriteData(1, myLongB64Str1);
		//Console.Out.WriteLine("Secret Box ReadData");
		std::string recvFromSecBox = secBox.SecboxReadData(1);
		//Console.Out.WriteLine("WAIT 4 SECONDS FOR PLUG OUT/IN SECRET BOX");
		//System.Threading.Thread.Sleep(ZWPAUSE*5);
	}

}