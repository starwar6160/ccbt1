#ifndef jcHidDevice_h__
#define jcHidDevice_h__

namespace jchidDevice2015{

	class jcHidDevice
	{
	public:
		jcHidDevice();
		~jcHidDevice();
		int OpenJc();
		void CloseJc();
		int SendJson(const char *jcJson);
		JCHID_STATUS RecvJson(char *recvJson,int bufLen);
		int getConnectStatus();
	protected:

	private:
		JCHID m_jcElock;
		bool m_hidOpened;
		boost::mutex m_jchid_mutex;
	};

}	//namespace jchidDevice2015{
#endif // jcHidDevice_h__
