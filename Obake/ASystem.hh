#pragma once

#include <functional>
#include <vector>

namespace Obake {
	class Core;

	class ASystem {
	protected:
		void executeAtBegin();
		void executeAtEnd();
		std::vector<std::function<void(void)>> _executionQueue;
		decltype(_executionQueue)::iterator _beginLoop;
		decltype(_executionQueue)::iterator _task;

		void jump();

#define OBAKE_ADD(func) _executionQueue.push_back(std::bind(func, this));
#define OBAKE_LOOP for (executeAtBegin(); false; executeAtEnd())

		inline decltype(_executionQueue)::value_type& _getTask() { decltype(_executionQueue)::value_type&& t = std::move(*_task); if (_task != _executionQueue.end()) { _task++; } return t; }
		inline void _start()
		{
			_task = _executionQueue.begin();
		}
		inline void _registerCore(Core* core_) { _core = core_; }

	public:
		Core* _core;

		inline bool isStillWorking() const { return (_task != _executionQueue.end()); }
		inline void registerCore(Core* core_) { _registerCore(core_); }
		inline void start(){ _start(); }
		inline decltype(_executionQueue)::value_type& getTask() { return _getTask(); }
		inline const decltype(_executionQueue)& getExecQueue() const { return _executionQueue; }
		virtual ~ASystem() {};
		explicit ASystem();
	};
}