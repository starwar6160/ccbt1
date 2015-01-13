#include "stdafx.h"

#include <Windows.h>
#include <tchar.h>
#include <Dbt.h>
#include <setupapi.h>
#include <iostream>
#include <atlstr.h> // CString
using namespace std;
#pragma comment (lib, "Kernel32.lib")
#pragma comment (lib, "User32.lib")
#define THRD_MESSAGE_EXIT WM_USER + 1
const _TCHAR CLASS_NAME[]  = _T("Sample Window Class");
HWND hWnd;
static const GUID GUID_DEVINTERFACE_LIST[] =
{
	// GUID_DEVINTERFACE_USB_DEVICE
	{ 0xA5DCBF10, 0x6530, 0x11D2, { 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED } },
	// GUID_DEVINTERFACE_DISK
	{ 0x53f56307, 0xb6bf, 0x11d0, { 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b } },
	// GUID_DEVINTERFACE_HID,
	{ 0x4D1E55B2, 0xF16F, 0x11CF, { 0x88, 0xCB, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30 } },
	// GUID_NDIS_LAN_CLASS
	{ 0xad498944, 0x762f, 0x11d0, { 0x8d, 0xcb, 0x00, 0xc0, 0x4f, 0xc3, 0x35, 0x8c } }
	//// GUID_DEVINTERFACE_COMPORT
	//{ 0x86e0d1e0, 0x8089, 0x11d0, { 0x9c, 0xe4, 0x08, 0x00, 0x3e, 0x30, 0x1f, 0x73 } },
	//// GUID_DEVINTERFACE_SERENUM_BUS_ENUMERATOR
	//{ 0x4D36E978, 0xE325, 0x11CE, { 0xBF, 0xC1, 0x08, 0x00, 0x2B, 0xE1, 0x03, 0x18 } },
	//// GUID_DEVINTERFACE_PARALLEL
	//{ 0x97F76EF0, 0xF883, 0x11D0, { 0xAF, 0x1F, 0x00, 0x00, 0xF8, 0x00, 0x84, 0x5C } },
	//// GUID_DEVINTERFACE_PARCLASS
	//{ 0x811FC6A5, 0xF728, 0x11D0, { 0xA5, 0x37, 0x00, 0x00, 0xF8, 0x75, 0x3E, 0xD1 } }
};

void myUSBHidStringExtract113( char * dbcc_name )
{
	char *usbDevId=dbcc_name+4;
	char *idxTok1=strchr(usbDevId,'#');
	char *idxTok2=strchr(idxTok1+1,'#');
	char *idxTok3=strchr(idxTok2+1,'#');
	const int BLEN=20;
	char jcDevType[BLEN];
	char jcVid[BLEN],jcPid[BLEN],jcSerial[BLEN];
	memset(jcDevType,0,BLEN);
	memset(jcVid,0,BLEN);
	memset(jcPid,0,BLEN);
	memset(jcSerial,0,BLEN);
	strncpy(jcDevType,usbDevId,idxTok1-usbDevId);
	char *idxPid=strchr(idxTok1+1,'&');
	strncpy(jcVid,idxTok1+1,idxPid-idxTok1-1);		
	strncpy(jcPid,idxPid+1,idxTok2-idxPid-1);
	strncpy(jcSerial,idxTok2+1,idxTok3-idxTok2-1);
	printf("jcDevType=%s\t",jcDevType);
	printf("jcVid=%s\t",jcVid);
	printf("jcPid=%s\t",jcPid);
	printf("jcSerial=%s\n",jcSerial);
}

static void TcharToChar (const TCHAR * tchar, char * _char)  
{  
	int iLength ;  
	//获取字节长度   
	iLength = WideCharToMultiByte(CP_ACP, 0, tchar, -1, NULL, 0, NULL, NULL);  
	//将tchar值赋给_char    
	WideCharToMultiByte(CP_ACP, 0, tchar, -1, _char, iLength, NULL, NULL);   
}  

void UpdateDevice(PDEV_BROADCAST_DEVICEINTERFACE pDevInf, WPARAM wParam)
{
	char dbccName[128];
	memset(dbccName,0,128);
	TcharToChar(&pDevInf->dbcc_name[0],dbccName);
	myUSBHidStringExtract113(dbccName);
	return;
#ifdef _DEBUG113A1
	//pDevInf->dbcc_name	0x009bdf8c "\\?\HID#VID_0483&PID_5710#7&15ac344f&0&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}"	wchar_t [1]
	CString szDevId = pDevInf->dbcc_name + 4;
	//szDevId=HID#VID_0483&PID_5710#7&15ac344f&0&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}	
	int idx = szDevId.ReverseFind(_T('#'));	//idx=0x27
	szDevId.Truncate(idx);	
	//HID#VID_0483&PID_5710#7&15ac344f&0&0000
	szDevId.Replace(_T('#'), _T('\\'));
	//HID\VID_0483&PID_5710\7&15ac344f&0&0000
	szDevId.MakeUpper();
	//HID\VID_0483&PID_5710\7&15AC344F&0&0000
	CString szClass;
	idx = szDevId.Find(_T('\\'));	//idx=0x03
	szClass = szDevId.Left(idx);	//szClass=HID
	CString szTmp;
	if ( DBT_DEVICEARRIVAL == wParam ) \
		szTmp.Format(_T("Adding %s\r\n"), szDevId.GetBuffer());
	else
		szTmp.Format(_T("Removing %s\r\n"), szDevId.GetBuffer());
	//Removing HID\VID_0483&PID_5710\7&15AC344F&0&0000
	_tprintf(szTmp);
#endif // _DEBUG113A1


}
LRESULT DeviceChange(UINT message, WPARAM wParam, LPARAM lParam)
{
	if ( DBT_DEVICEARRIVAL == wParam || DBT_DEVICEREMOVECOMPLETE == wParam )
	{
		PDEV_BROADCAST_HDR pHdr = (PDEV_BROADCAST_HDR)lParam;
		PDEV_BROADCAST_DEVICEINTERFACE pDevInf;
		PDEV_BROADCAST_HANDLE pDevHnd;
		PDEV_BROADCAST_OEM pDevOem;
		PDEV_BROADCAST_PORT pDevPort;
		PDEV_BROADCAST_VOLUME pDevVolume;
		switch( pHdr->dbch_devicetype )
		{
		case DBT_DEVTYP_DEVICEINTERFACE:
			pDevInf = (PDEV_BROADCAST_DEVICEINTERFACE)pHdr;
			UpdateDevice(pDevInf, wParam);
			break;
		case DBT_DEVTYP_HANDLE:
			pDevHnd = (PDEV_BROADCAST_HANDLE)pHdr;
			break;
		case DBT_DEVTYP_OEM:
			pDevOem = (PDEV_BROADCAST_OEM)pHdr;
			break;
		case DBT_DEVTYP_PORT:
			pDevPort = (PDEV_BROADCAST_PORT)pHdr;
			break;
		case DBT_DEVTYP_VOLUME:
			pDevVolume = (PDEV_BROADCAST_VOLUME)pHdr;
			break;
		}
	}
	return 0;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_PAINT:
		break;
	case WM_SIZE:
		break;
	case WM_DEVICECHANGE:
		return DeviceChange(message, wParam, lParam);
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}
ATOM MyRegisterClass()
{
	WNDCLASS wc = {0};
	wc.lpfnWndProc   = WndProc;
	wc.hInstance     = GetModuleHandle(NULL);
	wc.lpszClassName = CLASS_NAME;
	return RegisterClass(&wc);
}
bool CreateMessageOnlyWindow()
{
	hWnd = CreateWindowEx(0, CLASS_NAME, _T(""), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL,       // Parent window   
		NULL,       // Menu
		GetModuleHandle(NULL),  // Instance handle
		NULL        // Additional application data
		);
	return hWnd != NULL;
}
void RegisterDeviceNotify()
{
	HDEVNOTIFY hDevNotify;
	for (int i = 0; i < sizeof(GUID_DEVINTERFACE_LIST) / sizeof(GUID); i++)
	{
		DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
		ZeroMemory( &NotificationFilter, sizeof(NotificationFilter) );
		NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
		NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
		NotificationFilter.dbcc_classguid = GUID_DEVINTERFACE_LIST[i];
		hDevNotify = RegisterDeviceNotification(hWnd, &NotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE);
	}
}
DWORD WINAPI zwHidDevPlugDetectThrdFunc( LPVOID lpParam )
{
	if (0 == MyRegisterClass())
		return -1;
	if (!CreateMessageOnlyWindow())
		return -1;
	RegisterDeviceNotify();
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (msg.message == THRD_MESSAGE_EXIT)
		{
			cout << "worker receive the exiting Message..." << endl;
			return 0;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}

int zwStartHidDevPlugThread(void)
{
	//防止重复启动多个USB设备拔插检测线程
	static bool G_JCHID_DEVPLUG_THR_RUNNING=false;
	if (true==G_JCHID_DEVPLUG_THR_RUNNING)
	{
		return 0;
	}
	DWORD iThread=0;
	HANDLE hThread = CreateThread( NULL, 0, zwHidDevPlugDetectThrdFunc, NULL, 0, &iThread);
	if (hThread == NULL) {
		cout << "start thread zwHidDevPlugDetectThrdFunc error" << endl;
		return -1;
	}
	else
	{
		G_JCHID_DEVPLUG_THR_RUNNING=true;
		return 0;
	}
}

int zwUsbPlugInOutTest(void)
{
	DWORD iThread;
	HANDLE hThread = CreateThread( NULL, 0, zwHidDevPlugDetectThrdFunc, NULL, 0, &iThread);
	if (hThread == NULL) {
		cout << "error" << endl;
		return -1;
	}
	char chQtNum;
	do
	{
		cout << "enter Q/q for quit: " << endl;
		cin >> chQtNum;
	} while (chQtNum != 'Q' && chQtNum != 'q');
	PostThreadMessage(iThread, THRD_MESSAGE_EXIT, 0, 0);
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	return 0;
}
