#include "Core.hh"
#include "ASystem.hh"

Obake::ASystem::ASystem()
	: _beginLoop(_executionQueue.end()), _core(nullptr),
	_pushNextAsBeginLoop(false)
{
    _executionQueue.reserve(32);
}

void Obake::ASystem::executeAtBegin()
{
	//std::cout << "Execute at begin" << std::endl;
	_beginLoop = --_executionQueue.end();
	_pushNextAsBeginLoop = true;
	_loopCount = 0;
}

void Obake::ASystem::executeAtEnd()
{
	//std::cout << "Execute at end" << std::endl;
	_executionQueue.push_back(std::bind(&Obake::ASystem::jump, this));
	++_loopCount;
}

void Obake::ASystem::jump()
{
	if (_beginLoop != _executionQueue.end())
	{
		_task = _beginLoop;
	}
}