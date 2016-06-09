#pragma once
#include "Core.hh"
#include "ASystem.hh"

namespace System
{
	class TestEventSystem1 : public Obake::ASystem
	{
	private:

	public:
		virtual ~TestEventSystem1();
		explicit TestEventSystem1();

		void initialize();
		void registerEvent(std::string eventName);
		void executeEvent(std::string eventName);
		void receiveEvent(std::string receiveEventFrom);
	};
}