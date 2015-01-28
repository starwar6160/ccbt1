#ifndef zwTimerHdr_h__
#define zwTimerHdr_h__
#include <windows.h>
class zwTrace1027
{
	char *m_strClass;
	char m_buf[64];
	LARGE_INTEGER nStart,nEnd;
public:
	zwTrace1027(const char *strClassName);
	float DiffTime(void);
	~zwTrace1027();
};

//#define ZWTRC	zwTrace1027 zwtrace(__FUNCTION__);
#define ZWTRC

#endif // zwTimerHdr_h__
