#pragma once

#include <functional>
#include <vector>
#include <iostream>

namespace Obake {
	class Core;

	class ASystem {
	protected:
		void empty();
		void executeAtBegin();
		void executeAtEnd();
		std::vector<std::function<void(void)>> _executionQueue;
		decltype(_executionQueue)::iterator _beginLoop;
		decltype(_executionQueue)::iterator _task;
		Core* _core;

		void jump();

#define OBAKE_ADD(func) _executionQueue.push_back(std::bind(func, this));
#define OBAKE_LOOP for (executeAtBegin(); false; _executeAtEnd())


	public:

		inline bool isStillWorking() const { return (_task != _executionQueue.end()); }
		inline void registerCore(Core* core_) { _core = core_; }
		inline void start()
		{
			std::cout << "EXECUTION QUEUE SIZE: " << _executionQueue.size() << std::endl;
			if (_executionQueue.size()) 
			{ 
				_task = _executionQueue.begin(); 
			}
		}
		inline decltype(_executionQueue)::value_type& getTask() { decltype(_executionQueue)::value_type&& t = std::move(*_task); if (_task != _executionQueue.end()) { _task++; } return t; }
		inline const decltype(_executionQueue)& getExecQueue() const {
			return _executionQueue;
		}
		ASystem();
	};
}