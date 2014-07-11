// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件:
//#include <windows.h>

// TODO: 在此处引用程序需要的其他头文件
#include <ctime>
#include <cassert>
#include <string>
#include <vector>
#include <map>
#include <stdint.h>
using std::string;
using std::endl;
using std::vector;
using std::map;


//////////////////////替换windows.h中用到的部分定义开始///////////////////////////////
//如果有新的部分定义，从windows.h中复制过来
typedef int                 BOOL;
#define WINAPI      __stdcall
#define APIENTRY    WINAPI
#define DECLARE_HANDLE(name) struct name##__{int unused;}; typedef struct name##__ *name
DECLARE_HANDLE(HINSTANCE);
typedef HINSTANCE HMODULE;      /* HMODULEs can be used in place of HINSTANCEs */
typedef unsigned long       DWORD;
typedef void *            *LPVOID;	//此处和windef.h不太一样，去掉了过时的far关键字

#define DLL_PROCESS_ATTACH   1    
#define DLL_THREAD_ATTACH    2    
#define DLL_THREAD_DETACH    3    
#define DLL_PROCESS_DETACH   0    

#define FALSE 0
#define TRUE 1

//////////////////////替换windows.h中用到的部分定义结束///////////////////////////////