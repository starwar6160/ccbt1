#include "stdafx.h"
#include "zwwsClient.h"
#include "zwCcbElockHdr.h"
#include "zwPocoLog.h"
using Poco::Net::ConnectionRefusedException;

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
				ZWTRACE("CPPEXECPTION804B");
				ZWTRACE(__FUNCTION__);
				ZWFATAL("webSocket类连接服务器异常")
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
////////////////////////////////////以下是我自己的代码部分//////////////////////////////////////
 using Poco::Net::NetException;

zwWebSocket::zwWebSocket(const char *host,const int port)
{	
	assert(host!=NULL && strlen(host)>0 && port>0 && port<(256*256));
	cs.setHost(host);
	cs.setPort(static_cast<unsigned int>(port));
	request.setMethod(HTTPRequest::HTTP_GET);
	request.setURI("/");
	request.setVersion("1.1");
	ws=NULL;
	m_connect=false;
}

void zwWebSocket::wsConnect(void)
{
	try
	{
		if (false==m_connect)
		{
			ws=new WebSocket(cs,request,response);	
			m_connect=true;
		}
	}
	catch (...)
	{
		OutputDebugStringA("JC WebSocket Connect Failed 20140813.1955!");
		throw;
	}
		
}

void zwWebSocket::wsClose(void)
{
	if (NULL!=ws)
	{
		ws->shutdown();
		delete ws;
		ws=NULL;
		m_connect=false;
	}
}

zwWebSocket::~zwWebSocket()
{
	if (NULL!=ws)
	{
		ws->shutdown();
		delete ws;
		ws=NULL;
		m_connect=false;
	}
}

//设置接收超时
bool zwWebSocket::zwSetLongTimeOut()
{
	//设置一整天的超时时间
	ws->setReceiveTimeout(Poco::Timespan(1,0,0,0,0));
	Poco::Timespan rtm2=ws->getReceiveTimeout();
	cout<<"RecvTimeOut2 Seconds="<<rtm2.totalSeconds()<<endl;
	return m_connect;
}

int zwWebSocket::SendString(const string &str)
{
	assert(str.length()>0);
	if (NULL==ws)
	{
		throw ZWWSERR_NOTCONNECT;
	}
	int flags=ws->sendFrame(str.data(),str.length(),WebSocket::FRAME_TEXT);
	return flags;
}

int zwWebSocket::ReceiveString(string &str)
{
	if (NULL==ws)
	{
		throw ZWWSERR_NOTCONNECT;
	}
	int flags=0;
	memset(m_recvBuffer,0,RECV_BUF_LEN);
	ws->receiveFrame(m_recvBuffer,RECV_BUF_LEN,flags);
	str=m_recvBuffer;
	assert(str.length()>0);
	return flags;
}

//////////////////////////////////////////////////////////////////////////


