#include <iostream>
#include "Core.hh"
#include "ASystem.hh"

Obake::ASystem::ASystem()
	: _beginLoop(_executionQueue.end()), _core(nullptr)
{
    _executionQueue.reserve(32);
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