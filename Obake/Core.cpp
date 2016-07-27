#include "Core.hh"

#ifndef stdMode
void* operator new[](size_t size, const char* pName, int flags, unsigned debugFlags, const char* file, int line) {
    Obake::log::debug("allocate");

    //std::cout << "allocSize:" << size << std::endl;
    //if (pName) {
    //	std::cout << "pName" << pName << std::endl;
    //}
    //std::cout << "flags " << flags << std::endl;
    //std::cout << "debug " << debugFlags << std::endl;
    //if (file) {
    //	std::cout << "file " << file << std::endl;
    //}
    //std::cout << "line " << line << std::endl << std::endl;
    return malloc(size);
}

void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* file, int line) {
    Obake::log::debug("alligned allocation");
    //std::cout << "allocSize:" << size << std::endl;
    //std::cout << "alignment " << alignment << std::endl;
    //std::cout << "alignmentOffset " << alignmentOffset << std::endl;
    //if (pName) {
    //	std::cout << "pName" << pName << std::endl;
    //}
    //std::cout << "flags " << flags << std::endl;
    //std::cout << "debug " << debugFlags << std::endl;
    //if (file) {
    //	std::cout << "file " << file << std::endl;
    //}
    //std::cout << "line " << line << std::endl << std::endl;
    //force alignment for performance reason (like SIMDs)
    EASTL_ASSERT(alignment <= 8);
#if defined (WIN32)
    return _aligned_malloc(size, alignment);
#else
    return aligned_alloc(alignment, size);
#endif
}
#endif


void Obake::Core::executeNext(ASystem* system_) {
	STL::function<void(void)> task = []() {};
	system_->fillTask(task);

	if (_threadingDisabled)
	{
		task();
		return;
	}
	if (_threadPool.size() < std::thread::hardware_concurrency()) {
		_threadPool.emplace_back(task);
	}
	else {
		bool taskAssigned = false;
		while (!taskAssigned) {
			for (decltype(_threadPool)::value_type& thread : _threadPool) {
				if (thread.joinable()) {
					thread = std::thread(task);
					taskAssigned = true;
					return;
				}
			}
		}
	}
}

bool Obake::Core::registerSystem(ASystem* sys_)
{
	sys_->registerCore(this);
	sys_->start();
	_registeredSystems.push_back(sys_);
	return true;
}

int Obake::Core::run() {
	bool still = true;
	bool currentSystemStill = false;
	while (still) {
		still = false;
		/*	for (decltype(_registeredSystems)::value_type system : _registeredSystems)
			{
			if (system != nullptr)
			{
			executeNext(system);
			still |= system->isStillWorking();
			}
			}*/

		for (decltype(_registeredSystems)::iterator systemIt = _registeredSystems.begin()
				; systemIt != _registeredSystems.end(); ++systemIt)
		{
			if ((*systemIt) != nullptr)
			{
				executeNext(*systemIt);
				currentSystemStill = (*systemIt)->isStillWorking();
				still |= currentSystemStill;
				/*if (!currentSystemStill)
				  {
				  (*systemIt)->unload();
				  _registeredSystems.erase(systemIt);
				  systemIt = _registeredSystems.begin();
				  }*/
			}
		}
		//std::cout << "Still: " << std::boolalpha << still << " | RegisteredSystems Size: " << _registeredSystems.size() << std::endl;
	}
	return 0;
}

	Obake::Core::Core()
: _threadingDisabled(true)
{
	std::cout << "launching: " << std::thread::hardware_concurrency() << " threads" << std::endl;
}

Obake::Core::~Core() {
	for (decltype(_threadPool)::value_type& thd : _threadPool) {
		if (thd.joinable()) {
			thd.join();
		}
	}
}
