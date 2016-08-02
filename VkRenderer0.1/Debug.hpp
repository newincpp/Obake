#pragma once

//#include "PLATFORM.h"

#include "Tools.hpp"

namespace Platy
{
	class Debug
	{
	public:
		static Debug *s_instance;
		// Default validation layers
		int							_validationLayerCount;
		std::vector<const char*>	_validationLayerNames;
		// Debug Create Info
		VkDebugReportCallbackCreateInfoEXT _debugCallbackCreateInfo = {};
	protected:
		PFN_vkCreateDebugReportCallbackEXT	CreateDebugReportCallback = VK_NULL_HANDLE;
		PFN_vkDestroyDebugReportCallbackEXT DestroyDebugReportCallback = VK_NULL_HANDLE;
		PFN_vkDebugReportMessageEXT			DebugBreakCallback = VK_NULL_HANDLE;

		//Variale given to createDebugReportCallback that will be filled with info
		VkDebugReportCallbackEXT _msgCallback = VK_NULL_HANDLE;
	private:
		// - - -

	public:
		Debug();
		~Debug();

		static Debug &instance()
		{
			if (!s_instance)
				s_instance = new Debug;
			return *s_instance;
		}

		// Fills the _debugCallbackCreateInfo struct
		void setupDebugging(VkDebugReportFlagsEXT flags_ = VK_DEBUG_REPORT_FLAG_BITS_MAX_ENUM_EXT);
		// Load debug function pointers and set debug callback
		void createDebugging(VkInstance instance_);
		// Clear debug callback
		void freeDebugCallback(VkInstance instance);
	protected:
		// - - -
	private:
		// - - -
	};
}
