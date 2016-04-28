#pragma once

#include <vector>
#include <thread>
#include <iostream>

#include "Event.hh"
#include "ASystem.hh"

namespace Obake {
    class Core {
    private:
#if defined( WIN32 )
        typedef void OSReturnType;
#else
        typedef int OSReturnType ;
#endif
        std::vector<ASystem*> _registeredSystems;
        std::vector<std::thread> _threadPool;
        EventList _frameEvent;
        void executeNext(ASystem* system_);
    public:
        inline const decltype(_frameEvent)& getEventListRef() {
            return _frameEvent;
        }
        explicit Core();
        ~Core();
        bool registerSystem(ASystem*);
        OSReturnType run();
    };
}