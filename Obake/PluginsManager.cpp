#include <iostream>
#include "PluginsManager.hh"

Obake::AvailablePlugin::~AvailablePlugin()
{
	unload();
}

Obake::AvailablePlugin::AvailablePlugin(const eastl::string& path_)
	: _path(path_), _isLoaded(false)
{
	if (load())
	{
		_isLoaded = false;
		_lib.close();
		_plugin = nullptr;
		_infos.initializeFunc = nullptr;
	}
}

bool Obake::AvailablePlugin::load()
{
	if (!_lib.open(_path))
	{
		_isLoaded = false;
	}
	else
	{
		ExternPluginInfos* tmpInfos;

		_lib.sym("exports", reinterpret_cast<void**>(&tmpInfos));

		_infos.apiVersion = tmpInfos->apiVersion;
		_infos.fileName = eastl::string(tmpInfos->fileName);
		_infos.className = eastl::string(tmpInfos->className);
		_infos.pluginName = eastl::string(tmpInfos->pluginName);
		_infos.pluginVersion = eastl::string(tmpInfos->pluginVersion);
		_infos.initializeFunc = tmpInfos->initializeFunc;
		_plugin = static_cast<Obake::IPlugin*>(_infos.initializeFunc());

		//_plugin->sayHello();

		/*	std::cout << "Plugin Info: "
				<< "\n\tAPI Version: " << _infos.apiVersion
				<< "\n\tFile Name: " << _infos.fileName
				<< "\n\tClass Name: " << _infos.className
				<< "\n\tPlugin Name: " << _infos.pluginName
				<< "\n\tPlugin Version: " << _infos.pluginVersion
				<< std::endl;*/
		_isLoaded = true;
	}
	return _isLoaded;
}

bool Obake::AvailablePlugin::unload()
{
	if (_isLoaded)
	{
		_lib.close();
		_isLoaded = false;
		delete _plugin;
		_plugin = nullptr;
		_infos.initializeFunc = nullptr;
	}

	return !_isLoaded;
}

bool Obake::AvailablePlugin::reload()
{
	if (unload())
		return load();
	return false;
}

bool Obake::AvailablePlugin::isLoaded() const
{
	return _isLoaded;
}

const eastl::string& Obake::AvailablePlugin::getPath() const
{
	return _path;
}

Obake::PluginInfos& Obake::AvailablePlugin::getInfos()
{
	return _infos;
}

Obake::IPlugin* Obake::AvailablePlugin::getPlugin()
{
	return _plugin;
}

Obake::PluginsManager::~PluginsManager()
{
}

Obake::PluginsManager::PluginsManager()
{
	eastl::vector<eastl::string> pluginsPaths = _getFilesPathsFromFolder(PLUGINS_FOLDER);
	
	std::cout << "Hello" << std::endl;

	std::cout << "NbFilesFound: " << pluginsPaths.size() << std::endl;

	
	int i = 0;
	for (const eastl::string& pluginPath : pluginsPaths)
	{
		std::cout << "Path[" << i << "]: " << pluginsPaths[i].c_str() << std::endl;
		AvailablePlugin* newPlugin = new AvailablePlugin(pluginPath);

		if (_isAvailablePluginExists(newPlugin))
		{
			delete newPlugin;
		}
		else
		{
			_availablePlugins.push_back(newPlugin);
		}
		++i;
	}

	//for (decltype(_availablePlugins)::value_type availablePlugin : _availablePlugins)
	//{

	//	/*std::cout << "Plugin[" << i << "]: Info: "
	//		<< "\n\tAPI Version: " << availablePlugin->getInfos().apiVersion
	//		<< "\n\tFile Name: " << availablePlugin->getInfos().fileName
	//		<< "\n\tClass Name: " << availablePlugin->getInfos().className
	//		<< "\n\tPlugin Name: " << availablePlugin->getInfos().pluginName
	//		<< "\n\tPlugin Version: " << availablePlugin->getInfos().pluginVersion
	//		<< std::endl;*/
	//}

}

eastl::vector<eastl::string> Obake::PluginsManager::_getFilesPathsFromFolder(const eastl::string& folder) const
{
	eastl::vector<eastl::string> filesPaths;

#if defined(LINUX) || defined (APPLE)

	//dirp = opendir(PLUGINS_FOLDER);
	//while ((dp = readdir(dirp)) != NULL)
	//{
	//	filesPaths.push_back(dp->d_name);
	//}
	//(void)closedir(dirp);

#elif defined (WIN32)

	WIN32_FIND_DATA findFileData;
	HANDLE hFind;

	hFind = FindFirstFile(eastl::string(eastl::string(PLUGINS_FOLDER) + "*.dll").c_str(), &findFileData);

	if (INVALID_HANDLE_VALUE == hFind)
	{
		//std::cout << PLUGINS_FOLDER << " invalid directory" << std::endl;
		return filesPaths;
	}

	do
	{
		if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			//std::cout << "File: " << findFileData.cFileName << " is a directory";
		}
		else
		{
			filesPaths.push_back(eastl::string(PLUGINS_FOLDER + eastl::string(findFileData.cFileName)));
		}
	} while (FindNextFile(hFind, &findFileData) != 0);

#endif

	return filesPaths;
}

bool Obake::PluginsManager::_isAvailablePluginExists(AvailablePlugin* testPlugin_) const
{
	for (decltype(_availablePlugins)::value_type currentPlugin : _availablePlugins)
	{
std::cout << "I" << std::endl;
		PluginInfos& infos = currentPlugin->getInfos();
		PluginInfos& testInfos = testPlugin_->getInfos();

		if ((infos.pluginName + infos.pluginVersion) == (testInfos.pluginName + testInfos.pluginVersion))
			return true;
	}
	return false;
}

const eastl::vector<Obake::AvailablePlugin*>& Obake::PluginsManager::getAllAvailablePlugins() const
{
	return _availablePlugins;
}

void Obake::PluginsManager::displayPluginsInfos()
{
	std::cout << std::endl << "Available Plugins: " << _availablePlugins.size()
		<< std::endl
		<< "------------------------------" << std::endl;

	for (int i = 0; i < _availablePlugins.size(); ++i)
	{
		AvailablePlugin* availablePlugin = _availablePlugins[i];
		const PluginInfos& infos = availablePlugin->getInfos();

		std::cout << "Plugin[" << (i) << "]:" << std::endl
			<< "\n\tAPI Version: " << infos.apiVersion
			<< "\n\tFile Name: " << infos.fileName.c_str()
			<< "\n\tClass Name: " << infos.className.c_str()
			<< "\n\tPlugin Name: " << infos.pluginName.c_str()
			<< "\n\tPlugin Version: " << infos.pluginVersion.c_str()
			<< "\n\tIs Loaded: " << std::boolalpha << availablePlugin->isLoaded()
			<< "\n\tPath: " << availablePlugin->getPath().c_str()
			<< std::endl << std::endl;
	}
}

Obake::AvailablePlugin* Obake::PluginsManager::getAvailablePlugin(const eastl::string& name_) const
{
	for (decltype(_availablePlugins)::value_type plugin : _availablePlugins)
	{
		if (plugin->getInfos().pluginName == name_)
			return plugin;
	}
	return nullptr;
}

Obake::AvailablePlugin* Obake::PluginsManager::getAvailablePlugin(uint16_t id) const
{
	return (id < _availablePlugins.size()) ? _availablePlugins[id] : nullptr;
}

bool Obake::PluginsManager::loadAvailablePlugin(const eastl::string& name_) const
{
	for (decltype(_availablePlugins)::value_type plugin : _availablePlugins)
	{
		if (plugin->getInfos().pluginName == name_)
			return plugin->load();
	}
	return false;
}

bool Obake::PluginsManager::loadAvailablePlugin(uint16_t id) const
{
	return (id < _availablePlugins.size()) ? _availablePlugins[id]->load() : false;
}

bool Obake::PluginsManager::unloadAvailablePlugin(const eastl::string& name_) const
{
	for (decltype(_availablePlugins)::value_type plugin : _availablePlugins)
	{
		if (plugin->getInfos().pluginName == name_)
			return plugin->unload();
	}
	return false;
}

bool Obake::PluginsManager::unloadAvailablePlugin(uint16_t id) const
{
	return (id < _availablePlugins.size()) ? _availablePlugins[id]->unload() : false;
}

void Obake::PluginsManager::loadAllAvailablePlugins() const
{
	for (decltype(_availablePlugins)::value_type plugin : _availablePlugins)
	{
		plugin->load();
	}
}
