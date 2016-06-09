#pragma once
#include "Definitions.hh"
#include "ASystem.hh"

namespace Obake
{
	class Plugin;
#define OBAKE_PLUGIN_API_VERSION 1

#ifdef WIN32
#	define OBAKE_PLUGIN_EXPORT __declspec(dllexport)
#else
#	define OBAKE_PLUGIN_EXPORT 
#endif

#ifdef WIN32
#	ifndef OBAKE_SHARED_LIBRARY
#		define OBAKE_EXTERN __declspec(dllexport)
#	else
#		define OBAKE_EXTERN __declspec(dllimport)
#	endif
#else
#	define OBAKE_EXTERN
#endif

	OBAKE_EXTERN typedef Plugin* (*GetPluginFunc)();

	struct ExternPluginInfos
	{
		uint16_t apiVersion;
		const char* fileName;
		const char* className;
		const char* pluginName;
		const char* pluginVersion;
		GetPluginFunc initializeFunc;
	};

	struct PluginInfos
	{
		uint16_t apiVersion;
		std::string fileName;
		std::string className;
		std::string pluginName;
		std::string pluginVersion;
		GetPluginFunc initializeFunc;
	};

#define OBAKE_STANDARD_PLUGIN_STUFF \
    OBAKE_PLUGIN_API_VERSION,       \
    __FILE__

#define OBAKE_PLUGIN(classType, pluginName, pluginVersion)     \
  extern "C" {                                               \
     OBAKE_PLUGIN_EXPORT Obake::Plugin* getPlugin()     \
      {                                                      \
          static classType singleton;                        \
          return &singleton;                                 \
      }                                                      \
      OBAKE_PLUGIN_EXPORT Obake::ExternPluginInfos exports =  \
      {                                                      \
          OBAKE_STANDARD_PLUGIN_STUFF,                         \
          #classType,                                        \
          pluginName,                                        \
          pluginVersion,                                     \
          getPlugin,                                         \
      };                                                     \
  }

	class Plugin : public ASystem
	{
	private: 

	public:
		virtual ~Plugin();
		Plugin();
	};

}