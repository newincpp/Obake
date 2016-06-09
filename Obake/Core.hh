#pragma once

#include <vector>
#include <thread>
#include <iostream>

#include "ASystem.hh"
#include "EventsManager.hh"
#include "SharedLibrary.hh"

namespace Obake {
	class ASystem;
    class Core {
    private:

        std::vector<ASystem*> _registeredSystems;
        std::vector<std::thread> _threadPool;
		std::vector<SharedLibrary> _libs;
        void executeNext(ASystem* system_);
    public:
        explicit Core();
        ~Core();
        bool registerSystem(ASystem*);
        int run();
		EventsManager eventsManager;
    };
}