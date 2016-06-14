#pragma once

#include <functional>
#include <vector>

namespace Obake {
	class Core;

	class ASystem {
	protected:
		void executeAtBegin();
		bool executeAtEnd();
		std::vector<std::function<void(void)>> _executionQueue;
		decltype(_executionQueue)::iterator _beginLoop;
		decltype(_executionQueue)::iterator _task;

		void jump();

#define OBAKE_ADD(func) _executionQueue.push_back(std::bind(func, this));
#define OBAKE_LOOP for (executeAtBegin(); executeAtEnd(); )

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
		inline void _registerCore(Core* core_) { _core = core_; }

	public:
		Core* _core;

		inline bool isStillWorking() const { return (_task != _executionQueue.end()); }
		inline void registerCore(Core* core_) { _core = core_; }
		inline void start() { _start(); }
		void fillTask(decltype(_executionQueue)::value_type& task_) { _fillTask(task_); }
		inline const decltype(_executionQueue)& getExecQueue() const { return _executionQueue; }
		virtual ~ASystem() {};
		explicit ASystem();
	};
}