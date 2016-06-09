#pragma once

#include <functional>
#include <vector>

namespace Obake {
    class Core;

    class ASystem {
    protected:
        std::vector<std::function<void(void)>> _executionQueue;
        std::vector<std::function<void(void)>> _executionQueueRo;
        decltype(_executionQueue)::iterator _task;
        decltype(_executionQueueRo)::const_iterator _taskRo;
	Core* _core;
    public:
	virtual inline void initialize() {};

        inline bool isStillWorking() const { return (_task != _executionQueue.end()); }
	inline void registerCore(Core* core_) { _core = core_; }
        inline void start() { _task = _executionQueue.begin(); _taskRo = _executionQueueRo.begin(); }
	inline decltype(_executionQueue)::value_type& getTask() {
		decltype(_executionQueue)::value_type&& t = std::move(*_task); 
		if (_task != _executionQueue.end()) {_task++;} 
		return t;
	}
        inline const decltype(_executionQueueRo)::value_type& getTaskRo() {
		const decltype(_executionQueueRo)::value_type&& t = std::move(*_taskRo);
		if (_taskRo != _executionQueueRo.end()) {_taskRo++;} 
		return t; 
	}
        inline decltype(_executionQueue)& getExecQueue() {
            return _executionQueue;
        }
        inline decltype(_executionQueueRo)& getExecQueueRo() {
            return _executionQueueRo;
        }
        ASystem();
    };
}
