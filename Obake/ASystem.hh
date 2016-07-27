#pragma once

#include "config.hh"
#ifdef stdMode
#include <vector>
#include <functional>
#else
#include <EASTL/EASTL/functional.h>
#include <EASTL/EASTL/vector.h>
#endif

#include <iostream>
#include <typeTraitsUtility.hh>

namespace Obake {
    class Core;
	class ASystem {
	protected:
		void executeAtBegin();
		void executeAtEnd();
		STL::vector<STL::function<void(void)>> _executionQueue;
		decltype(_executionQueue)::iterator _beginLoop;
		decltype(_executionQueue)::iterator _task;
		bool _pushNextAsBeginLoop;

		void jump();

#define OBAKE_ADD(func) _executionQueue.push_back(std::bind(&Obake::__remove_reference__<decltype(*this)>::type::func, this));  \
		if (_pushNextAsBeginLoop) \
		{  if (_executionQueue.size() > 0) { _beginLoop = std::prev(_executionQueue.end()); } \
		else { _beginLoop = _executionQueue.end(); } _pushNextAsBeginLoop = false;}
#define OBAKE_LOOP for (executeAtBegin(); _pushNextAsBeginLoop; executeAtEnd())

		void _fillTask(decltype(_executionQueue)::value_type& task_)
		{
			if (_task != _executionQueue.end())
			{
				task_ = *_task;
				//task_ = std::move(*_task);
				++_task;
			}
		}

		inline void _start()
		{
			_task = _executionQueue.begin();
		}

	public:
		Core* _core;

		inline bool isStillWorking() const { return (_task != _executionQueue.end()); }
		void registerCore(Core* core_);
		inline void start() { _start(); }
		void fillTask(decltype(_executionQueue)::value_type& task_) { _fillTask(task_); }
		inline void shutdown() { _beginLoop = _executionQueue.end(); }
		void unload() {}
		inline const decltype(_executionQueue)& getExecQueue() const { return _executionQueue; }
		virtual ~ASystem() {};
		explicit ASystem();
	};
}
