//#include "stdafx.h"
#include <iostream>  
#include <boost/asio.hpp>  
#include <boost/bind.hpp>  
#include "jcSerialPort.h"
using namespace std;  
const int ZW64K=64*1024;
const int ZWMAX_MESSAGE_LEN=1024;	//根据目前的状况，1024字节肯定够了
uint16_t  JCPAK_HEADER=0xAAAA;


extern "C"
{
	int SerialTest905main(void);
	int sptest905a2(void);
};

class jcSerialPortImpl
{
public:
	jcSerialPortImpl(const char *ComPortName);
	~jcSerialPortImpl();
	int SendData(const char *Data,const int MsgLen);
	int RecvData(char *outData,const int outMaxLen,int *outLen);
private:
	boost::asio::io_service m_io;  
	boost::asio::serial_port *m_sp;  
};

jcSerialPort::jcSerialPort(const char *ComPortName)
{
	m_impl=new jcSerialPortImpl(ComPortName);
}

jcSerialPort::~jcSerialPort()
{
	m_impl->~jcSerialPortImpl();
}

int jcSerialPort::SendData(const char *Data,const int MsgLen)
{
	return	m_impl->SendData(Data,MsgLen);	
}

int jcSerialPort::RecvData(char *outData,const int outMaxLen,int *outLen)
{
	return	m_impl->RecvData(outData,outMaxLen,outLen);
}


jcSerialPortImpl::jcSerialPortImpl(const char *ComPortName)
{
	try{
	m_sp=new boost::asio::serial_port(m_io,ComPortName);
	m_sp->set_option(boost::asio::serial_port::baud_rate(38400));  
	m_sp->set_option(boost::asio::serial_port::flow_control());  
	m_sp->set_option(boost::asio::serial_port::parity());  
	m_sp->set_option(boost::asio::serial_port::stop_bits());  
	m_sp->set_option(boost::asio::serial_port::character_size(8));  
	}  
	catch (exception& err)  
	{  
		cout << "Open Serial Port "<<ComPortName<<" Exception Error: " << err.what() << endl;  
		exit(1);
	}  

}

jcSerialPortImpl::~jcSerialPortImpl()
{
	m_sp->close();
	delete m_sp;
}

int jcSerialPortImpl::SendData(const char *Data,const int MsgLen)
{
	assert(NULL!=Data);
	assert(MsgLen>0 && MsgLen<=ZW64K);
	if (NULL==Data || MsgLen<=0 || MsgLen>ZW64K)
	{
		return 1;
	}
	//发出网络字节序的包头，尽管此处0XAAAA无需转换，但还是做了以备通用性，
	//以后可以换用0xAABB之类不对称的2个字节作为包头
	uint16_t jcPakHeaderNet=
		boost::asio::detail::socket_ops::host_to_network_short(JCPAK_HEADER);
	boost::asio::write( *m_sp, 		
		boost::asio::buffer(&jcPakHeaderNet, sizeof(jcPakHeaderNet)));
//////////////////////////////////////////////////////////////////////////
	//增加16bit的定长的长度字段
	uint16_t msgLenShort=MsgLen;
	//长度字段自身转换为网络字节序格式msgLenShortNet后发出去
	uint16_t msgLenShortNet=
		boost::asio::detail::socket_ops::host_to_network_short(msgLenShort);
	boost::asio::write( *m_sp, 		
		boost::asio::buffer(&msgLenShortNet, sizeof(msgLenShortNet)));
//////////////////////////////////////////////////////////////////////////
	//发出数据本体
	boost::asio::write( *m_sp, 
		boost::asio::buffer(Data, msgLenShort));  
	return 0;
}

int jcSerialPortImpl::RecvData( char *outData,const int outMaxLen,int *outLen )
{
	assert(NULL!=outData);
	assert(outMaxLen>0 && outMaxLen<=ZW64K);
	assert(NULL!=outLen);
	if (NULL==outData || NULL==outLen || outMaxLen<=0 || outMaxLen>ZW64K)
	{
		return 2;
	}
	//获得主机字节序的包头，尽管此处0XAAAA无需转换，但还是做了以备通用性，
	//以后可以换用0xAABB之类不对称的2个字节作为包头
	uint16_t jcPakHeaderHost=0;
	boost::asio::read( *m_sp, 		
		boost::asio::buffer(&jcPakHeaderHost, sizeof(jcPakHeaderHost)));
	jcPakHeaderHost=
		boost::asio::detail::socket_ops::network_to_host_short(jcPakHeaderHost);
	if (JCPAK_HEADER!=jcPakHeaderHost)
	{
		//是一个不完整的包，直接放弃
		return 3;
	}

//////////////////////////////////////////////////////////////////////////
	uint16_t msgSize=0;
	//获得16bit整数的定长长度字段，指示后面的消息整体长度，也起到消息分界的作用
	boost::asio::read( *m_sp, 		
		boost::asio::buffer(&msgSize, sizeof(msgSize)));
	msgSize=boost::asio::detail::socket_ops::network_to_host_short(msgSize);
#ifdef _DEBUG
	cout<<"msgSize="<<msgSize<<endl;
#endif // _DEBUG
	*outLen=msgSize;
	if (msgSize>ZWMAX_MESSAGE_LEN)
	{
		//如果长度大于合理大小，一般都是错误
		return 4;
	}
//////////////////////////////////////////////////////////////////////////
	memset(outData,0,outMaxLen-1);
	boost::system::error_code err;  	
		size_t ret = boost::asio::read(*m_sp,
			boost::asio::buffer(outData, msgSize), err); 
		if (err)  
		{  
			cout << "Read Data From Serial Port Error: " << err.message() << endl;  
			return 1;
		}  
		else  
		{  
			outData[ret] = '\0';  
			cout<<outData<<endl;  
		}  
	return 0;
}
//////////////////////////////////////////////////////////////////////////



int sptest905a2(void)
{
//	以下是十六进制的串口调试助手发送数据，开头8个字节是大端结尾的数字50(0x32)的HEX表示，后面是5组"0123456789"的HEX表示
//	000000323031323334353637383930313233343536373839303132333435363738393031323334353637383930313233343536373839

	jcSerialPort jcsp("COM2");
	string msg1="ZhouWei20140905.1112\n";
	jcsp.SendData(msg1.c_str(),msg1.size());
	const int BLEN=500;
	char recvBuf[BLEN+1];
	memset(recvBuf,0,BLEN+1);
	int outLen=0;
	Sleep(5000);
	for (int i=0;i<20;i++)
	{
		memset(recvBuf,0,BLEN);
		jcsp.RecvData(recvBuf, BLEN,&outLen);
		//cout<<"R "<<recvBuf<<endl;
		Sleep(500);
	}
	
	//Sleep(6000);
	//jcsp.RecvData(recvBuf,&outLen);

	return 0;
}

#ifdef _DEBUG_905_SERIALPORT_TEST
int SerialTest905main(void)  
{  
	try  
	{  
		boost::asio::io_service io;  
		boost::asio::serial_port sp(io, "COM2");  

		sp.set_option(boost::asio::serial_port::baud_rate(38400));  
		sp.set_option(boost::asio::serial_port::flow_control());  
		sp.set_option(boost::asio::serial_port::parity());  
		sp.set_option(boost::asio::serial_port::stop_bits());  
		sp.set_option(boost::asio::serial_port::character_size(8));  
		string msg1="ZhouWei20140905.1005\n";
		boost::asio::write(sp, boost::asio::buffer(msg1, msg1.size()));  

		char buf[101];  
		boost::system::error_code err;  
		while (true)  
		{  
			size_t ret = sp.read_some(boost::asio::buffer(buf, 100), err);  
			if (err)  
			{  
				cout << "read_some Error: " << err.message() << endl;  
				break;  
			}  
			else  
			{  
				buf[ret] = '\0';  
				cout << buf;  
			}  
		}  

		io.run();  
	}  
	catch (exception& err)  
	{  
		cout << "Exception Error: " << err.what() << endl;  
	}  


	getchar();  
	return 0;  
}  
#endif // _DEBUG_905_SERIALPORT_TEST

//如果想进行读写超时控制的话,写需要适用异步写的方式,另外加入定时代码:
//[cpp] view plaincopy
//	boost::asio::deadline_timer timer(io);  
//timer.expires_from_now(boost::posix_time::millisec(60000));  
//timer.async_wait(boost::bind(&boost::asio::serial_port::cancel,  &sp));  