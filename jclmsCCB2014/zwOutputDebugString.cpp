#include <windows.h>

void zwOutDebugString(const char *pszStr)
{
	OutputDebugStringA(pszStr);
}
