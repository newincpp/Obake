#pragma once

#include <iostream>
#include <sstream>
#include <vector>

#ifdef _WIN32
#include "Windows.h"
#define VK_USE_PLATFORM_WIN32_KHR
#elif __linux__
#define VK_USE_PLATFORM_XCB_KHR
#endif

#include <vulkan\vulkan.h>

#include "Core.hh"
#include "ASystem.hh"

namespace System
{
	class VulkanRenderer : public Obake::ASystem
	{
	private:
		static VulkanRenderer *_sysInstance;

	public:
		explicit VulkanRenderer(Obake::Core* core);
		~VulkanRenderer();

		static VulkanRenderer & getInstance()
		{
			Obake::EventList x;

			if (_sysInstance == nullptr)
				_sysInstance = new VulkanRenderer(x);
			return *_sysInstance;
		};

	private:
		void _InitInstance();
		void _DeInitInstance();

		void _InitDevice();
		void _DeInitDevice();

		void _SetupDebug();
		void _InitDebug();
		void _DeInitDebug();

		// Instance Initialisation
		VkInstance					_instance = nullptr;

		// Device Initialisation
		VkPhysicalDevice			_gpu = nullptr;
		VkDevice					_device = nullptr;
		VkPhysicalDeviceProperties	_gpuProperties = {};
		uint32_t					_graphicsFamilyIndex = 0;

		// VALIDATION LAYERS
		std::vector<const char*>	_instanceLayers;
		std::vector<const char*>	_instanceExtensions;
		//std::vector<const char*>	_deviceLayers; //deprecated
		std::vector<const char*>	_deviceExtensions;

		// Debugging

		// Function pointer to two functions in the DEBUG Extension
		PFN_vkCreateDebugReportCallbackEXT fvkCreateDebugReportCallbackEXT = nullptr;
		PFN_vkDestroyDebugReportCallbackEXT fvkDestroyDebugReportCallbackEXT = nullptr;

		// A handle set by vkCreateDebugReportCallbackEXT, not used at the moment
		VkDebugReportCallbackEXT	_debugReport = NULL;

		// A structure containing information in regards to CreateDebugReportCallback
		// It's here because it is used in 2 functions unlike the others that are just
		// declared locally
		VkDebugReportCallbackCreateInfoEXT _debugCallbackCreateInfo{};
	};
}