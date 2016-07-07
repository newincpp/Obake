#include "Core.hh"
#include "ASystem.hh"

Obake::ASystem::ASystem()
	: _beginLoop(_executionQueue.end()), _pushNextAsBeginLoop(false),
	_core(nullptr)
{
    _executionQueue.reserve(32);
}

void Obake::ASystem::registerCore(Core* core_)
{
	_core = core_;
	_core->eventsManager.bindEvent("SHUTDOWN ALL", this, &ASystem::shutdown);
}

void Obake::ASystem::executeAtBegin()
{
	if (_executionQueue.size() > 0)
	{
		_beginLoop = _executionQueue.rbegin().base();
	}
	else
	{
		_beginLoop = _executionQueue.end();
	}
	_pushNextAsBeginLoop = !_pushNextAsBeginLoop;
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
