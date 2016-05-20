#pragma once
#include "Core.hh"
#include "ASystem.hh"

namespace System
{
	class TestEventSystem1 : public Obake::ASystem
	{
	private:
		Obake::Core* _core;

	public:
		~TestEventSystem1();
		explicit TestEventSystem1(Obake::Core* core);

		void registerEvent(std::string eventName);
		void executeEvent(std::string eventName);
		void receiveEvent(std::string receiveEventFrom);
	};
}