#include "Core.hh"
#include "PluginsManager.hh"
//#include "TestEventSystem1.hh"
//#include "TestEventSystem2.hh"
//#include "VulkanRenderer.hh"

int main()
{
	Obake::Core c;
	//System::TestEventSystem1 system1;
	//System::TestEventSystem2 system2;
	//System::VulkanRenderer vulkan;

	//system1.registerCore(&c);
	//system2.registerCore(&c);
	//vulkan.registerCore(&c);
	//system1.initialize();
	//system2.initialize();
	//vulkan.initialize();

	//c.registerSystem(&system1);
	//c.registerSystem(&system2);

	//Obake::SharedLibrary testLib;

	//if (!testLib.open(".\\Plugins\\TestPlugin.dll"))
	//	std::cout << "Failed to open dll" << std::endl;
	//else
	//{
	//	Obake::PluginInfos* infos;

	//	testLib.sym("exports", reinterpret_cast<void**>(&infos));
	//	std::cout << "Plugin Info: "
	//		<< "\n\tAPI Version: " << infos->apiVersion
	//		<< "\n\tFile Name: " << infos->fileName
	//		<< "\n\tClass Name: " << infos->className
	//		<< "\n\tPlugin Name: " << infos->pluginName
	//		<< "\n\tPlugin Version: " << infos->pluginVersion
	//		<< std::endl;
	//}

	Obake::PluginsManager pluginsManager;

	pluginsManager.loadAllAvailablePlugins();
	pluginsManager.displayPluginsInfos();
	const std::vector<Obake::AvailablePlugin*>& availablePlugins = pluginsManager.getAllAvailablePlugins();

	for (Obake::AvailablePlugin* plugin : availablePlugins)
	{
		if (plugin->isLoaded())
		{
			c.registerSystem(plugin->getPlugin());
			plugin->getPlugin()->sayHello();
		}
	}

	//int runRet = c.run();
	system("Pause");
	//return runRet;
	return 0;
}
