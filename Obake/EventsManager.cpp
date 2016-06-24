#include "EventsManager.hh"

Obake::EventsManager::~EventsManager()
{
	reset();
}

Obake::EventsManager::EventsManager()
{
	/*Event<void, int> event1;

	event1.bind(this, &EventsManager::event1test1);

	event1(5);

	bindEvent("TOTO", this, &EventsManager::event1test1);

	Event<void, int>* event = getEvent<void, int>("TOTO");
	(*event)(4242);

	executeEvent<void, int>("TOTO", 4242);*/
}

void Obake::EventsManager::reset()
{
	for (decltype(_events)::value_type event : _events)
	{
		delete event.second;
	}
}