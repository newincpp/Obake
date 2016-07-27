#pragma once
#include "Plugin.hh"

class TestPlugin : public Obake::IPlugin
{
public:
	virtual ~TestPlugin();
	TestPlugin();

	void registerCore(Obake::Core* core_);
	void initialize();
	void unload();
};

