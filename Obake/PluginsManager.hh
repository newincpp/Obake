#pragma once
#ifdef stdMode
#include <vector>
#include <string>
#else
#include <EASTL/EASTL/vector.h>
#include <EASTL/EASTL/string.h>
#endif
#include "SharedLibrary.hh"

namespace Obake
{
	class AvailablePlugin
	{
	private:

		STL::string _path;
		bool _isLoaded;
		PluginInfos _infos;
		IPlugin*	_plugin;
		SharedLibrary _lib;

	public:

		~AvailablePlugin();
		explicit AvailablePlugin(const STL::string& path_);

		bool load();
		bool unload();
		bool reload();
		bool isLoaded() const;
		const STL::string& getPath() const;
		PluginInfos& getInfos();
		IPlugin* getPlugin();
	};

	class PluginsManager
	{
	private:
		STL::vector<AvailablePlugin*> _availablePlugins;
		
		STL::vector<STL::string> _getFilesPathsFromFolder(const STL::string& folder_) const;
		bool _isAvailablePluginExists(AvailablePlugin* testPlugin_) const;

	public:
		~PluginsManager();
		explicit PluginsManager();

		const STL::vector<AvailablePlugin*>& getAllAvailablePlugins() const;
		void displayPluginsInfos();
		AvailablePlugin* getAvailablePlugin(const STL::string& name_) const;
		AvailablePlugin* getAvailablePlugin(uint16_t id_) const;
		bool loadAvailablePlugin(const STL::string& name_) const;
		bool loadAvailablePlugin(uint16_t id_) const;
		bool unloadAvailablePlugin(const STL::string& name_) const;
		bool unloadAvailablePlugin(uint16_t id_) const;
		void loadAllAvailablePlugins() const;
	};
}
