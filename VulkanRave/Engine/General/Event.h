#pragma once
#include <list>
#include <memory>
#include <mutex>
#include "Utilities/ID.h"
#include "Utilities/Reference.h"

namespace rv
{

	struct Event
	{
		Event() = default;
		Event(const ID<Event>& id);
		virtual ~Event() noexcept;

		template<typename E>
		bool equals() const { return id == E::static_event; }

		template<typename E>
		E& cast() { return *static_cast<E*>(this); }
		template<typename E>
		const E& cast() const { return *static_cast<const E*>(this); }

		template<typename E>
		E* opt_cast() { return equals<E>() ? static_cast<E*>(this) : nullptr; }
		template<typename E>
		const E* opt_cast() const { return equals<E>() ? static_cast<const E*>(this) : nullptr; }

		ID<Event> id;
	};

	using EventID  = ID<Event>;

	namespace detail
	{
		typedef std::list<std::shared_ptr<const Event>> EventQueue;
	}

	class EventQueueBase
	{
	protected:
		std::list<detail::EventQueue> queues;
		std::mutex mutex;
		friend class EventListener;
	};

	class EventListener
	{
	public:
		EventListener() = default;
		EventListener(const EventListener&) = delete;
		EventListener(EventListener&& rhs) noexcept;
		EventListener& operator= (const EventListener&) = delete;
		EventListener& operator= (EventListener&& rhs) noexcept;
		~EventListener() noexcept;

		std::shared_ptr<const Event> Get();

	private:
		std::list<detail::EventQueue>::iterator queue;
		ORef<EventQueueBase> parent;
		std::shared_ptr<const bool> parent_alive;

		friend class EventQueue;
	};

	class EventQueue : public EventQueueBase
	{
	public:
		EventQueue();
		~EventQueue();

		EventListener Listen();
		template<typename E>
		void Push(const E& event)
		{
			if (!queues.empty())
			{
				std::lock_guard<std::mutex> guard(mutex);
				auto ptr = std::make_shared<E>(event);
				for (auto& queue : queues)
					queue.push_back(ptr);
			}
		}
		template<typename E>
		void Push(E&& event)
		{
			if (!queues.empty())
			{
				std::lock_guard<std::mutex> guard(mutex);
				auto ptr = std::make_shared<E>(std::move(event));
				for (auto& queue : queues)
					queue.push_back(ptr);
			}
		}

	private:
		std::shared_ptr<bool> alive;
	};

	class EventQueueInterface
	{
	public:
		EventListener Listen();

	protected:
		EventQueue queue;
	};

	EventID unique_event_id();
}