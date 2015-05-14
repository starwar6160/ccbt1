// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�:
#include <windows.h>


// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�

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
#include <glog/logging.h>   // glog ͷ�ļ�

#define ZWINFO(x)  LOG(INFO)<<(x);
#define ZWWARN(x) LOG(WARNING)<<(x);
#define ZWERROR(x) LOG(ERROR)<<(x);
#define ZWFATAL(x) LOG(FATAL)<<(x);
