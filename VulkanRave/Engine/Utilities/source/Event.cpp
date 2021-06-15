#include "Utilities/Event.h"

rv::Event::Event(const ID<Event>& id)
	:
	id(id)
{
}

rv::Event::~Event() noexcept
{
}

rv::EventListener rv::EventQueue::CreateListener()
{
	queues.emplace_back();
	EventListener listener;
	listener.parent = queues;
	listener.queue = --queues.end();
	return listener;
}

rv::EventListener::EventListener(EventListener&& rhs) noexcept
	:
	queue(rhs.queue),
	parent(std::move(rhs.parent))
{
}

rv::EventListener& rv::EventListener::operator=(EventListener&& rhs) noexcept
{
	if (parent.valid())
		parent.get().erase(queue);
	queue = rhs.queue;
	parent = std::move(rhs.parent);
	return *this;
}

rv::EventListener::~EventListener() noexcept
{
	if (parent.valid())
		parent.get().erase(queue);
}

std::shared_ptr<const rv::Event> rv::EventListener::Get()
{
	if (queue->empty())
		return nullptr;
	auto ret = queue->front();
	queue->pop_front();
	return ret;
}

rv::EventID rv::unique_event_id()
{
    return unique_id<Event>();
}
