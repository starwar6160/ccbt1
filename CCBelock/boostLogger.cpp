#include "boostLogger.h"

void g_InitLog(SeverityLevel file_level, SeverityLevel console_level)
{
	logging::formatter formatter_file =
		expr::stream
		<< "[" << expr::format_date_time(log_timestamp, "%Y-%m-%d %H:%M:%S")
		<< "]" << expr::if_(expr::has_attr(log_uptime))
		[
			expr::stream << " [" << format_date_time(log_uptime, "%O:%M:%S") << "]"
		]
	<< expr::if_(expr::has_attr(log_scope))
		[
			expr::stream << "[" << expr::format_named_scope(log_scope, keywords::format = "%n (%f : %l)") << "]"
		]
	<< "[" << log_threadId << "][" << log_severity << "]" << expr::message;

	logging::formatter formatter_console =
		expr::stream
		<< "[" << expr::format_date_time(log_timestamp, "%H:%M:%S")
		<< "]" << expr::if_(expr::has_attr(log_uptime))
		[
			expr::stream << " [" << format_date_time(log_uptime, "%O:%M:%S") << "]"
		]
	<< "[" << log_severity << "]:" << expr::message;

	logging::add_common_attributes();

	auto console_sink = logging::add_console_log();
	auto file_sink = logging::add_file_log
		(
		keywords::file_name = "JCELOCK%Y-%m-%d_%N.log",      //�ļ���
		keywords::rotation_size = 10 * 1024 * 1024,       //�����ļ����ƴ�С
		keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0)    //ÿ���ؽ�
		);

	file_sink->locked_backend()->set_file_collector(sinks::file::make_collector(
		keywords::target = "logs",        //�ļ�����
		keywords::max_size = 50 * 1024 * 1024,    //�ļ�����ռ���ռ�
		keywords::min_free_space = 100 * 1024 * 1024  //������СԤ���ռ�
		));

	file_sink->set_filter(log_severity >= file_level);   //��־�������
	file_sink->locked_backend()->scan_for_files();

	console_sink->set_formatter(formatter_console);
	console_sink->set_filter(log_severity >= console_level);

	file_sink->set_formatter(formatter_file);
	//file_sink->locked_backend()->auto_flush(true);//����д��־

	//logging::core::get()->add_global_attribute("Scope", attrs::named_scope());
	logging::core::get()->add_global_attribute("ThreadID", attrs::current_thread_id());
	logging::core::get()->add_sink(console_sink);
	logging::core::get()->add_sink(file_sink);
}