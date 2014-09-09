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

#include "Poco/Format.h"
#include "Poco/AutoPtr.h"  
#include "Poco/Logger.h"  
#include "Poco/ConsoleChannel.h"  
#include "Poco/LogStream.h"  
#include "Poco/PatternFormatter.h"  
#include "Poco/FileChannel.h"  
#include "Poco/FormattingChannel.h"  
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/Util/IniFileConfiguration.h"

using Poco::Timestamp;

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
using boost::property_tree::ptree;

