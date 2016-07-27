#include <iostream>
#include "TestPlugin.hh"

OBAKE_PLUGIN(TestPlugin, "Test Plugin", "0.1.0")

TestPlugin::~TestPlugin()
{
}

TestPlugin::TestPlugin()
{
}

void TestPlugin::initialize()
{
}

void TestPlugin::registerCore(Obake::Core* core_)
{
	ASystem::registerCore(core_);
}

void TestPlugin::unload()
{
}
