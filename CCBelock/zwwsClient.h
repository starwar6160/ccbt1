#ifndef zwwsClient_h__
#define zwwsClient_h__

void myPrintCurrentTime();

enum zwWebSocket_Error{
	ZWWSERR_SUCCESS,
	ZWWSERR_SERVER_OFFLINE,
	ZWWSERR_NOTCONNECT
};

class zwWebSocket{
	HTTPClientSession cs;
	HTTPRequest request;
	HTTPResponse response;	
	WebSocket *ws;
	const static int RECV_BUF_LEN=1024*16;
	char m_recvBuffer[RECV_BUF_LEN];	
	bool m_connect;
public:	
	zwWebSocket(const char *host,const int port);
	~zwWebSocket();
	int SendString(const string &str);
	int ReceiveString(string &str);
	void wsConnect(void);
	void wsClose(void);
	bool zwSetLongTimeOut(void);
};


#endif // zwwsClient_h__
