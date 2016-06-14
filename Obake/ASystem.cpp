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
	std::cout << "Execute at begin" << std::endl;
	_beginLoop = --_executionQueue.end();
}

bool Obake::ASystem::executeAtEnd()
{
	std::cout << "Execute at end" << std::endl;
	_executionQueue.push_back(std::bind(&Obake::ASystem::jump, this));

	return false;
}

void Obake::ASystem::jump()
{
	if (_beginLoop != _executionQueue.end())
	{
		_task = _beginLoop;
	}
}