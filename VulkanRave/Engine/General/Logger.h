#pragma once
#include "Utilities/Event.h"
#include "General/System.h"
#include <sstream>
#include <chrono>
#include <type_traits>
#include <deque>

namespace rv
{
	enum MessageType
	{
		RV_MT_INFO = 0,
		RV_MT_DEBUG,
		RV_MT_WARNING,
		RV_MT_ERROR
	};

	struct LogEvent : public Event
	{
		LogEvent();
		LogEvent(const EventID& id, const MessageType& type = RV_MT_INFO);

		std::chrono::system_clock::time_point time;
		MessageType type = RV_MT_INFO;
	};

	struct LogMessageEvent : public LogEvent
	{
		static const EventID static_event;

		LogMessageEvent();
		LogMessageEvent(const std::string& message, const MessageType& type = RV_MT_INFO);

		std::string message;
	};

	enum TimeFormat
	{
		RV_TF_NONE = 0,
		RV_TF_TIME,
		RV_TF_TIME_SHORT,
		RV_TF_DATE,
		RV_TF_DATE_TIME,
	};

	class Logger 
	{
	public:
		Logger(const TimeFormat& format = RV_TF_TIME);
		
		EventListener CreateListener();

		template<typename L>
		void Log(const L& event) { static_assert(std::is_base_of_v<LogEvent, L>); OnLog(event); queue.Push(event); }

		TimeFormat format;

	protected:
		virtual void OnLog(const LogEvent& event);
		static std::string Format(const TimeFormat& format, const LogEvent& event);

	private:
		EventQueue queue;
	};

	class MessageLogger : public Logger
	{
	public:
		MessageLogger(const TimeFormat& format = RV_TF_TIME, const MessageType& defaultType = RV_MT_INFO);
		~MessageLogger();

		void Log(const LogMessageEvent& message);
		void Log(const std::string& message);
		void Log(const MessageType type, const std::string& message);

		std::string dumpFileName;
		MessageType defaultType;

	protected:
		static std::string Format(const TimeFormat& format, const LogMessageEvent& event);
		std::deque<LogMessageEvent> queue;
	};

	#ifdef RV_DEBUG

	class DebugLogger : public MessageLogger
	{
	public:
		DebugLogger(const std::string& dumpFile = "Engine/log.txt");

	private:
		void OnLog(const LogEvent& message) override;
	};

	#else

	class DebugLogger : public MessageLogger
	{
	public:
		void Log(const LogMessageEvent& message);
		void Log(const std::string& message);
		void Log(const MessageType type, const std::string& message);
	};

	#endif

	extern DebugLogger debug;
}