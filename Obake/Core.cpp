#include "Core.hh"

void Obake::Core::executeNext(ASystem* system_) {

	std::function<void(void)> task = []() {};
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
	sys_->start();
	_registeredSystems.push_back(sys_);
	return true;
}

int Obake::Core::run() {
	bool still = true;
	while (still) {
		still = false;
		for (decltype(_registeredSystems)::value_type system : _registeredSystems)
		{
			if (system != nullptr)
			{
				executeNext(system);
				still |= system->isStillWorking();
			}
		}
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