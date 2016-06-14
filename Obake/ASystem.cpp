#include "Core.hh"
#include "ASystem.hh"
#include <iostream>

Obake::ASystem::ASystem()
	: _beginLoop(_executionQueue.end())
{
    //_executionQueue.reserve(32);
	//_executionQueue.push_back(std::bind(&Obake::ASystem::empty, this));
	std::cout << "CONSTRUCT SYSTEM" << std::endl;
}

void Obake::ASystem::empty()
{

}

void Obake::ASystem::executeAtBegin()
{
	_beginLoop = --_executionQueue.end();
}

void Obake::ASystem::executeAtEnd()
{
	_executionQueue.push_back(std::bind(&Obake::ASystem::jump, this));
}

void Obake::ASystem::jump()
{
	if (_beginLoop != _executionQueue.end())
	{
		_task = _beginLoop;
	}
}