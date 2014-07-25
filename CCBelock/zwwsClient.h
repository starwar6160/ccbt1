#ifndef zwwsClient_h__
#define zwwsClient_h__

class zwWebSocketClientImpl;

class zwWebSocketClient
{
public:
	//构造函数直接连接到WebSocket服务器，指定服务器IP和端口
	zwWebSocketClient(const char *serverIP,const int serverPort);
	//析构函数，关闭WebSocket连接
	~zwWebSocketClient();
	//收发文本字符串到服务器，并且接收返回的字符串
	string SendRecvData(const string &message);
private:
	zwWebSocketClientImpl *m_imp;
};

class zwTrace410
{
public:
	zwTrace410(const char *funcName);
	~zwTrace410();
private:
	std::string m_funcname;
};

#endif // zwwsClient_h__
