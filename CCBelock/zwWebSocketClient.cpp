#include "stdafx.h"

namespace{
	class WebSocketRequestHandler: public Poco::Net::HTTPRequestHandler
	{
	public:
		WebSocketRequestHandler(std::size_t bufSize = 1024): _bufSize(bufSize)
		{
		}

		void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
		{
			try
			{
				WebSocket ws(request, response);
				std::auto_ptr<char> pBuffer(new char[_bufSize]);
				int flags;
				int n;
				do
				{
					n = ws.receiveFrame(pBuffer.get(), _bufSize, flags);
					ws.sendFrame(pBuffer.get(), n, flags);
				}
				while (n > 0 || (flags & WebSocket::FRAME_OP_BITMASK) != WebSocket::FRAME_OP_CLOSE);
			}
			catch (WebSocketException& exc)
			{
				switch (exc.code())
				{
				case WebSocket::WS_ERR_HANDSHAKE_UNSUPPORTED_VERSION:
					response.set("Sec-WebSocket-Version", WebSocket::WEBSOCKET_VERSION);
					// fallthrough
				case WebSocket::WS_ERR_NO_HANDSHAKE:
				case WebSocket::WS_ERR_HANDSHAKE_NO_VERSION:
				case WebSocket::WS_ERR_HANDSHAKE_NO_KEY:
					response.setStatusAndReason(HTTPResponse::HTTP_BAD_REQUEST);
					response.setContentLength(0);
					response.send();
					break;
				}
			}
		}

	private:
		std::size_t _bufSize;
	};

	class WebSocketRequestHandlerFactory: public Poco::Net::HTTPRequestHandlerFactory
	{
	public:
		WebSocketRequestHandlerFactory(std::size_t bufSize = 1024): _bufSize(bufSize)
		{
		}

		Poco::Net::HTTPRequestHandler* createRequestHandler(const HTTPServerRequest& request)
		{
			return new WebSocketRequestHandler(_bufSize);
		}

	private:
		std::size_t _bufSize;
	};
}
////////////////////////////////////���������Լ��Ĵ��벿��//////////////////////////////////////

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



zwWebSocket::zwWebSocket(const char *host,const int port)
{	
	cs.setHost(host);
	cs.setPort(static_cast<unsigned int>(port));
	request.setMethod(HTTPRequest::HTTP_GET);
	request.setURI("/ws");
	ws=NULL;
	
}

void zwWebSocket::wsConnect(void)
{
	ws=new WebSocket(cs,request,response);	
}

void zwWebSocket::wsClose(void)
{
	if (NULL!=ws)
	{
		ws->shutdown();
		delete ws;
		ws=NULL;
	}
}

zwWebSocket::~zwWebSocket()
{
	if (NULL!=ws)
	{
		ws->shutdown();
		delete ws;
	}
}

int zwWebSocket::SendString(const string &str)
{
	int flags=ws->sendFrame(str.data(),str.length(),WebSocket::FRAME_TEXT);
	return flags;
}

int zwWebSocket::ReceiveString(string &str)
{
	int flags=0;
	memset(m_recvBuffer,0,RECV_BUF_LEN);
	ws->receiveFrame(m_recvBuffer,RECV_BUF_LEN,flags);
	str=m_recvBuffer;
	return flags;
}

//////////////////////////////////////////////////////////////////////////

zwWebSocket zwc("localhost",1425);	
void zwTestWebSocket()
{
	try
	{
	Poco::Thread::sleep(200);

	char buffer[1024];
	memset(buffer,0,1024);
	int flags=0;
	int n = 0;
	string strSend,strRecv;
	myPrintCurrentTime();
	//�����ַ�������
	strSend = "Hello, world! ZWTESTSTR2014.0729.1116";
	zwc.wsConnect();
for (int i=0;i<5;i++)
{
	zwc.SendString(strSend);
	cout<<"SEND729 \t"<<strSend<<endl;
	zwc.ReceiveString(strRecv);
	cout<<"RECV729 \t"<<strRecv<<endl;
	Poco::Thread::sleep(2000);
}
zwc.wsClose();
	}
	catch (...)
	{
		printf("CPP EXECEPTION!");
		myPrintCurrentTime();
	}

}

void myPrintCurrentTime()
{
	time_t now=time(NULL);
	char buf[256];
	memset(buf,0,256);
	struct tm *tm_ptr=localtime(&now);
	
	printf("TIME=\t%d:%d:%d\n",tm_ptr->tm_hour,tm_ptr->tm_min,tm_ptr->tm_sec);
}
