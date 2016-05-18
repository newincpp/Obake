#include "Event.hh"
#include <iostream>
#include <stdio.h>

Obake::EventsManager::~EventsManager()
{

}

Obake::EventsManager::EventsManager()
{
	Event test;

	/*printf("WUT M8 ABAOUA: %p\n", &EventsManager::ABAOUABAA);
	printf("WUT M8 DZAZEA: %p\n", &EventsManager::DZAZEA);*/
	test.Bind(&EventsManager::ABAOUABAA, this);
	test.Bind(&EventsManager::DZAZEA, this);
	test.Bind(&EventsManager::ABAOUABAA, this);
	//auto blabla = std::bind(&EventsManager::ABAOUABAA, this);
	//std::cout << "BIND: " << &blabla << std::endl;DZAZEA
	//test += std::bind(&EventsManager::DZAZEA, this);

	//test();

	//test -= std::bind(&EventsManager::ABAOUABAA, this);

	//test();
}

void Obake::EventsManager::ABAOUABAA()
{
	std::cout << "HEY" << std::endl;
}

void Obake::EventsManager::DZAZEA()
{
	std::cout << "DAA" << std::endl;
}
