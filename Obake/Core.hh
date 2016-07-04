#pragma once

#include <vector>
#include <thread>
#include <iostream>

#if !defined GLM_FORCE_SSE2 && !defined GLM_SWIZZLE
#define GLM_FORCE_SSE2
#define GLM_SWIZZLE
#endif
// #define GLM_FORCE_AVX

#include "glm/glm.hpp"
#include "ASystem.hh"
#include "EventsManager.hh"
#include "SharedLibrary.hh"

namespace Obake {
	class ASystem;
    class Core {
    private:
		bool _threadingDisabled;
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
