#include "General/Logger.h"
#include "General/SystemInclude.h"
#include <fstream>
#include <iomanip>

const rv::EventID rv::LogMessageEvent::static_event = rv::unique_event_id();

rv::Logger::Logger(const TimeFormat& format)
	:
	format(format)
{
}

rv::EventListener rv::Logger::CreateListener()
{
	return queue.CreateListener();
}

void rv::Logger::OnLog(const LogEvent& event)
{
}

std::string rv::Logger::Format(const TimeFormat& format, const LogEvent& event)
{
	std::stringstream ss;
	auto t = std::chrono::system_clock::to_time_t(event.time);
	std::tm tm;
	localtime_s(&tm, &t);
	switch (format)
	{
		case RV_TF_TIME:
		{
			ss << std::put_time(&tm, "[%T]\t");
		}
		break;
		case RV_TF_TIME_SHORT:
		{
			ss << std::put_time(&tm, "[%R]\t");
		}
		break;
		case RV_TF_DATE:
		{
			ss << std::put_time(&tm, "[%F]\t");
		}
		break;
		case RV_TF_DATE_TIME:
		{
			ss << std::put_time(&tm, "[%F %T]\t");
		}
		break;
	}

	switch (event.type)
	{
		case RV_MT_INFO:	ss << "[INFO]     "; break;
		case RV_MT_DEBUG:	ss << "[DEBUG]    "; break;
		case RV_MT_WARNING:	ss << "[WARNING]  "; break;
		case RV_MT_ERROR:	ss << "[ERROR]    "; break;
		default:			ss << "[???]      "; break;
	}

	return ss.str();
}

rv::MessageLogger::MessageLogger(const TimeFormat& format, const MessageType& defaultType)
	:
	Logger(format),
	defaultType(defaultType)
{
}

rv::MessageLogger::~MessageLogger()
{
	if (!dumpFileName.empty())
	{
		std::ofstream file(dumpFileName);
		if (file.is_open())
		{
			for (const auto& m : queue)
				file << Format(format, m) << '\n';
		}
	}
}

void rv::MessageLogger::Log(const LogMessageEvent& message)
{
	Logger::Log(message);
	queue.push_back(message);
}

void rv::MessageLogger::Log(const std::string& message)
{
	Log(defaultType, message);
}

void rv::MessageLogger::Log(const MessageType type, const std::string& message)
{
	Log(LogMessageEvent(message, type));
}

std::string rv::MessageLogger::Format(const TimeFormat& format, const LogMessageEvent& event)
{
	return Logger::Format(format, event) + event.message;
}

rv::LogMessageEvent::LogMessageEvent()
	:
	LogEvent(static_event)
{
}

rv::LogMessageEvent::LogMessageEvent(const std::string& message, const MessageType& type)
	:
	LogEvent(static_event, type),
	message(message)
{
}

rv::LogEvent::LogEvent()
	:
	time(std::chrono::system_clock::now())
{
}

rv::LogEvent::LogEvent(const EventID& id, const MessageType& type)
	:
	Event(id),
	time(std::chrono::system_clock::now()),
	type(type)
{
}

rv::DebugLogger rv::debug;

#ifdef RV_DEBUG

rv::DebugLogger::DebugLogger(const std::string& dumpFile)
	:
	MessageLogger(RV_TF_TIME, RV_MT_DEBUG)
{
	dumpFileName = dumpFile;
}

void rv::DebugLogger::OnLog(const LogEvent& message)
{
	if constexpr (sys.windows)
		if (message.isType<LogMessageEvent>())
			OutputDebugString((Format(format, message.cast<LogMessageEvent>()) + '\n').c_str());
}

#else

void rv::DebugLogger::Log(const LogMessageEvent& message)
{
}

void rv::DebugLogger::Log(const std::string& message)
{
}

void rv::DebugLogger::Log(const MessageType type, const std::string& message)
{
}

#endif
