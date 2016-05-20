#include "TestEventSystem2.hh"

using namespace System;

TestEventSystem2::~TestEventSystem2()
{
}

TestEventSystem2::TestEventSystem2(Obake::Core* core)
{
	_core = core;
	//ASystem::_executionQueue.push_back(std::bind(&TestEventSystem2::registerEvent, this, "TestEvent2"));
	//ASystem::_executionQueue.push_back(std::bind(&TestEventSystem2::executeEvent, this, "TestEvent1"));
	executeEvent("TestEvent1");
}

void TestEventSystem2::registerEvent(std::string eventName)
{
	std::cout << "TestEventSystem2 registerEvent: " << eventName << std::endl << std::endl;
	_core->eventsManager.bindEvent(eventName, this, &TestEventSystem2::receiveEvent);
}

void TestEventSystem2::executeEvent(std::string eventName)
{
	std::cout << "TestEventSystem2 executeEvent: " << eventName << std::endl << std::endl;
	_core->eventsManager.executeEvent<void, std::string>(eventName, "TestEventSystem2");
}

void TestEventSystem2::receiveEvent(std::string receiveEventFrom)
{
	std::cout << "TestEventSystem2 receiveEvent From: " << receiveEventFrom << std::endl << std::endl;
}