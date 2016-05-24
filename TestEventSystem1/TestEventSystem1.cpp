#include "TestEventSystem1.hh"

using namespace System;

TestEventSystem1::~TestEventSystem1()
{
}

TestEventSystem1::TestEventSystem1(Obake::Core* core)
{
	_core = core;
	//ASystem::_executionQueue.push_back(std::bind(&TestEventSystem1::registerEvent, this, "TestEvent1"));
	//ASystem::_executionQueue.push_back(std::bind(&TestEventSystem1::executeEvent, this, "TestEvent1"));
	registerEvent("TestEvent1");
}

void TestEventSystem1::registerEvent(std::string eventName)
{
	std::cout << "TestEventSystem1 registerEvent: " << eventName << std::endl;
	_core->eventsManager.bindEvent(eventName, this, &TestEventSystem1::receiveEvent);
}

void TestEventSystem1::executeEvent(std::string eventName)
{
	std::cout << "TestEventSystem1 executeEvent: " << eventName << std::endl;
	_core->eventsManager.executeEvent<void, std::string>(eventName, "TestEventSystem1");
}

void TestEventSystem1::receiveEvent(std::string receiveEventFrom)
{
	std::cout << "TestEventSystem1 receiveEvent From: " << receiveEventFrom << std::endl;
}