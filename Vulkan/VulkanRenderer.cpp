#include <cstdlib>
#include <assert.h>

#include "VulkanRenderer.hh"

using namespace System;

// CALLBACKS
VKAPI_ATTR VkBool32 VKAPI_CALL
VulkanDebugCallback(
	VkDebugReportFlagsEXT msgFlags,
	VkDebugReportObjectTypeEXT objType,
	uint64_t srcObj,
	size_t location,
	int32_t msgCode,
	const char * layerPrefix,
	const char * msg,
	void * userDate)
{
	std::ostringstream stream;

	stream << "VKDBG: ";
	if (msgFlags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
		stream << "INFO: ";
	if (msgFlags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
		stream << "WARNING: ";
	if (msgFlags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
		stream << "PERFORMANCE: ";
	if (msgFlags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
		stream << "ERROR: ";
	if (msgFlags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
		stream << "DEBUG: ";
	stream << "@[" << layerPrefix << "]: ";
	stream << msg << std::endl;

	std::cout << stream.str();

#ifdef _WIN32
	if (msgFlags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
		MessageBox(NULL, stream.str().c_str(), "Vulkan Error!", 0);
#endif
	return false;
}
// - - -

VulkanRenderer::VulkanRenderer(Obake::Core* core)
	: _core(core)
{
	_SetupDebug();
	_InitInstance();
	_InitDebug();
	_InitDevice();
}


VulkanRenderer::~VulkanRenderer()
{
	_DeInitDevice();
	_DeInitDebug();
	_DeInitInstance();
}

void VulkanRenderer::_InitInstance()
{

	// Filling the _instanceExtension array with the name of the extensions we wish to activate:
	{
#if defined(_WIN32)
		// DEPRECATED
		//_instanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(__linux__)
		_instanceExtensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#endif
	}

	// Contains info in regards to the application
	VkApplicationInfo applicationInfo =
	{
		// (MANDATORY) sType is the type of the structure
		applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		// (MANDATORY) pNext is NULL or a pointer to an extension-specific structure.
		applicationInfo.pNext = NULL,
		// (MANDATORY) The name of our application
		applicationInfo.pApplicationName = "Vulkan Test",
		// A developer-supplied version number of the application
		applicationInfo.applicationVersion = 1,
		// (MANDATORY) The name of the engine (if any) that was used to create the application
		applicationInfo.pEngineName = nullptr,
		// A developer-supplied version number of the engine
		applicationInfo.engineVersion = 1,
		// (MANDATORY) The version of the Vulkan API that is being used
		applicationInfo.apiVersion = VK_MAKE_VERSION(1, 0, 11)
	};

	// Contains info in regards to the Vulkan instance
	VkInstanceCreateInfo instanceCreateInfo =
	{
		// (MANDATORY) sType is the type of the structure
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		// (MANDATORY) pNext is NULL or a pointer to an extension-specific structure.
		instanceCreateInfo.pNext = &_debugCallbackCreateInfo,
		// - Is reserved for future use -
		instanceCreateInfo.flags = 0,
		// A pointer to a valid VkApplicationInfo structure
		instanceCreateInfo.pApplicationInfo = &applicationInfo,
		// The number of layers that are enabled
		instanceCreateInfo.enabledLayerCount = _instanceLayers.size(),
		// An array of pointer that contains the names of the layers to activate
		instanceCreateInfo.ppEnabledLayerNames = _instanceLayers.data(),
		// The number of extensions that are enabled
		instanceCreateInfo.enabledExtensionCount = _instanceExtensions.size(),
		// An array of pointers that contains the names of the extensions to activate
		instanceCreateInfo.ppEnabledExtensionNames = _instanceExtensions.data()
	};

	VkResult err = VkResult::VK_SUCCESS;
	//Creates the Vulkan Instance
	if ((err = vkCreateInstance(&instanceCreateInfo, nullptr, &_instance)) != VK_SUCCESS)
	{
		assert(0 && "## Vulkan ERROR: Create instance FAILED.");
	}
	else
	{
		std::cout << "## Vulkan: Create instance SUCCESS" << std::endl;
	}
}

void VulkanRenderer::_DeInitInstance()
{
	vkDestroyInstance(_instance, nullptr);
	_instance = nullptr;
}

void VulkanRenderer::_InitDevice()
{

	//Getting the devices and picking the one we want:
	{
		uint32_t gpuCount = 0;
		// Get number of devices (passing nullptr as third param)
		vkEnumeratePhysicalDevices(_instance, &gpuCount, nullptr);
		std::vector<VkPhysicalDevice> gpuList(gpuCount);
		// Fills the container with the device's info
		vkEnumeratePhysicalDevices(_instance, &gpuCount, gpuList.data());

		_gpu = gpuList[0];
		// Get additional information about the device
		vkGetPhysicalDeviceProperties(_gpu, &_gpuProperties);
	}

	//Getting a queue family that supports graphic processing:
	{
		uint32_t familyCount = 0;
		// Get number of family queues present on the device (passing nullptr as third param)
		//A family of queue is a group of queues of the same type
		vkGetPhysicalDeviceQueueFamilyProperties(_gpu, &familyCount, nullptr);
		std::vector<VkQueueFamilyProperties> familyPropertyList(familyCount);
		// Fills the container with the device's queue's info
		vkGetPhysicalDeviceQueueFamilyProperties(_gpu, &familyCount, familyPropertyList.data());

		bool found = false;
		for (uint32_t i = 0; i < familyCount; ++i)
		{
			if (familyPropertyList[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				found = true;
				_graphicsFamilyIndex = i;
			}
		}
		if (found == false)
		{
			assert(0 && "## Vulkan ERROR: Queue family supporting graphics NOT FOUND.");
		}
	}

	// Getting all available instance layers:
	{
		uint32_t layerCount = 0;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		std::vector<VkLayerProperties> layerPropertyList(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, layerPropertyList.data());
		std::cout << "Instance Layers: " << std::endl;
		for (auto &i : layerPropertyList)
		{
			std::cout << "  " << i.layerName << "\t\t | " << i.description << std::endl;
		}
	}

	// Getting all available device layers:
	{
		uint32_t layerCount = 0;
		vkEnumerateDeviceLayerProperties(_gpu, &layerCount, nullptr);
		std::vector<VkLayerProperties> layerPropertyList(layerCount);
		vkEnumerateDeviceLayerProperties(_gpu, &layerCount, layerPropertyList.data());
		std::cout << "Device Layers: " << std::endl;
		for (auto &i : layerPropertyList)
		{
			std::cout << "  " << i.layerName << "\t\t | " << i.description << std::endl;
		}
	}


	float queuePriorities[]{ 1.0f };

	VkDeviceQueueCreateInfo deviceQueueCreateInfo =
	{
		// (MANDATORY) sType is the type of the structure
		deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		// (MANDATORY) pNext is NULL or a pointer to an extension-specific structure.
		deviceQueueCreateInfo.pNext = NULL,
		// - Is reserved for future use -
		deviceQueueCreateInfo.flags = 0,
		// An unsigned integer indicating the index of the queue family to create on this device.
		deviceQueueCreateInfo.queueFamilyIndex = _graphicsFamilyIndex,
		// The number of queues to create in the queue family
		deviceQueueCreateInfo.queueCount = 1,
		// An array of queueCount normalized floating point values, specifying priorities of work that will be submitted to each created queue.
		deviceQueueCreateInfo.pQueuePriorities = queuePriorities
	};

	VkDeviceCreateInfo deviceCreateInfo =
	{
		// (MANDATORY) sType is the type of the structure
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		// (MANDATORY) pNext is NULL or a pointer to an extension-specific structure.
		deviceCreateInfo.pNext = NULL,
		// - Is reserved for future use -
		deviceCreateInfo.flags = 0,
		// The number of VkDeviceQueueCreateInfo that will be passed to pQueueCreateInfos
		deviceCreateInfo.queueCreateInfoCount = 1,
		// A pointer to an array of VkDeviceQueueCreateInfo structures describing the queues that are requested to be created along with the logical device
		deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo,
		// (DEPRECATED)
		deviceCreateInfo.enabledLayerCount = 0,
		// (DEPRECATED)
		deviceCreateInfo.ppEnabledLayerNames = 0,
		// The number of extensions that are enabled
		deviceCreateInfo.enabledExtensionCount = _deviceExtensions.size(),
		// An array of pointers that contains the names of the extensions to activate
		deviceCreateInfo.ppEnabledExtensionNames = _deviceExtensions.data(),
		// Is NULL or a pointer to a VkPhysicalDeviceFeatures structure that contains boolean indicators of all the features to be enabled
		deviceCreateInfo.pEnabledFeatures = NULL
	};

	// Creates a Vulkan Device
	if (vkCreateDevice(_gpu, &deviceCreateInfo, nullptr, &_device) != VK_SUCCESS)
	{
		assert(0 && "## Vulkan ERROR: Create device FAILED.");
	}
	else
	{
		std::cout << "## Vulkan: Create device SUCCESS" << std::endl;
	}
}

void VulkanRenderer::_DeInitDevice()
{
	vkDestroyDevice(_device, nullptr);
	_device = nullptr;
}

void VulkanRenderer::_SetupDebug()
{
	// Filling the VkDebugReportCallbackCreateInfoEXT struct:
	{
		_debugCallbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
		_debugCallbackCreateInfo.pfnCallback = &VulkanDebugCallback;
		_debugCallbackCreateInfo.flags = VK_DEBUG_REPORT_FLAG_BITS_MAX_ENUM_EXT;
	}

	// Filling the _instanceLayer array with the name of the layers we wish to activate:
	{
		_instanceLayers.push_back("VK_LAYER_LUNARG_standard_validation");
	}
	// Filling the _instanceExtension array with the name of the extensions we wish to activate:
	{
		_instanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	}

	// Since device layers are deprecated for now we'll just leave this commented out
	//	_deviceLayers.push_back("VK_LAYER_LUNARG_standard_validation");
}

void VulkanRenderer::_InitDebug()
{
	fvkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(_instance, "vkCreateDebugReportCallbackEXT");
	fvkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(_instance, "vkDestroyDebugReportCallbackEXT");
	if (fvkCreateDebugReportCallbackEXT == nullptr || fvkDestroyDebugReportCallbackEXT == nullptr)
	{
		assert(0 && "##Vulkan ERROR: Can't fetch debug function pointers.");
	}

	fvkCreateDebugReportCallbackEXT(_instance, &_debugCallbackCreateInfo, nullptr, &_debugReport);
}

void VulkanRenderer::_DeInitDebug()
{
	fvkDestroyDebugReportCallbackEXT(_instance, _debugReport, nullptr);
	_debugReport = NULL;
}