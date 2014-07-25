#ifndef zwwsClient_h__
#define zwwsClient_h__

class zwWebSocketClientImpl;

class zwWebSocketClient
{
public:
	//���캯��ֱ�����ӵ�WebSocket��������ָ��������IP�Ͷ˿�
	zwWebSocketClient(const char *serverIP,const int serverPort);
	//�����������ر�WebSocket����
	~zwWebSocketClient();
	//�շ��ı��ַ����������������ҽ��շ��ص��ַ���
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
