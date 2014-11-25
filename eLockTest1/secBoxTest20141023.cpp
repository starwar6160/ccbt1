#include "stdafx.h"
#include <string>
#include <windows.h>
#include "zwSecretBoxCCBcsWrap.h"
using std::string;

void zwSecboxWDXtest20141023(void)
{
	char zwbuf[256];
	memset(zwbuf,0,256);
	const int ZWPAUSE = 1500;
	//����һ���ܺж���ʹ�øö����3����������֤����ȡ��д�룬����Open/Close�ɸö����ڲ��Զ���ɣ�            
	int i=0;
	for (int i = 0; i < 3; i++)
	//while(1)
	{
		i++;
		JcSecBox secBox;

		//sprintf(zwbuf,"Secret Box Open###############################################TestByCPP %04d\n",i);
		//printf(zwbuf);
		//OutputDebugStringA(zwbuf);
		//���ܺ�                
		int status =
			secBox.SecboxAuth();

#ifdef _DEBUG_ZWHIDCOMM
		if (0==status)
		{
			printf("Good Secret Box\n");
		}
#endif // _DEBUG_ZWHIDCOMM
		if (1==status)
		{
			OutputDebugStringA("SECBOX_FAIL");
			printf("SECBOX_FAIL\n");
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
		sprintf(zwbuf,"end of secBox test loop %d\n",i);
		//OutputDebugStringA(zwbuf);
		//printf(zwbuf);
	}
	printf("\n");
}