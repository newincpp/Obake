#pragma once
#include <EASTL/EASTL/vector.h>
#include <EASTL/EASTL/string.h>
#include "SharedLibrary.hh"

namespace Obake
{
	class AvailablePlugin
	{
	private:

		eastl::string _path;
		bool _isLoaded;
		PluginInfos _infos;
		IPlugin*	_plugin;
		SharedLibrary _lib;

	public:

		~AvailablePlugin();
		explicit AvailablePlugin(const eastl::string& path_);

		bool load();
		bool unload();
		bool reload();
		bool isLoaded() const;
		const eastl::string& getPath() const;
		PluginInfos& getInfos();
		IPlugin* getPlugin();
	};

	class PluginsManager
	{
	private:
		eastl::vector<AvailablePlugin*> _availablePlugins;
		
		eastl::vector<eastl::string> _getFilesPathsFromFolder(const eastl::string& folder_) const;
		bool _isAvailablePluginExists(AvailablePlugin* testPlugin_) const;

	public:
		~PluginsManager();
		explicit PluginsManager();

		const eastl::vector<AvailablePlugin*>& getAllAvailablePlugins() const;
		void displayPluginsInfos();
		AvailablePlugin* getAvailablePlugin(const eastl::string& name_) const;
		AvailablePlugin* getAvailablePlugin(uint16_t id_) const;
		bool loadAvailablePlugin(const eastl::string& name_) const;
		bool loadAvailablePlugin(uint16_t id_) const;
		bool unloadAvailablePlugin(const eastl::string& name_) const;
		bool unloadAvailablePlugin(uint16_t id_) const;
		void loadAllAvailablePlugins() const;
	};
}
