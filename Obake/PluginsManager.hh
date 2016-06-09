#pragma once
#include "SharedLibrary.hh"

namespace Obake
{
	class AvailablePlugin
	{
	private:

		std::string _path;
		bool _isLoaded;
		PluginInfos* _infos;
		Plugin*	_plugin;
		SharedLibrary _lib;

	public:

		~AvailablePlugin();
		explicit AvailablePlugin(const std::string& path_);

		bool load();
		bool unload();
		bool reload();
		bool isLoaded() const;
		const std::string& getPath() const;
		PluginInfos* getInfos() const;
		Plugin* getPlugin() const;
	};

	class PluginsManager
	{
	private:
		std::vector<AvailablePlugin*> _availablePlugins;
		std::vector<AvailablePlugin*>::const_iterator  _availablePluginsIt;
		
		std::vector<std::string> _getFilesPathsFromFolder(const std::string& folder) const;

	public:
		~PluginsManager();
		explicit PluginsManager();

		const std::vector<AvailablePlugin*>& getAvailablePlugins() const;
		void displayPluginsInfos();
	};
}