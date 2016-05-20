#pragma once

#include <functional>
#include <vector>
#include "Event.hh"

namespace Obake {
    class ASystem {
    protected:
        std::vector<std::function<void(void)>> _executionQueue;
        decltype(_executionQueue)::iterator _task;
    public:
        inline bool isStillWorking() const { return (_task != _executionQueue.end()); }
        inline void start() { _task = _executionQueue.begin(); }
        inline decltype(_executionQueue)::value_type& getTask() { decltype(_executionQueue)::value_type&& t = std::move(*_task); if (_task != _executionQueue.end()) {_task++;} return t; }
        inline const decltype(_executionQueue)& getExecQueue() const {
            return _executionQueue;
        }
        ASystem();
    };
}