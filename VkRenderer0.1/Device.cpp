#include "MACRO.h"

#include "Device.hpp"

using namespace Platy;

Device::Device(VkInstance instance_, VkSurfaceKHR surface_)
	: _instance(instance_), _surface(surface_)
{

}

Device::~Device()
{

}

void
Device::createDevice()
{
	PRINT("## [VKRENDERER] [" << __FILE__ << "] CREATE DEVICE");
	pickPhysicalDevice();
	createLogicalDevice();
}

void
Device::destroyDevice()
{
	vkDestroyDevice(_device, nullptr);
}
void
Device::pickPhysicalDevice()
{
	// Physical device
	uint32_t deviceCount = 0;
	VK_CHECK_RESULT(vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr));
	// If no device supports Vulkan than there is no point in continuing
	assert(deviceCount > 0);
	std::vector<VkPhysicalDevice> devices(deviceCount);
	VK_CHECK_RESULT(vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data()));

	// We go through all compatible devices to see if they match our criterias
	for (decltype(devices)::value_type & device : devices)
	{
		if (isDeviceSuitable(device))
		{
			_physicalDevice = device;
			break;
		}
	}

	if (_physicalDevice == VK_NULL_HANDLE)
	{
		PRINT("## [VKRENDERER] [" << __FILE__ << "] [" << __LINE__ << "] No valid gpu found")
			assert(_physicalDevice != VK_NULL_HANDLE);
	}
}

void
Device::createLogicalDevice()
{
	// Vector to store queueCreateInfo for each queue family we wish to create
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

	// Setting up the correct queues before the device creation
	{
		std::vector<int> queueFamilyIndices = { _queueIndices.graphicsFamily };
		if (_queueIndices.graphicsFamily != _queueIndices.presentFamily)
			queueFamilyIndices.push_back(_queueIndices.presentFamily);

		// Create a VkDeviceQueueCreateInfo for each queue we wish to create
		for (decltype(queueFamilyIndices)::value_type & queueFamilyIndice : queueFamilyIndices)
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
				queueCreateInfo.queueFamilyIndex = queueFamilyIndice,
				// An unsigned integer specifying the number of queues to create in the queue family indicated by queueFamilyIndex
				queueCreateInfo.queueCount = 1,
				// An array of queueCount normalized floating point values, specifying priorities of work that will be submitted to each created queue
				queueCreateInfo.pQueuePriorities = &queuePriority
			};

			queueCreateInfos.push_back(queueCreateInfo);
		}
	}

	// Setup and create the device
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
			deviceCreateInfo.pEnabledFeatures = &deviceFeatures
		};

		VK_CHECK_RESULT(vkCreateDevice(_physicalDevice, &deviceCreateInfo, nullptr, &_device));

		vkGetDeviceQueue(_device, _queueIndices.graphicsFamily, 0, &_queueHandles.graphicsQueue);
		vkGetDeviceQueue(_device, _queueIndices.presentFamily, 0, &_queueHandles.presentQueue);
	}
}

bool
Device::isDeviceSuitable(VkPhysicalDevice device_)
{
	bool extensionsSupported = false;
	bool swapchainAqequate = false;

	findQueueFamilies(device_, VK_QUEUE_GRAPHICS_BIT);
	extensionsSupported = checkDeviceExtensionSupport(device_);

	if (extensionsSupported)
	{
//		sSwapChainSupportDetails swapChainSupport = querySwapChainSupport(device_);
//		swapchainAqequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	return _queueIndices.isComplete();
}

void
Device::findQueueFamilies(VkPhysicalDevice device_, VkQueueFlags flags_)
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

bool
Device::checkDeviceExtensionSupport(VkPhysicalDevice device_)
{
	// List of extensions the device needs to support
	std::vector<const char*> deviceExtensions = {};
	{
		deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	}

	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device_, nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device_, nullptr, &extensionCount, availableExtensions.data());

	for (decltype(deviceExtensions)::value_type & deviceExtension : deviceExtensions)
	{
		bool isPresent = false;

		for (decltype(availableExtensions)::value_type & availableExtension : availableExtensions)
		{
			isPresent |= (deviceExtension == availableExtension.extensionName);
			if (isPresent)
				break;
		}
		if (!isPresent)
			return false;
	}

	return true;
}