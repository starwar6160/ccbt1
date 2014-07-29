#ifndef zwwsClient_h__
#define zwwsClient_h__

void myPrintCurrentTime();

class zwWebSocket{
	HTTPClientSession cs;
	HTTPRequest request;
	HTTPResponse response;	
	WebSocket *ws;
	const static int RECV_BUF_LEN=1024;
	char m_recvBuffer[RECV_BUF_LEN];	

public:	
	zwWebSocket(const char *host,const int port);
	~zwWebSocket();
	int SendString(const string &str);
	int ReceiveString(string &str);
	void wsConnect(void);
	void wsClose(void);
};


#endif // zwwsClient_h__
