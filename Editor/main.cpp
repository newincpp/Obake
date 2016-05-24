#include "Core.hh"
#include "TestEventSystem1.hh"
#include "TestEventSystem2.hh"
#include "VulkanRenderer.hh"

int main()
{
	Obake::Core c;
	System::TestEventSystem1 system1;
	System::TestEventSystem2 system2;
	System::VulkanRenderer vulkan;

	system1.registerCore(&c);
	system2.registerCore(&c);
	vulkan.registerCore(&c);
	system1.initialize();
	system2.initialize();
	vulkan.initialize();

	//c.registerSystem(&system1);
	//c.registerSystem(&system2);


	int runRet = c.run();
	system("Pause");
	return runRet;
}
