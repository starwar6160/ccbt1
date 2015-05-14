// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件:
#include <windows.h>


// TODO: 在此处引用程序需要的其他头文件

#include <stdio.h>
#include <iostream>
#include <cassert>
#include <vector>
#include <deque>
using std::cout;
using std::cin;
using std::cerr;
using std::endl;
using std::string;
using std::deque;
using std::vector;

#include <boost/thread.hpp> 
#include <boost/thread/mutex.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/exceptions.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/lexical_cast.hpp>  
#include <boost/bind.hpp>  
#include <boost/function.hpp>  
using boost::property_tree::ptree;
#include <map>
#include <vector>
using std::map;
using std::vector;


#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <glog/logging.h>   // glog 头文件

#define ZWINFO(x)  LOG(INFO)<<(x);
#define ZWWARN(x) LOG(WARNING)<<(x);
#define ZWERROR(x) LOG(ERROR)<<(x);
#define ZWFATAL(x) LOG(FATAL)<<(x);
