#ifndef jcSerialPort_h__
#define jcSerialPort_h__
class jcSerialPortImpl;

class jcSerialPort
{
public:
	jcSerialPort(const char *ComPortName);
	~jcSerialPort();
	int SendData(const char *Data,const int MsgLen);
	int RecvData(char *outData,const int outMaxLen,int *outLen);
private:
	jcSerialPortImpl *m_impl;
};

#endif // jcSerialPort_h__