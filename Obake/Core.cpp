#include "Core.hh"

void Obake::Core::executeNext(ASystem* system_) {

	//decltype(system_->getTask()) && task(system_->getTask());
	if (_threadingDisabled)
	{
		//task();
		return ;
	}
	/*if (_threadPool.size() < std::thread::hardware_concurrency()) {
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
	}*/
}

bool Obake::Core::registerSystem(ASystem* sys_) 
{
	std::cout << "REGISTER 1" << std::endl;
	sys_->start();
	std::cout << "REGISTER 2" << std::endl;
	_registeredSystems.push_back(sys_);
	std::cout << "REGISTER 3" << std::endl;
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
		/*		executeNext(system);
				still |= system->isStillWorking();*/
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