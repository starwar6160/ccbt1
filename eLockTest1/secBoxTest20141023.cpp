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
	//声明一个密盒对象；使用该对象的3个方法来认证，读取，写入，至于Open/Close由该对象内部自动完成；            
	int i=0;
	for (int i = 0; i < 3; i++)
	//while(1)
	{
		i++;
		JcSecBox secBox;

		//sprintf(zwbuf,"Secret Box Open###############################################TestByCPP %04d\n",i);
		//printf(zwbuf);
		//OutputDebugStringA(zwbuf);
		//打开密盒                
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
		//随便用一段比较长的文字经过base64编码形成的下面这段有待写入的base64数据
		//实践中，可以用二进制数据编码之后成为base64字符串写入；
		//第二个参数是索引号，大致上是0到10左右，具体还得和赵工确认
		//第三个参数，也就是数据，大体上可以达到最大400多个字节，具体多少还得和赵工确认
		const char  *myLongB64Str1 = "emhvdXdlaXRlc3RPdXRwdXREZWJ1Z1N0cmluZ0FuZEppbkNodUVMb2NraW5kZXg9MFRvdGFsQmxvY2s9MkN1ckJsb2NrTGVuPTU4U2VkaW5nIERhdGEgQmxvY2sgIzBSZWNldmVkIERhdGEgRnJvbSBKQ0VMb2NrIGlzOg==";
		//通过句柄，索引号，读取密盒数据，返回的也是Base64编码过的字符串，解码后可能是文本，也可能是二进制数据
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