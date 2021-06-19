#include "General/Event.h"

rv::Event::Event(const ID<Event>& id)
	:
	id(id)
{
}

rv::Event::~Event() noexcept
{
}

rv::EventListener rv::EventQueue::Listen()
{
	std::lock_guard<std::mutex> guard(mutex);
	queues.emplace_back();
	EventListener listener;
	listener.parent = queues;
	listener.queue = --queues.end();
	listener.mutex = mutex;
	return listener;
}

rv::EventListener::EventListener(EventListener&& rhs) noexcept
	:
	queue(rhs.queue),
	parent(std::move(rhs.parent)),
	mutex(std::move(rhs.mutex))
{
}

rv::EventListener& rv::EventListener::operator=(EventListener&& rhs) noexcept
{
	if (parent.valid())
	{
		std::lock_guard<std::mutex> guard(rhs.mutex);
		parent.get().erase(queue);
	}
	queue = rhs.queue;
	parent = std::move(rhs.parent);
	return *this;
}

rv::EventListener::~EventListener() noexcept
{
	if (parent.valid())
	{
		std::lock_guard<std::mutex> guard(mutex);
		parent.get().erase(queue);
	}
}

std::shared_ptr<const rv::Event> rv::EventListener::Get()
{
	if (queue->empty())
		return nullptr;
	std::lock_guard<std::mutex> guard(mutex);
	auto ret = queue->front();
	queue->pop_front();
	return ret;
}

rv::EventID rv::unique_event_id()
{
    return unique_id<Event>();
}

rv::EventListener rv::EventQueueInterface::Listen()
{
    return queue.Listen();
}