#include "MACRO.h"

#include "vulkanTools.h"

#include "VkRenderer.hpp"

using namespace System;

OBAKE_PLUGIN(VkRenderer, "Vulkan Renderer", "0.1.0")

VkRenderer::VkRenderer()
: Renderer()
{
}

VkRenderer::~VkRenderer()
{
}

// # # # # # # # # # # # # # # //

void
VkRenderer::initialize()
{
	Renderer::initialize();
	
	OBAKE_ADD(initVulkan);
	OBAKE_ADD(mainLoop);
}

void
VkRenderer::unload()
{
	if (_enableValidation)
		_debug.freeDebugCallback(_instance);
	destroyInstance();
	Renderer::unload();
}

// # # # # # # # # # # # # # # //

void
VkRenderer::mainLoop()
{
	while (!_isExiting)
	{
		Renderer::mainLoop();
	}
}

void
VkRenderer::initVulkan()
{
	_enableValidation = VULKAN_VALIDATION_LAYER;

	_debug.setupDebugging(VK_DEBUG_REPORT_FLAG_BITS_MAX_ENUM_EXT);
	createInstance();
	if (_enableValidation)
		_debug.createDebugging(_instance);
	createDevice();
}

void
VkRenderer::createInstance()
{	
	PRINT("## [VKRENDERER] [" << __FILE__ << "] CREATE INSTANCE");
	// Contains info in regards to the application
	VkApplicationInfo applicationInfo =
	{
		// (MANDATORY) sType is the type of the structure
		applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		// (MANDATORY) pNext is NULL or a pointer to an extension-specific structure.
		applicationInfo.pNext = NULL,
		// (MANDATORY) The name of our application
		applicationInfo.pApplicationName = "VkRenderer",
		// A developer-supplied version number of the application
		applicationInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0),
		// (MANDATORY) The name of the engine (if any) that was used to create the application
		applicationInfo.pEngineName = "Obake",
		// A developer-supplied version number of the engine
		applicationInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0),
		// (MANDATORY) The version of the Vulkan API that is being used
		applicationInfo.apiVersion = VK_MAKE_VERSION(1, 0, 21)
	};
	
	// Enable surface extensions depending on os
	std::vector<const char*> enabledExtensions = {};
	{
		enabledExtensions.push_back(PLATFORM_SURFACE_EXTENSION_NAME);
		enabledExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
		if (_enableValidation)
		{
			enabledExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
		}
	}
	
	// Contains info in regards to the Vulkan instance
	VkInstanceCreateInfo instanceCreateInfo =
	{
		// (MANDATORY) sType is the type of the structure
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		// (MANDATORY) pNext is NULL or a pointer to an extension-specific structure.
		instanceCreateInfo.pNext = (_enableValidation) ? (&(_debug._debugCallbackCreateInfo)) : (nullptr),
		// - Is reserved for future use -
		instanceCreateInfo.flags = 0,
		// A pointer to a valid VkApplicationInfo structure
		instanceCreateInfo.pApplicationInfo = &applicationInfo,
		// The number of layers that are enabled
		instanceCreateInfo.enabledLayerCount = (_enableValidation) ? (_debug._validationLayerCount) : (0),
		// An array of pointer that contains the names of the layers to activate
		instanceCreateInfo.ppEnabledLayerNames = (_enableValidation) ? (_debug._validationLayerNames.data()) : (nullptr),
		// The number of extensions that are enabled
		instanceCreateInfo.enabledExtensionCount = enabledExtensions.size(),
		// An array of pointers that contains the names of the extensions to activate
		instanceCreateInfo.ppEnabledExtensionNames = enabledExtensions.data()
	};
	
	VK_CHECK_RESULT(vkCreateInstance(&instanceCreateInfo, nullptr, &_instance));
}

void
VkRenderer::destroyInstance()
{
	PRINT("## [VKRENDERER] [" << __FILE__ << "] DESTROY INSTANCE");
	vkDestroyInstance(_instance, nullptr);	
}

void
VkRenderer::createDevice()
{
	PRINT("## [VKRENDERER] [" << __FILE__ << "] CREATE DEVICE");
	pickPhysicalDevice();
}

void
VkRenderer::pickPhysicalDevice()
{
	// Physical device
	uint32_t deviceCount = 0;
	VK_CHECK_RESULT(vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr));
	// If no device supports Vulkan than there is no point in continuing
	assert(deviceCount > 0);
	std::vector<VkPhysicalDevice> devices(deviceCount);
	VK_CHECK_RESULT(vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data()));
	
	for (const auto& device : devices) 
	{
		if (isDeviceSuitable(device))
		{
			_physicalDevice = device;
			break;
		}
	}
	assert(_physicalDevice != VK_NULL_HANDLE);
}

bool
VkRenderer::isDeviceSuitable(VkPhysicalDevice device_)
{
	findQueueFamilies(device_, VK_QUEUE_GRAPHICS_BIT);
	return true;		
}

bool
VkRenderer::findQueueFamilies(VkPhysicalDevice device_, VkQueueFlags flags_)
{
	uint32_t queueCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device_, &queueCount, NULL);
	if (queueCount < 1)
		return false;
	std::vector<VkQueueFamilyProperties> queueProps(queueCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device_, &queueCount, queueProps.data());
	
	for (int graphicsQueueIndex = 0; graphicsQueueIndex < queueCount; ++graphicsQueueIndex)
	{
		if (queueProps[graphicsQueueIndex].queueFlags & flags_)
			return true;
	}
	return false;
}