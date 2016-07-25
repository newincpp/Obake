#include "MACRO.h"

#include "vulkanTools.h"

#include "VkRenderer.hpp"

using namespace System;

OBAKE_PLUGIN(VkRenderer, "Vulkan Renderer", "0.1.0")

VkRenderer::VkRenderer()
: Renderer()
{
	_debug = new VkDebug();
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
	destroyDevice();
	destroySurface();
	if (_enableValidation)
		_debug->freeDebugCallback(_instance);
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

	// This is done at the construction of VkDebug but you can call it again
	// to specify debug flags
	_debug->setupDebugging(VK_DEBUG_REPORT_ERROR_BIT_EXT);
	createInstance();
	if (_enableValidation)
		_debug->createDebugging(_instance);
	createSurface();
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
		// (MANDATORY) pNext is a nullptr or a pointer to an extension-specific structure.
		applicationInfo.pNext = nullptr,
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
		// (MANDATORY) pNext is a nullptr or a pointer to an extension-specific structure.
		instanceCreateInfo.pNext = (_enableValidation) ? (&(_debug->_debugCallbackCreateInfo)) : (nullptr),
		// - Is reserved for future use -
		instanceCreateInfo.flags = 0,
		// A pointer to a valid VkApplicationInfo structure
		instanceCreateInfo.pApplicationInfo = &applicationInfo,
		// The number of layers that are enabled
		instanceCreateInfo.enabledLayerCount = (_enableValidation) ? (_debug->_validationLayerNames.size()) : (0),
		// An array of pointer that contains the names of the layers to activate
		instanceCreateInfo.ppEnabledLayerNames = (_enableValidation) ? (_debug->_validationLayerNames.data()) : (nullptr),
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
VkRenderer::createSurface()
{
	PRINT("## [VKRENDERER] [" << __FILE__ << "] CREATE SURFACE");
	
#ifdef _WIN32
	// Contains info in regards to the Win32 specific surface
	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo =
	{
		// (MANDATORY) sType is the type of the structure
		surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
		// (MANDATORY) pNext is a nullptr or a pointer to an extension-specific structure.
		surfaceCreateInfo.pNext = nullptr,
		// - Is reserved for future use -
		surfaceCreateInfo.flags = 0,
		// Win32 Window instance
		surfaceCreateInfo.hinstance = _winHandle.winInstance,
		// Win32 Window handle to associate the surface with
		surfaceCreateInfo.hwnd = _winHandle.winHandle
	};

	VK_CHECK_RESULT(vkCreateWin32SurfaceKHR(_instance, &surfaceCreateInfo, nullptr, &_surface));
#elif __linux__
		VkXcbSurfaceCreateInfoKHR surfaceCreateInfo =
	{
		// (MANDATORY) sType is the type of the structure
		surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR,
		// (MANDATORY) pNext is a nullptr or a pointer to an extension-specific structure.
		surfaceCreateInfo.pNext = nullptr,
		// - Is reserved for future use -
		surfaceCreateInfo.flags = 0,
		//  A pointer to an xcb_connection_t to the X server
		surfaceCreateInfo.connection = _winHandle.connection,
		// The X11 window handle to associate the surface with
		surfaceCreateInfo.window = _winHandle.window
	};

	VK_CHECK_RESULT(vkCreateXcbSurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface));
#else
	// - - -
#endif
}

void
VkRenderer::destroySurface()
{
	PRINT("## [VKRENDERER] [" << __FILE__ << "] DESTROY SURFACE");
	vkDestroySurfaceKHR(_instance, _surface, nullptr);
}

void
VkRenderer::createDevice()
{
	PRINT("## [VKRENDERER] [" << __FILE__ << "] CREATE DEVICE");
	pickPhysicalDevice();
}

void
VkRenderer::destroyDevice()
{
	vkDestroyDevice(_device, nullptr);
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

	// Vector to store queueCreateInfo for each queue family we wish to create
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	// We go through all compatible devices to see if they match our criterias
	for (int i = 0; i < deviceCount; ++i)
	{
		if (isDeviceSuitable(devices[i], &queueCreateInfos))
		{
			_physicalDevice = devices[i];
			break;
		}
	}

	if (_physicalDevice == VK_NULL_HANDLE)
	{
		PRINT("## [VKRENDERER] [" << __FILE__ << "] [" << __LINE__ << "] No valid gpu found")
		assert(_physicalDevice != VK_NULL_HANDLE);
	}

	{
		VkPhysicalDeviceFeatures deviceFeatures = {};

		// Contains info in regards to the Vukan device
		VkDeviceCreateInfo deviceCreateInfo =
		{
			// (MANDATORY) sType is the type of the structure
			deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			// (MANDATORY) pNext is NULL or a pointer to an extension-specific structure.
			deviceCreateInfo.pNext = NULL,
			// - Is reserved for future use -
			deviceCreateInfo.flags = 0,
			// The number of VkDeviceQueueCreateInfo that will be passed to pQueueCreateInfos
			deviceCreateInfo.queueCreateInfoCount = queueCreateInfos.size(),
			// A pointer to an array of VkDeviceQueueCreateInfo structures describing the queues that are requested to be created along with the logical device
			deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data(),
			// (DEPRECATED)
			deviceCreateInfo.enabledLayerCount = 0,
			// (DEPRECATED)
			deviceCreateInfo.ppEnabledLayerNames = nullptr,
			// The number of extensions that are enabled
			deviceCreateInfo.enabledExtensionCount = 0,
			// An array of pointers that contains the names of the extensions to activate
			deviceCreateInfo.ppEnabledExtensionNames = nullptr,
			// Is NULL or a pointer to a VkPhysicalDeviceFeatures structure that contains boolean indicators of all the features to be enabled
			deviceCreateInfo.pEnabledFeatures = nullptr
		};

		VK_CHECK_RESULT(vkCreateDevice(_physicalDevice, &deviceCreateInfo, nullptr, &_device));

		vkGetDeviceQueue(_device, _queueIndices.graphicsFamily, 0, &_queueHandles.graphicsQueue);
		vkGetDeviceQueue(_device, _queueIndices.presentFamily, 0, &_queueHandles.presentQueue);
	}
}

bool
VkRenderer::isDeviceSuitable(VkPhysicalDevice device_, std::vector<VkDeviceQueueCreateInfo> * queueCreateInfos_)
{
	findQueueFamilies(device_, VK_QUEUE_GRAPHICS_BIT);

	if (_queueIndices.isComplete())
	{
		std::vector<int> queueFamilyIndices = { _queueIndices.graphicsFamily };
		if (_queueIndices.graphicsFamily != _queueIndices.presentFamily)
			queueFamilyIndices.push_back(_queueIndices.presentFamily);

		// Create a VkDeviceQueueCreateInfo for each queue we wish to create
		for (int i = 0; i < queueFamilyIndices.size(); ++i)
		{
			float queuePriority = 1.0f;

			VkDeviceQueueCreateInfo queueCreateInfo =
			{
				// (MANDATORY) sType is the type of the structure
				queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				// (MANDATORY) pNext is a nullptr or a pointer to an extension-specific structure.
				queueCreateInfo.pNext = nullptr,
				// - Is reserved for future use -
				queueCreateInfo.flags = 0,
				// An unsigned integer indicating the index of the queue family to create on this device
				queueCreateInfo.queueFamilyIndex = queueFamilyIndices[i],
				// An unsigned integer specifying the number of queues to create in the queue family indicated by queueFamilyIndex
				queueCreateInfo.queueCount = 1,
				// An array of queueCount normalized floating point values, specifying priorities of work that will be submitted to each created queue
				queueCreateInfo.pQueuePriorities = &queuePriority
			};
			queueCreateInfos_->push_back(queueCreateInfo);
		}
	}

	return _queueIndices.isComplete();
}

void
VkRenderer::findQueueFamilies(VkPhysicalDevice device_, VkQueueFlags flags_)
{
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device_, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device_, &queueFamilyCount, queueFamilies.data());

	VkBool32 presentSupport = false;
	// Checking if the device has a Graphics queue and a queue that can present to the surface we've created
	for (int i = 0; i < queueFamilyCount; ++i)
	{
		if (queueFamilies[i].queueCount > 0 && queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			_queueIndices.graphicsFamily = i;
		}

		VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceSupportKHR(device_, i, _surface, &presentSupport));

		if (queueFamilies[i].queueCount > 0 && presentSupport)
		{
			_queueIndices.presentFamily = i;
		}

		if (_queueIndices.isComplete())
		{
			break;
		}
	}
}
