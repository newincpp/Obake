#include "Event.hh"
#include <iostream>
#include <stdio.h>

Obake::EventsManager::~EventsManager()
{

}

Obake::EventsManager::EventsManager()
{
	Event<void, int> event1;
	Event<void, std::string, int> event2;

	event1.bind(this, &EventsManager::event1test1);
	event1.bind(this, &EventsManager::event1test2);
	event1.bind(this, &EventsManager::event1test1);

	event1(5);

	event2.bind(this, &EventsManager::event2test1);
	event2.bind(this, &EventsManager::event2test2);
	event2.bind(this, &EventsManager::event2test1);
	event2.bind(this, &EventsManager::event2test2);
	event2("toto", 5);
	event2("HEY", 2);
	event2("HOHO", 3);
}

void Obake::EventsManager::event1test1(int print)
{
	std::cout << "event1test1: " << print << std::endl;
}

void Obake::EventsManager::event1test2(int print)
{
	std::cout << "event1test2: " << print << std::endl;
}

void Obake::EventsManager::event2test1(std::string printStr, int param2)
{
	std::cout << "event2test1: " << printStr << " | param2: " << param2 << std::endl;
}

void Obake::EventsManager::event2test2(std::string printStr, int param2)
{
	std::cout << "event2test2: " << printStr << " | param2: " << param2 << std::endl;
}
