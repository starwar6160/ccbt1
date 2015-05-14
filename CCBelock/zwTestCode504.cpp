#include "stdafx.h"
#include "zwHidComm.h"
#include "zwHidDevClass2015.h"
using jchidDevice2015::jcHidDevice;

namespace zwtest504
{
	void zwtest504hidClass(void)
	{
		const char *msg02="{\"Command\":\"Lock_Now_Info\"}";
		char recvJson[256];
		memset(recvJson,0,256);

		jcHidDevice *jc1=new jcHidDevice();	
		printf("%s\n",__FUNCTION__);
		jc1->SendJson(	msg02);
		jc1->CloseJc();
		jc1->OpenJc();
		//jc1->OpenJc();
		//jc1->SendJson(	msg02);
		jc1->RecvJson(recvJson,256);
		ZWWARN(recvJson)
			Sleep(2000);

		//jc1->OpenJc();

		//Sleep(3000);
		//memset(recvJson,0,256);
		//jc1->RecvJson(recvJson,256);
		//ZWWARN(recvJson)
		//jc1->CloseJc();

		//jcHidDevice *jc2=new jcHidDevice();
		//memset(recvJson,0,256);
		//jc2->SendJson(	msg02);
		//jc2->RecvJson(recvJson,256);
		//ZWWARN(recvJson)
	}

	void zwtest514deque1(void)
	{
		deque<string> dqStr;
		cout<<"dqStr empty size is "<<dqStr.size()<<endl;
		dqStr.push_back("dqstr Line1");
		dqStr.push_back("dqstr Line2");
		dqStr.push_back("dqstr Line3");
		cout<<"dqStr after push 3 item size is "<<dqStr.size()<<endl;
		cout<<"deque front item is "<<dqStr.front()<<endl;
		dqStr.pop_front();
		cout<<"dqStr after pope 3 item size is "<<dqStr.size()<<endl;
		dqStr.clear();
		cout<<"dqStr after clear size is "<<dqStr.size()<<endl;

	}
}
