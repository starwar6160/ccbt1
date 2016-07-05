


namespace jchidDevice2015{

	class jcHidDevice
	{
	public:
		jcHidDevice();
		~jcHidDevice();
		int OpenJc();
		void CloseJc();
		int SendJson(const char *jcJson);
		int RecvJson(char *recvJson,int bufLen);
		int getConnectStatus();
	protected:

	private:
		JCHID m_jcElock;
		bool m_hidOpened;
		boost::mutex m_jchid_mutex;
#ifdef _USE_FAKEHID_DEV20160705
		deque<string> m_dqMockLock;
#endif // _USE_JCHID_DEV20160705
	};

}	//namespace jchidDevice2015{
