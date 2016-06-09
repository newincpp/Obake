#pragma once
#include "Plugin.hh"

class TestPlugin : public Obake::IPlugin
{
public:
	virtual ~TestPlugin();
	TestPlugin();

	virtual void sayHello();
};

