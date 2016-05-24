#pragma once
#include "Core.hh"
#include "ASystem.hh"

namespace System
{
	class TestEventSystem2 : public Obake::ASystem
	{
	private:

	public:
		virtual ~TestEventSystem2();
		explicit TestEventSystem2();

		void initialize() override;
		void registerEvent(std::string eventName);
		void executeEvent(std::string eventName);
		void receiveEvent(std::string receiveEventFrom);
	};
}