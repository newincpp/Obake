#pragma once
#include "SharedLibrary.hh"

namespace Obake
{
	class AvailablePlugin
	{
	private:

		std::string _path;
		bool _isLoaded;
		PluginInfos _infos;
		IPlugin*	_plugin;
		SharedLibrary _lib;

	public:

		~AvailablePlugin();
		explicit AvailablePlugin(const std::string& path_);

		bool load();
		bool unload();
		bool reload();
		bool isLoaded() const;
		const std::string& getPath() const;
		PluginInfos& getInfos();
		IPlugin* getPlugin();
	};

	class PluginsManager
	{
	private:
		std::vector<AvailablePlugin*> _availablePlugins;
		
		std::vector<std::string> _getFilesPathsFromFolder(const std::string& folder_) const;
		bool _isAvailablePluginExists(AvailablePlugin* testPlugin_) const;

	public:
		~PluginsManager();
		explicit PluginsManager();

		const std::vector<AvailablePlugin*>& getAllAvailablePlugins() const;
		void displayPluginsInfos();
		AvailablePlugin* getAvailablePlugin(const std::string& name_) const;
		AvailablePlugin* getAvailablePlugin(uint16_t id_) const;
		bool loadAvailablePlugin(const std::string& name_) const;
		bool loadAvailablePlugin(uint16_t id_) const;
		bool unloadAvailablePlugin(const std::string& name_) const;
		bool unloadAvailablePlugin(uint16_t id_) const;
		void loadAllAvailablePlugins() const;
	};
}