#pragma once
#include <iostream>
#include <EASTL/EASTL/string.h>
#include "Definitions.hh"
#if defined(LINUX) || defined (APPLE)
#	include <dlfcn.h>
#endif
#include "ASystem.hh"

namespace Obake
{
	class IPlugin;
#define OBAKE_PLUGIN_API_VERSION 1
	//#define BUILDING_OBAKE_SHARED

#ifdef WIN32
#	define OBAKE_PLUGIN_EXPORT __declspec(dllexport)
#else
#	define OBAKE_PLUGIN_EXPORT 
#endif

#ifdef WIN32
	/* Windows - set up dll import/export decorators. */
# if defined(BUILDING_OBAKE_SHARED)
	/* Building shared library. */
#   define OBAKE_EXTERN __declspec(dllexport)
# elif defined(USING_OBAKE_SHARED)
	/* Using shared library. */
#   define OBAKE_EXTERN __declspec(dllimport)
# else
	/* Building static library. */
#   define OBAKE_EXTERN /* nothing */
# endif
#else
#   define OBAKE_EXTERN /* nothing */
#endif

	OBAKE_EXTERN typedef IPlugin* (*CreatePluginFunc)();

	struct ExternPluginInfos
	{
		uint16_t apiVersion;
		const char* fileName;
		const char* className;
		const char* pluginName;
		const char* pluginVersion;
		CreatePluginFunc initializeFunc;
	};

	struct PluginInfos
	{
		uint16_t apiVersion;
		eastl::string fileName;
		eastl::string className;
		eastl::string pluginName;
		eastl::string pluginVersion;
		CreatePluginFunc initializeFunc;
		ExternPluginInfos* externInfos;
	};

#define OBAKE_STANDARD_PLUGIN_STUFF \
    OBAKE_PLUGIN_API_VERSION,       \
    __FILE__

#define OBAKE_PLUGIN(classType, pluginName, pluginVersion)     \
  extern "C" {                                               \
     OBAKE_PLUGIN_EXPORT Obake::IPlugin* createPlugin()     \
      {                                                      \
         return new classType;                               \
      }                                                      \
      OBAKE_PLUGIN_EXPORT Obake::ExternPluginInfos exports =  \
      {                                                      \
          OBAKE_STANDARD_PLUGIN_STUFF,                         \
          #classType,                                        \
          pluginName,                                        \
          pluginVersion,                                     \
          createPlugin,                                      \
      };                                                     \
  }

	class IPlugin : public ASystem
	{
	private:

	public:
		virtual ~IPlugin() {};
		IPlugin() : ASystem() { };

		// TODO Remove test
		virtual void initialize() = 0;
		virtual void unload() = 0;
	};

}
