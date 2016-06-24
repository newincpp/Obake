#pragma once

#include <vector>

#include "BUILD_OPTIONS.h"
#include "Platform.h"
#include "Shared.h"

#include "Core.hh"
#include "Plugin.hh"

namespace System
{
	class VulkanRenderer : public Obake::IPlugin
	{
	private:
		static VulkanRenderer *_sysInstance;

	public:
		explicit VulkanRenderer();
		~VulkanRenderer();

		static VulkanRenderer & getInstance()
		{
			if (_sysInstance == nullptr)
				_sysInstance = new VulkanRenderer();
			return *_sysInstance;
		};

		void registerCore(Obake::Core* core_);
		void initialize();
		void unload();

		void vulkanEvent();
		void sendWinHandleEvent(HWND winHandle_, HINSTANCE winInstance_);
		void createSurface();

	private:
		void _InitInstance();
		void _DeInitInstance();

		void _InitDevice();
		void _DeInitDevice();

		void _InitExtension();
		void _DeInitExtension();

		void _SetupDebug();
		void _InitDebug();
		void _DeInitDebug();

		void _InitSurface();
		void _DeInitSurface();

		void _InitSwapchain();
		void _DeInitSwapchain();


		// Instance Initialisation
		VkInstance					_instance = VK_NULL_HANDLE;

		// Device Initialisation
		VkPhysicalDevice			_gpu = VK_NULL_HANDLE;
		VkDevice					_device = VK_NULL_HANDLE;
		VkQueue						_queue = VK_NULL_HANDLE;
		VkPhysicalDeviceProperties	_gpuProperties = {};

		uint32_t					_graphicsFamilyIndex = 0;

		// VALIDATION LAYERS
		std::vector<const char*>	_instanceLayers;
		std::vector<const char*>	_instanceExtensions;
		//std::vector<const char*>	_deviceLayers; //deprecated
		std::vector<const char*>	_deviceExtensions;

		// Debugging

		// Function pointer to two functions in the DEBUG Extension
		PFN_vkCreateDebugReportCallbackEXT fvkCreateDebugReportCallbackEXT = VK_NULL_HANDLE;
		PFN_vkDestroyDebugReportCallbackEXT fvkDestroyDebugReportCallbackEXT = VK_NULL_HANDLE;

		// A handle set by vkCreateDebugReportCallbackEXT, not used at the moment
		VkDebugReportCallbackEXT	_debugReport = VK_NULL_HANDLE;

		// A structure containing information in regards to CreateDebugReportCallback
		// It's here because it is used in 2 functions unlike the others that are just
		// declared locally
		VkDebugReportCallbackCreateInfoEXT _debugCallbackCreateInfo{};

		// WINDOW RELATED VARIABLES

		HWND _hwnd = nullptr;
		HINSTANCE _hInstance = nullptr;
		VkSurfaceKHR _surface = VK_NULL_HANDLE;
		VkSurfaceCapabilitiesKHR _surfaceCapabilities = {};
		VkSurfaceFormatKHR _surfaceFormat = {};

		uint32_t _surfaceX = 512;
		uint32_t _surfaceY = 512;

		VkSwapchainKHR _swapchain = VK_NULL_HANDLE;
	};
}