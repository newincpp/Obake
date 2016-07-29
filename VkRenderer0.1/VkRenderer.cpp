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
	destroyGraphicsPipeline();
	destroyImageViews();
	destroySwapchain();
	destroyDevice();
	destroySurface();
	if (_enableValidation); { _debug.freeDebugCallback(_instance); }
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
	_debug.setupDebugging(VK_DEBUG_REPORT_ERROR_BIT_EXT);
	createInstance();
	if (_enableValidation); { _debug.createDebugging(_instance); }
	createSurface();
	createDevice();
	createSwapchain();
	createImageViews();
	createGraphicsPipeline();
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
		// (MANDATORY) sType is the type of the structure0
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		// (MANDATORY) pNext is a nullptr or a pointer to an extension-specific structure.
		instanceCreateInfo.pNext = (_enableValidation) ? (&(_debug._debugCallbackCreateInfo)) : (nullptr),
		// - Is reserved for future use -
		instanceCreateInfo.flags = 0,
		// A pointer to a valid VkApplicationInfo structure
		instanceCreateInfo.pApplicationInfo = &applicationInfo,
		// The number of layers that are enabled
		instanceCreateInfo.enabledLayerCount = (_enableValidation) ? (_debug._validationLayerNames.size()) : (0),
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

	VK_CHECK_RESULT(vkCreateXcbSurfaceKHR(_instance, &surfaceCreateInfo, nullptr, &_surface));
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
	createLogicalDevice();
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
VkRenderer::createLogicalDevice()
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

		std::vector<const char*> deviceExtensions = {};
		{
			deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		}

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
			deviceCreateInfo.enabledExtensionCount = deviceExtensions.size(),
			// An array of pointers that contains the names of the extensions to activate
			deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data(),
			// Is NULL or a pointer to a VkPhysicalDeviceFeatures structure that contains boolean indicators of all the features to be enabled
			deviceCreateInfo.pEnabledFeatures = &deviceFeatures
		};

		VK_CHECK_RESULT(vkCreateDevice(_physicalDevice, &deviceCreateInfo, nullptr, &_device));

		vkGetDeviceQueue(_device, _queueIndices.graphicsFamily, 0, &_queueHandles.graphicsQueue);
		vkGetDeviceQueue(_device, _queueIndices.presentFamily, 0, &_queueHandles.presentQueue);
	}
}

bool
VkRenderer::isDeviceSuitable(VkPhysicalDevice device_)
{
	bool extensionsSupported = false;
	bool swapchainAqequate = false;

	findQueueFamilies(device_, VK_QUEUE_GRAPHICS_BIT);
	extensionsSupported = checkDeviceExtensionSupport(device_);

	if (extensionsSupported) {
		sSwapChainSupportDetails swapChainSupport = querySwapChainSupport(device_);
		swapchainAqequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
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

bool
VkRenderer::checkDeviceExtensionSupport(VkPhysicalDevice device_)
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

void
VkRenderer::createSwapchain()
{
	PRINT("## [VKRENDERER] [" << __FILE__ << "] CREATE SWAPCHAIN");
	sSwapChainSupportDetails swapChainSupport = querySwapChainSupport(_physicalDevice);

	// Checking to find the best configuration for our swapchain
	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

	// Sets the number of images in our swapchain
	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
	{
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	uint32_t queueFamilyIndices[] = { (uint32_t)_queueIndices.graphicsFamily, (uint32_t)_queueIndices.presentFamily };

	VkSwapchainCreateInfoKHR createInfo =
	{
		// (MANDATORY) sType is the type of the structure0
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		// (MANDATORY) pNext is a nullptr or a pointer to an extension-specific structure.
		createInfo.pNext = nullptr,
		// - Is reserved for future use -
		createInfo.flags = 0,
		// The surface that the swapchain will present images to
		createInfo.surface = _surface,
		// the minimum number of presentable images that the application needs
		createInfo.minImageCount = imageCount,
		// A VkFormat that is valid for swapchains on the specified surface
		createInfo.imageFormat = surfaceFormat.format,
		// A VkColorSpaceKHR that is valid for swapchains on the specified surface
		createInfo.imageColorSpace = surfaceFormat.colorSpace,
		// The size (in pixels) of the swapchain
		createInfo.imageExtent = extent,
		// The number of views in a multiview/stereo surface. For non-stereoscopic-3D applications, this value is 1.
		createInfo.imageArrayLayers = 1,
		// A bitfield of VkImageUsageFlagBits, indicating how the application will use the swapchain’s presentable images
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		// The sharing mode used for the images of the swapchain
		createInfo.imageSharingMode = ((_queueIndices.graphicsFamily != _queueIndices.presentFamily) ? (VK_SHARING_MODE_CONCURRENT) : (VK_SHARING_MODE_EXCLUSIVE)),
		// The number of queue families having access to the images of the swapchain in case imageSharingMode is VK_SHARING_MODE_CONCURRENT
		createInfo.queueFamilyIndexCount = (createInfo.imageSharingMode == VK_SHARING_MODE_CONCURRENT) ? 2 : 0,
		// An array of queue family indices having access to the images of the swapchain in case imageSharingMode is VK_SHARING_MODE_CONCURRENT.
		createInfo.pQueueFamilyIndices = (createInfo.imageSharingMode == VK_SHARING_MODE_CONCURRENT) ? queueFamilyIndices : nullptr,
		// A bitfield of VkSurfaceTransformFlagBitsKHR, describing the transform, relative to the presentation engine’s natural orientation, applied to the image content prior to presentation.
		// If it does not match the currentTransform value returned by vkGetPhysicalDeviceSurfaceCapabilitiesKHR, the presentation engine will transform the image content as part of the presentation operation
		createInfo.preTransform = swapChainSupport.capabilities.currentTransform,
		// A bitfield of VkCompositeAlphaFlagBitsKHR, indicating the alpha compositing mode to use when this surface is composited together with other surfaces on certain window systems.
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		// Ehe presentation mode the swapchain will use
		createInfo.presentMode = presentMode,
		// Indicates whether the Vulkan implementation is allowed to discard rendering operations that affect regions of the surface which aren’t visible.
		createInfo.clipped = VK_TRUE,
		createInfo.oldSwapchain = VK_NULL_HANDLE
	};

	VK_CHECK_RESULT(vkCreateSwapchainKHR(_device, &createInfo, nullptr, &_swapchain));

	vkGetSwapchainImagesKHR(_device, _swapchain, &imageCount, nullptr);
	_swapchainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(_device, _swapchain, &imageCount, _swapchainImages.data());

	_swapchainImageFormat = surfaceFormat.format;
	_swapchainExtent = extent;
}

void
VkRenderer::destroySwapchain()
{
	PRINT("## [VKRENDERER] [" << __FILE__ << "] DESTROY SWAPCHAIN");
	vkDestroySwapchainKHR(_device, _swapchain, nullptr);
}

VkRenderer::sSwapChainSupportDetails
VkRenderer::querySwapChainSupport(VkPhysicalDevice device_)
{
	sSwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device_, _surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device_, _surface, &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device_, _surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device_, _surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device_, _surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

VkSurfaceFormatKHR
VkRenderer::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> & availableFormats_)
{
	if (availableFormats_.size() == 1 && availableFormats_[0].format == VK_FORMAT_UNDEFINED)
	{
		return{ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}

	for (const std::remove_reference<decltype(availableFormats_)>::type::value_type & availableFormat : availableFormats_)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}

	return availableFormats_[0];
}

VkPresentModeKHR
VkRenderer::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> & availablePresentModes_)
{
	for (const std::remove_reference<decltype(availablePresentModes_)>::type::value_type & availablePresentMode : availablePresentModes_) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D
VkRenderer::chooseSwapExtent(const VkSurfaceCapabilitiesKHR & capabilities_)
{
	if (capabilities_.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return capabilities_.currentExtent;
	}
	else
	{
		VkExtent2D actualExtent = { WIDTH, HEIGHT };

		actualExtent.width = glm::max(capabilities_.minImageExtent.width, glm::min(capabilities_.maxImageExtent.width, actualExtent.width));
		actualExtent.height = glm::max(capabilities_.minImageExtent.height, glm::min(capabilities_.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

void
VkRenderer::createImageViews()
{
	PRINT("## [VKRENDERER] [" << __FILE__ << "] CREATE IMAGE VIEWS");

	_swapchainImageViews.resize(_swapchainImages.size());

	for (uint32_t i = 0; i < _swapchainImages.size(); i++) 
	{
		VkImageViewCreateInfo createInfo =
		{
			// (MANDATORY) sType is the type of the structure
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			// (MANDATORY) pNext is NULL or a pointer to an extension-specific structure.
			createInfo.pNext = nullptr,
			// - Is reserved for future use -
			createInfo.flags = 0,
			// A VkImage on which the view will be created
			createInfo.image = _swapchainImages[i],
			// The type of the image view
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D,
			// A VkFormat describing the format and type used to interpret data elements in the image.
			createInfo.format = _swapchainImageFormat,
			// A remapping of color components (or of depth or stencil components after they have been converted into color components)
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY,
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY,
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY,
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY,
			// The set of mipmap levels and array layers to be accessible to the view
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			createInfo.subresourceRange.baseMipLevel = 0,
			createInfo.subresourceRange.levelCount = 1,
			createInfo.subresourceRange.baseArrayLayer = 0,
			createInfo.subresourceRange.layerCount = 1
		};
		VK_CHECK_RESULT(vkCreateImageView(_device, &createInfo, nullptr, &_swapchainImageViews[i]));
	}
}

void
VkRenderer::destroyImageViews()
{
	PRINT("## [VKRENDERER] [" << __FILE__ << "] DESTROY IMAGE VIEWS");

	for (decltype(_swapchainImageViews)::value_type imageView : _swapchainImageViews)
	{
		vkDestroyImageView(_device, imageView, nullptr);
	}
}

void System::VkRenderer::createGraphicsPipeline()
{
	PRINT("## [VKRENDERER] [" << __FILE__ << "] CREATE GRAPHICS PIPELINE");
}

void System::VkRenderer::destroyGraphicsPipeline()
{
	PRINT("## [VKRENDERER] [" << __FILE__ << "] DESTROY GRAPHICS PIPELINE");
}
