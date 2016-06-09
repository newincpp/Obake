//#include <iostream>
#include "TestPlugin.hh"

OBAKE_PLUGIN(TestPlugin, "Test Plugin", "0.1.0")

TestPlugin::~TestPlugin()
{
}

TestPlugin::TestPlugin()
{
}

void TestPlugin::sayHello()
{
	std::cout << "HELLO FROM TEST PLUGIN" << std::endl;
}