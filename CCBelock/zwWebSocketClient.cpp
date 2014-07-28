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



typedef struct zwWebSocket{
	HTTPClientSession cs;
	HTTPRequest request;
	HTTPResponse response;
	WebSocket *ws;
}ZWWS;

int myWebSocketInit(ZWWS *pws,const char *host,const int port)
{
	pws->cs.setHost(host);
	pws->cs.setPort(static_cast<unsigned int>(port));
	pws->request.setMethod(HTTPRequest::HTTP_GET);
	pws->request.setURI("/ws");
	pws->ws=new WebSocket(pws->cs,pws->request,pws->response);
	return 0;
}

void zwTestWebSocket()
{
	Poco::Thread::sleep(200);
	ZWWS zws;
	myWebSocketInit(&zws,"localhost",1425);

	char buffer[1024];
	memset(buffer,0,1024);
	int flags=0;
	int n = 0;
	std::string payload("x");

	//·¢ËÍ×Ö·û´®²âÊÔ
	payload = "Hello, world!";
	zws.ws->sendFrame(payload.data(), (int) payload.size());
	n = zws.ws->receiveFrame(buffer, sizeof(buffer), flags);
	//assert (n == payload.size());
	//assert (payload.compare(0, payload.size(), buffer, 0, n) == 0);
	assert (flags == WebSocket::FRAME_TEXT);
	cout<<"RECV728 \t"<<buffer<<endl;

	//ws.shutdown();
	n = zws.ws->receiveFrame(buffer, sizeof(buffer), flags);
	//assert (n == 2);
	assert ((flags & WebSocket::FRAME_OP_BITMASK) == WebSocket::FRAME_OP_CLOSE);
}
