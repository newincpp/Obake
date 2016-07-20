#include <Core.hh>
#include <PluginsManager.hh>

int main()
{
	std::cout << "Plop" << std::endl;
	Obake::Core core;

	std::cout << "Plip" << std::endl;
	Obake::PluginsManager pluginsManager;
	std::cout << "Plap" << std::endl;

	pluginsManager.loadAllAvailablePlugins();
	pluginsManager.displayPluginsInfos();
	const eastl::vector<Obake::AvailablePlugin*>& availablePlugins = pluginsManager.getAllAvailablePlugins();

	for (Obake::AvailablePlugin* plugin : availablePlugins)
	{
		if (plugin->isLoaded())
		{
			//plugin->getPlugin()->registerCore(&core);
			//plugin->getPlugin()->initialize();
			//core.registerSystem(plugin->getPlugin());
		}
	}


	int runRet = core.run();
	//return runRet;

	for (Obake::AvailablePlugin* plugin : availablePlugins)
	{
		if (plugin->isLoaded())
		{
			plugin->getPlugin()->unload();
		}
	}

	system("Pause");

	return 0;
}
