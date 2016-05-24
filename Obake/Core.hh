#pragma once

#include <vector>
#include <thread>
#include <iostream>

#include "ASystem.hh"
#include "EventsManager.hh"

namespace Obake {
	class ASystem;
    class Core {
    private:
#if defined( WIN32 )
        typedef void OSReturnType;
#else
        typedef int OSReturnType ;
#endif
        std::vector<ASystem*> _registeredSystems;
        std::vector<std::thread> _threadPool;
        void executeNext(ASystem* system_);
    public:
        explicit Core();
        ~Core();
        bool registerSystem(ASystem*);
        OSReturnType run();
		EventsManager eventsManager;
    };
}