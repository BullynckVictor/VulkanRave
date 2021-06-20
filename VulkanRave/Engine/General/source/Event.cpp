#include "General/Event.h"
#include "Utilities/Exception.h"

rv::Event::Event(const ID<Event>& id)
	:
	id(id)
{
}

rv::Event::~Event() noexcept
{
}

rv::EventQueue::EventQueue()
	:
	alive(std::make_shared<bool>(true))
{
}

rv::EventQueue::~EventQueue()
{
	std::lock_guard<std::mutex> guard(mutex);
	queues.clear();
	*alive = false;
}

rv::EventListener rv::EventQueue::Listen()
{
	std::lock_guard<std::mutex> guard(mutex);
	queues.emplace_back();
	EventListener listener;
	listener.parent = *this;
	listener.queue = --queues.end();
	listener.parent_alive = alive;
	return listener;
}

rv::EventListener::EventListener(EventListener&& rhs) noexcept
	:
	queue(rhs.queue),
	parent(std::move(rhs.parent)),
	parent_alive(std::move(rhs.parent_alive))
{
}

rv::EventListener& rv::EventListener::operator=(EventListener&& rhs) noexcept
{
	rv_assert(parent == rhs.parent);
	parent_alive = std::move(rhs.parent_alive);
	if (parent.valid() && *parent_alive.get())
	{
		std::lock_guard<std::mutex> guard(parent.get().mutex);
		parent.get().queues.erase(queue);
	}
	queue = rhs.queue;
	parent = std::move(rhs.parent);
	return *this;
}

rv::EventListener::~EventListener() noexcept
{
	if (parent.valid() && *parent_alive.get())
	{
		std::lock_guard<std::mutex> guard(parent.get().mutex);
		parent.get().queues.erase(queue);
	}
}

std::shared_ptr<const rv::Event> rv::EventListener::Get()
{
	if (queue->empty())
		return nullptr;
	std::lock_guard<std::mutex> guard(parent.get().mutex);
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