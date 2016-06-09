#include <iostream>
#include "PluginsManager.hh"

Obake::AvailablePlugin::~AvailablePlugin()
{
	unload();
}

Obake::AvailablePlugin::AvailablePlugin(const std::string& path_)
	: _path(path_), _isLoaded(false)
{
	if (load())
	{
		_isLoaded = false;
		_lib.close();
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
		_lib.sym("exports", reinterpret_cast<void**>(&_infos));
		std::cout << "Plugin Info: "
			<< "\n\tAPI Version: " << _infos->apiVersion
			<< "\n\tFile Name: " <<  _infos->fileName
			<< "\n\tClass Name: " << _infos->className
			<< "\n\tPlugin Name: " << _infos->pluginName
			<< "\n\tPlugin Version: " << _infos->pluginVersion
			<< std::endl;
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

const Obake::PluginInfos* Obake::AvailablePlugin::getInfos() const
{
	return _infos;
}

Obake::Plugin* Obake::AvailablePlugin::getPlugin() const
{
	return _plugin;
}

Obake::PluginsManager::~PluginsManager()
{
}

Obake::PluginsManager::PluginsManager()
{
	std::vector<std::string> pluginsPaths = _getFilesPathsFromFolder(PLUGINS_FOLDER);

	std::cout << "NbFilesFound: " << pluginsPaths.size() << std::endl;

	for (int i = 0; i < pluginsPaths.size(); ++i)
	{
		std::cout << "Path[" << i << "]: " << pluginsPaths[i] << std::endl;
		_availablePlugins.push_back(new AvailablePlugin(pluginsPaths[i]));
	}

	for (int i = 0; i < _availablePlugins.size(); ++i)
	{
		AvailablePlugin* availablePlugin = _availablePlugins[i];

		/*std::cout << "Plugin[" << i << "]: Info: "
			<< "\n\tAPI Version: " << availablePlugin->getInfos().apiVersion
			<< "\n\tFile Name: " << availablePlugin->getInfos().fileName
			<< "\n\tClass Name: " << availablePlugin->getInfos().className
			<< "\n\tPlugin Name: " << availablePlugin->getInfos().pluginName
			<< "\n\tPlugin Version: " << availablePlugin->getInfos().pluginVersion
			<< std::endl;*/
	}

}

std::vector<std::string> Obake::PluginsManager::_getFilesPathsFromFolder(const std::string& folder) const
{
	std::vector<std::string> filesPaths;

#if defined (UNIX) || defined (APPLE)

	dirp = opendir(PLUGINS_FOLDER);
	while ((dp = readdir(dirp)) != NULL)
	{
		filesPaths.push_back(dp->d_name);
	}
	(void)closedir(dirp);

#elif defined (WIN32)

	WIN32_FIND_DATA findFileData;
	HANDLE hFind;

	hFind = FindFirstFile(std::string(std::string(PLUGINS_FOLDER) + "*.dll").c_str(), &findFileData);

	if (INVALID_HANDLE_VALUE == hFind)
	{
		std::cout << PLUGINS_FOLDER << " invalid directory" << std::endl;
		return filesPaths;
	}

	do
	{
		if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			std::cout << "File: " << findFileData.cFileName << " is a directory";
		}
		else
		{
			filesPaths.push_back(std::string(PLUGINS_FOLDER + std::string(findFileData.cFileName)));
		}
	} while (FindNextFile(hFind, &findFileData) != 0);

#endif

	return filesPaths;
}

const std::vector<Obake::AvailablePlugin*>& Obake::PluginsManager::getAvailablePlugins() const
{
	return _availablePlugins;
}