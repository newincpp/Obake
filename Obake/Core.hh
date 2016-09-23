#pragma once

#include <thread>
#include <iostream>

#if !defined GLM_FORCE_SSE2 && !defined GLM_SWIZZLE
#define GLM_FORCE_SSE2
#define GLM_SWIZZLE
#endif
// #define GLM_FORCE_AVX

#ifdef stdMode
#include <string>
#else
#include <EASTL/EASTL/string.h>
#endif

#include "glm/glm.hpp"
#include "config.hh"
#include "TypesManager.hh"
#include "ASystem.hh"
#include "EventsManager.hh"
#include "SharedLibrary.hh"

namespace Obake {
	class Core {
		private:
			bool _threadingDisabled;
			STL::vector<ASystem*> _registeredSystems;
			STL::vector<std::thread> _threadPool;
			STL::vector<SharedLibrary> _libs;
			void executeNext(ASystem* system_);
		public:
			explicit Core();
			~Core();
			bool registerSystem(ASystem*);
			int run();
			EventsManager eventsManager;
	};
}
