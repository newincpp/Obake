#include "MACRO.h"

#include "Tools.hpp"

#include "VkRenderer.hpp"

using namespace System;

OBAKE_PLUGIN(VkRenderer, "Vulkan Renderer", "0.1.2")

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
	_core->eventsManager.bindEvent("Window Update", this, &VkRenderer::evUpdateSurface);

	OBAKE_ADD(initVulkan);
	OBAKE_ADD(mainLoop);
}

void
VkRenderer::unload()
{
	destroySemaphore();
	destroyCommandBuffers();
	destroyCommandPool();
	destroyFramebuffers();
	destroyGraphicsPipeline();
//	destroyShaderModule();
	destroyRenderPass();
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
VkRenderer::evUpdateSurface()
{
	Renderer::evUpdateSurface();

	recreateSwapchain();
}

// # # # # # # # # # # # # # # //

void
VkRenderer::mainLoop()
{
	while (!_isExiting)
	{
		Renderer::mainLoop();
		drawFrame();
		vkDeviceWaitIdle(_device);
	}
}

void
VkRenderer::drawFrame()
{
	uint32_t imageIndex;
	VkSemaphore signalSemaphores[] = { _renderFinishedSemaphore };
	{
		VkSemaphore waitSemaphores[] = { _imageAvailableSemaphore };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		vkAcquireNextImageKHR(_device, _swapchain, std::numeric_limits<uint64_t>::max(), _imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

		VkSubmitInfo submitInfo =
		{
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			submitInfo.pNext = nullptr,
			submitInfo.waitSemaphoreCount = 1,
			submitInfo.pWaitSemaphores = waitSemaphores,
			submitInfo.pWaitDstStageMask = waitStages,
			submitInfo.commandBufferCount = 1,
			submitInfo.pCommandBuffers = &_commandBuffers[imageIndex],
			submitInfo.signalSemaphoreCount = 1,
			submitInfo.pSignalSemaphores = signalSemaphores
		};

		VK_CHECK_RESULT(vkQueueSubmit(_queueHandles.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE));
	}

	{
		VkSwapchainKHR swapChains[] = { _swapchain };
		VkPresentInfoKHR presentInfo =
		{
			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			presentInfo.pNext = nullptr,
			presentInfo.waitSemaphoreCount = 1,
			presentInfo.pWaitSemaphores = signalSemaphores,
			presentInfo.swapchainCount = 1,
			presentInfo.pSwapchains = swapChains,
			presentInfo.pImageIndices = &imageIndex
		};

		VK_CHECK_RESULT(vkQueuePresentKHR(_queueHandles.presentQueue, &presentInfo));
	}
}

// Vulkan

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
	createRenderPass();
	createGraphicsPipeline();
	createFramebuffers();
	createCommandPool();
	createCommandBuffers();
	createSemaphore();
}

void
VkRenderer::recreateSwapchain()
{
	if (_device == VK_NULL_HANDLE)
		return;
	vkDeviceWaitIdle(_device);

	destroyCommandBuffers();
	destroyFramebuffers();
	destroyGraphicsPipeline();
	destroyRenderPass();
	destroyImageViews();
	destroySwapchain();

	createSwapchain();
	createImageViews();
	createRenderPass();
	createGraphicsPipeline();
	createFramebuffers();
	createCommandBuffers();
	drawFrame();
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

void
VkRenderer::createRenderPass()
{
	PRINT("## [VKRENDERER] [" << __FILE__ << "] CREATE RENDER PASS");

	// COMMENT !!!
	VkAttachmentDescription colorAttachment =
	{
		// A bitmask describing additional properties of the attachment
		colorAttachment.flags = 0,
		colorAttachment.format = _swapchainImageFormat,
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT,
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
	};

	VkAttachmentReference colorAttachmentRef =
	{
		// The index of the attachment of the render pass, and corresponds to the index of the corresponding element
		// in the pAttachments array of the VkRenderPassCreateInfo structure
		colorAttachmentRef.attachment = 0,
		// A VkImageLayout value specifying the layout the attachment uses during the subpass.
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};

	// COMMENT !!!
	VkSubpassDescription subPass =
	{
		subPass.flags = 0,
		subPass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
		// The number of input attachments
		subPass.inputAttachmentCount = 0,
		subPass.pInputAttachments = nullptr,
		subPass.colorAttachmentCount = 1,
		subPass.pColorAttachments = &colorAttachmentRef,
		subPass.pResolveAttachments = nullptr,
		subPass.pDepthStencilAttachment = nullptr,
		subPass.preserveAttachmentCount = 0,
		subPass.pPreserveAttachments = nullptr
	};

	// COMMENT !!!
	VkRenderPassCreateInfo renderPassInfo =
	{
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		renderPassInfo.pNext = nullptr,
		renderPassInfo.flags = 0,
		renderPassInfo.attachmentCount = 1,
		renderPassInfo.pAttachments = &colorAttachment,
		renderPassInfo.subpassCount = 1,
		renderPassInfo.pSubpasses = &subPass,
		renderPassInfo.dependencyCount = 0,
		renderPassInfo.pDependencies = nullptr

	};

	VK_CHECK_RESULT(vkCreateRenderPass(_device, &renderPassInfo, nullptr, &_renderPass));
}

void
VkRenderer::destroyRenderPass()
{
	PRINT("## [VKRENDERER] [" << __FILE__ << "] DESTROY RENDER PASS");
	vkDestroyRenderPass(_device, _renderPass, nullptr);
}

void
VkRenderer::createGraphicsPipeline()
{
	PRINT("## [VKRENDERER] [" << __FILE__ << "] CREATE GRAPHICS PIPELINE");

	std::vector<char> vertShaderCode = Tools::readFile("shaders/vert.spv");
	std::vector<char> fragShaderCode = Tools::readFile("shaders/frag.spv");
	createShaderModule(vertShaderCode, _vertShaderModule);
	createShaderModule(fragShaderCode, _fragShaderModule);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo =
	{
		// (MANDATORY) sType is the type of the structure
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		// (MANDATORY) pNext is a nullptr or a pointer to an extension-specific structure.
		vertShaderStageInfo.pNext = nullptr,
		// - Is reserved for future use -
		vertShaderStageInfo.flags = 0,
		// Names a single pipeline stage (VkShaderStageFlagBits)
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT,
		// A VkShaderModule object that contains the shader for this stage
		vertShaderStageInfo.module = _vertShaderModule,
		// A pointer to a null-terminated UTF-8 string specifying the entry point name of the shader for this stage
		vertShaderStageInfo.pName = "main",
		// A pointer to VkSpecializationInfo, as described in Specialization Constants, and can be NULL
		vertShaderStageInfo.pSpecializationInfo = VK_NULL_HANDLE
	};

	VkPipelineShaderStageCreateInfo fragShaderStageInfo =
	{
		// (MANDATORY) sType is the type of the structure
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		// (MANDATORY) pNext is a nullptr or a pointer to an extension-specific structure.
		fragShaderStageInfo.pNext = nullptr,
		// - Is reserved for future use -
		fragShaderStageInfo.flags = 0,
		// Names a single pipeline stage (VkShaderStageFlagBits)
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
		// A VkShaderModule object that contains the shader for this stage
		fragShaderStageInfo.module = _fragShaderModule,
		// A pointer to a null-terminated UTF-8 string specifying the entry point name of the shader for this stage
		fragShaderStageInfo.pName = "main",
		// A pointer to VkSpecializationInfo, as described in Specialization Constants, and can be NULL
		fragShaderStageInfo.pSpecializationInfo = VK_NULL_HANDLE
	};

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	// Describes the format of the vertex data that will be passed to the vertex shader
	VkPipelineVertexInputStateCreateInfo vertexInputInfo =
	{
		// (MANDATORY) sType is the type of the structure
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		// (MANDATORY) pNext is a nullptr or a pointer to an extension-specific structure.
		vertexInputInfo.pNext = nullptr,
		// - Is reserved for future use -
		vertexInputInfo.flags = 0,
		// The number of vertex binding descriptions provided in pVertexBindingDescriptions
		vertexInputInfo.vertexBindingDescriptionCount = 0,
		// A pointer to an array of VkVertexInputBindingDescription structures
		vertexInputInfo.pVertexBindingDescriptions = nullptr,
		// The number of vertex attribute descriptions provided in pVertexAttributeDescriptions
		vertexInputInfo.vertexAttributeDescriptionCount = 0,
		// A pointer to an array of VkVertexInputAttributeDescriptions structures
		vertexInputInfo.pVertexAttributeDescriptions = nullptr
	};

	// Mainly describes what kind of geometry will be drawn from the vetices
	VkPipelineInputAssemblyStateCreateInfo inputAssembly =
	{
		// (MANDATORY) sType is the type of the structure
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		// (MANDATORY) pNext is a nullptr or a pointer to an extension-specific structure.
		inputAssembly.pNext = nullptr,
		// - Is reserved for future use -
		inputAssembly.flags = 0,
		// Defines the primitive topology
		// Primitive topology determines how consecutive vertices are organized into primitives
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		// Controls whether a special vertex index value is treated as restarting the assembly of primitives.
		inputAssembly.primitiveRestartEnable = VK_FALSE
	};

	VkViewport viewport =
	{
		// The viewport's upper left corner
		viewport.x = 0.0f,
		viewport.y = 0.0f,
		// The viewort's width and height0
		viewport.width = (float)_swapchainExtent.width,
		viewport.height = (float)_swapchainExtent.height,
		// The min and max depth range for the viewport
		viewport.minDepth = 0.0f,
		viewport.maxDepth = 1.0f
	};

	VkRect2D scissor =
	{
		scissor.offset = { 0, 0 },
		scissor.extent = _swapchainExtent
	};

	VkPipelineViewportStateCreateInfo viewportState =
	{
		// (MANDATORY) sType is the type of the structure
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		// (MANDATORY) pNext is a nullptr or a pointer to an extension-specific structure.
		viewportState.pNext = nullptr,
		// - Is reserved for future use -
		viewportState.flags = 0,
		// The number of viewports used by the pipeline
		viewportState.viewportCount = 1,
		// A pointer to an array of VkViewport structures, defining the viewport transforms
		viewportState.pViewports = &viewport,
		// The number of scissors (must match the number of viewports)
		viewportState.scissorCount = 1,
		// A pointer to an array of VkRect2D structures which define the rectangle bounds of the scissor for the corresponding viewport
		viewportState.pScissors = &scissor
	};

	VkPipelineRasterizationStateCreateInfo rasterizer =
	{
		// (MANDATORY) sType is the type of the structure
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		// (MANDATORY) pNext is a nullptr or a pointer to an extension-specific structure.
		rasterizer.pNext = nullptr,
		// - Is reserved for future use -
		rasterizer.flags = 0,
		// Controls whether to clamp the fragment’s depth values instead of clipping primitives to the z planes of the frustum
		rasterizer.depthClampEnable = VK_FALSE,
		// Controls whether primitives are discarded immediately before the rasterization stage
		rasterizer.rasterizerDiscardEnable = VK_FALSE,
		// The triangle rendering mode
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL,
		// The triangle facing direction used for primitive culling
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT,
		// the front-facing triangle orientation to be used for culling
		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE,
		// Controls whether to bias fragment depth values
		rasterizer.depthBiasEnable = VK_FALSE,
		// A scalar factor controlling the constant depth value added to each fragment.
		rasterizer.depthBiasConstantFactor = 0,
		// The maximum (or minimum) depth bias of a fragment.
		rasterizer.depthBiasClamp = 0,
		// A scalar factor applied to a fragment’s slope in depth bias calculations
		rasterizer.depthBiasSlopeFactor = 0,
		// The width of rasterized line segments
		rasterizer.lineWidth = 1.0f
	};

	VkPipelineMultisampleStateCreateInfo multisampling =
	{
		// (MANDATORY) sType is the type of the structure
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		// (MANDATORY) pNext is a nullptr or a pointer to an extension-specific structure.
		multisampling.pNext = nullptr,
		// - Is reserved for future use -
		multisampling.flags = 0,
		// A VkSampleCountFlagBits specifying the number of samples per pixel used in rasterization
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
		// Specifies that fragment shading executes per-sample if VK_TRUE, or per-fragment if VK_FALSE
		multisampling.sampleShadingEnable = VK_FALSE,
		// The minimum fraction of sample shading
		multisampling.minSampleShading = 0,
		// A bitmask of static coverage information that is ANDed with the coverage information generated during rasterization
		multisampling.pSampleMask = nullptr,
		// Controls whether a temporary coverage value is generated based on the alpha component of the fragment’s first color output
		multisampling.alphaToCoverageEnable = VK_FALSE,
		// Controls whether the alpha component of the fragment’s first color output is replaced with one
		multisampling.alphaToOneEnable = VK_FALSE
	};

	// COMMENT !!!
	VkPipelineColorBlendAttachmentState colorBlendAttachment =
	{
		colorBlendAttachment.blendEnable = VK_FALSE,
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO,
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD,
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD,
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT

	};

	// COMMENT !!!
	VkPipelineColorBlendStateCreateInfo colorBlending =
	{
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		colorBlending.pNext = nullptr,
		colorBlending.flags = 0,
		colorBlending.logicOpEnable = VK_FALSE,
		colorBlending.logicOp = VK_LOGIC_OP_COPY,
		colorBlending.attachmentCount = 1,
		colorBlending.pAttachments = &colorBlendAttachment,
		colorBlending.blendConstants[0] = 0.0f,
		colorBlending.blendConstants[1] = 0.0f,
		colorBlending.blendConstants[2] = 0.0f,
		colorBlending.blendConstants[3] = 0.0f
	};

	// COMMENT !!!
	VkPipelineLayoutCreateInfo pipelineLayoutInfo =
	{
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		pipelineLayoutInfo.pNext = nullptr,
		pipelineLayoutInfo.flags = 0,
		pipelineLayoutInfo.setLayoutCount = 0,
		pipelineLayoutInfo.pSetLayouts = nullptr,
		pipelineLayoutInfo.pushConstantRangeCount = 0,
		pipelineLayoutInfo.pPushConstantRanges = nullptr,
	};

	VK_CHECK_RESULT(vkCreatePipelineLayout(_device, &pipelineLayoutInfo, nullptr, &_pipelineLayout));

	VkGraphicsPipelineCreateInfo pipelineInfo =
	{
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		pipelineInfo.pNext = nullptr,
		pipelineInfo.flags = 0,
		pipelineInfo.stageCount = 2,
		pipelineInfo.pStages = shaderStages,
		pipelineInfo.pVertexInputState = &vertexInputInfo,
		pipelineInfo.pInputAssemblyState = &inputAssembly,
		pipelineInfo.pTessellationState = nullptr,
		pipelineInfo.pViewportState = &viewportState,
		pipelineInfo.pRasterizationState = &rasterizer,
		pipelineInfo.pMultisampleState = &multisampling,
		pipelineInfo.pDepthStencilState = nullptr,
		pipelineInfo.pColorBlendState = &colorBlending,
		pipelineInfo.pDynamicState = nullptr,
		pipelineInfo.layout = _pipelineLayout,
		pipelineInfo.renderPass = _renderPass,
		pipelineInfo.subpass = 0,
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE
	};

	VK_CHECK_RESULT(vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_graphicsPipeline))
}

void
VkRenderer::destroyGraphicsPipeline()
{
	PRINT("## [VKRENDERER] [" << __FILE__ << "] DESTROY GRAPHICS PIPELINE");
	vkDestroyPipelineLayout(_device, _pipelineLayout, nullptr);
	vkDestroyPipeline(_device, _graphicsPipeline, nullptr);
	destroyShaderModule();
}

void
VkRenderer::createShaderModule(const std::vector<char>& code, VkShaderModule& shaderModule)
{
	VkShaderModuleCreateInfo createInfo =
	{
		// (MANDATORY) sType is the type of the structure
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		// (MANDATORY) pNext is a nullptr or a pointer to an extension-specific structure.
		createInfo.pNext = nullptr,
		// - Is reserved for future use -
		createInfo.flags = 0,
		// The size, in bytes, of the code pointed to by pCode
		createInfo.codeSize = code.size(),
		// Points to code that is used to create the shader module.
		// The type and format of the code is determined from the content of the memory addressed by pCode
		createInfo.pCode = (uint32_t*)code.data()
	};

	VK_CHECK_RESULT(vkCreateShaderModule(_device, &createInfo, nullptr, &shaderModule));
}

void
VkRenderer::destroyShaderModule()
{
	vkDestroyShaderModule(_device, _vertShaderModule, nullptr);
	vkDestroyShaderModule(_device, _fragShaderModule, nullptr);
}

void
VkRenderer::createFramebuffers()
{
	PRINT("## [VKRENDERER] [" << __FILE__ << "] CREATE FRAMEBUFFERS");

	_swapchainFramebuffers.resize(_swapchainImageViews.size());

	for (size_t i = 0; i < _swapchainImageViews.size(); i++)
	{
		VkImageView attachments[] = { _swapchainImageViews[i] };

		VkFramebufferCreateInfo framebufferInfo =
		{
			// (MANDATORY) sType is the type of the structure
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			// (MANDATORY) pNext is a nullptr or a pointer to an extension-specific structure.
			framebufferInfo.pNext = nullptr,
			// - Is reserved for future use -
			framebufferInfo.flags = 0,
			// A handle to a compatible renderPass (uses the same number and type of attachements)
			framebufferInfo.renderPass = _renderPass,
			// The number of attachments
			framebufferInfo.attachmentCount = 1,
			// An array of VkImageView handles, each of which will be used as the corresponding attachment in a render pass instance
			framebufferInfo.pAttachments = attachments,
			// The dimensions of the framebuffer
			framebufferInfo.width = _swapchainExtent.width,
			framebufferInfo.height = _swapchainExtent.height,
			framebufferInfo.layers = 1
		};

		VK_CHECK_RESULT(vkCreateFramebuffer(_device, &framebufferInfo, nullptr, &_swapchainFramebuffers[i]));
	}

}

void
VkRenderer::destroyFramebuffers()
{
	PRINT("## [VKRENDERER] [" << __FILE__ << "] DESTROY FRAMEBUFFERS");

	for (decltype(_swapchainFramebuffers)::value_type framebuffer : _swapchainFramebuffers)
		vkDestroyFramebuffer(_device, framebuffer, nullptr);
}

void System::VkRenderer::createCommandPool()
{
	PRINT("## [VKRENDERER] [" << __FILE__ << "] CREATE COMMAND POOL");

	VkCommandPoolCreateInfo cmdPoolInfo =
	{
		// (MANDATORY) sType is the type of the structure
		cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		// (MANDATORY) pNext is NULL or a pointer to an extension-specific structure.
		cmdPoolInfo.pNext = nullptr,
		// A bitmask indicating usage behavior for the pool and command buffers allocated from it
		cmdPoolInfo.flags = 0,
		// (NOTE) All command buffers created from this command pool must be submitted on queues from the same queue family.
		cmdPoolInfo.queueFamilyIndex = _queueIndices.graphicsFamily
	};

	VK_CHECK_RESULT(vkCreateCommandPool(_device, &cmdPoolInfo, nullptr, &_commandPool));
}

void System::VkRenderer::destroyCommandPool()
{
	PRINT("## [VKRENDERER] [" << __FILE__ << "] DESTROY COMMAND POOL");

	vkDestroyCommandPool(_device, _commandPool, nullptr);
}

void System::VkRenderer::createCommandBuffers()
{
	PRINT("## [VKRENDERER] [" << __FILE__ << "] ALLOCATE COMMAND BUFFERS");

	_commandBuffers.resize(_swapchainFramebuffers.size());

	VkCommandBufferAllocateInfo allocInfo = 
	{
		// (MANDATORY) sType is the type of the structure
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		// (MANDATORY) pNext is NULL or a pointer to an extension-specific structure.
		allocInfo.pNext = nullptr,
		// The name of the command pool that the command buffers allocate their memory from
		allocInfo.commandPool = _commandPool,
		// Determines whether the command buffers are primary or secondary command buffers
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		// The number of command buffers to allocate from the pool
		allocInfo.commandBufferCount = (uint32_t)_commandBuffers.size()
	};

	vkAllocateCommandBuffers(_device, &allocInfo, _commandBuffers.data());

	{
		for (size_t i = 0; i < _commandBuffers.size(); i++)
		{
			VkCommandBufferBeginInfo beginInfo =
			{
				// (MANDATORY) sType is the type of the structure
				beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
				// (MANDATORY) pNext is NULL or a pointer to an extension-specific structure.
				beginInfo.pNext = nullptr,
				//  A bitmask indicating usage behavior for the command buffer
				beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
				// A pointer to a VkCommandBufferInheritanceInfo structure, which is used if commandBuffer is a secondary command buffer
				beginInfo.pInheritanceInfo = nullptr
			};

			vkBeginCommandBuffer(_commandBuffers[i], &beginInfo);
			{
				VkRenderPassBeginInfo renderPassInfo =
				{
					// (MANDATORY) sType is the type of the structure
					renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
					// (MANDATORY) pNext is NULL or a pointer to an extension-specific structure.
					renderPassInfo.pNext = nullptr,
					// The render pass to begin an instance of
					renderPassInfo.renderPass = _renderPass,
					// The framebuffer containing the attachments that are used with the render pass
					renderPassInfo.framebuffer = _swapchainFramebuffers[i],
					// The render area that is affected by the render pass instance
					renderPassInfo.renderArea.offset = { 0, 0 },
					renderPassInfo.renderArea.extent = _swapchainExtent,
					// The number of elements in pClearValues
					renderPassInfo.clearValueCount = 0,
					// An array of VkClearValue structures that contains clear values for each attachment, if the attachment uses a loadOp value of
					// VK_ATTACHMENT_LOAD_OP_CLEAR or if the attachment has a depth/stencil format and uses a stencilLoadOp value of VK_ATTACHMENT_LOAD_OP_CLEAR
					renderPassInfo.pClearValues = nullptr
				};

				VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
				renderPassInfo.clearValueCount = 1;
				renderPassInfo.pClearValues = &clearColor;

				vkCmdBeginRenderPass(_commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
				{
					vkCmdBindPipeline(_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline);

					vkCmdDraw(_commandBuffers[i], 3, 1, 0, 0);
				}
				vkCmdEndRenderPass(_commandBuffers[i]);
			}
			VK_CHECK_RESULT(vkEndCommandBuffer(_commandBuffers[i]));
		}
	}
}

void System::VkRenderer::destroyCommandBuffers()
{
	PRINT("## [VKRENDERER] [" << __FILE__ << "] FREE COMMAND BUFFERS");

	vkFreeCommandBuffers(_device, _commandPool, _commandBuffers.size(), _commandBuffers.data());
}

void System::VkRenderer::createSemaphore()
{
	PRINT("## [VKRENDERER] [" << __FILE__ << "] CREATE SEMAPHORE");

	VkSemaphoreCreateInfo semaphoreInfo =
	{
		// (MANDATORY) sType is the type of the structure
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		// (MANDATORY) pNext is a nullptr or a pointer to an extension-specific structure.
		semaphoreInfo.pNext = nullptr,
		// - Is reserved for future use -
		semaphoreInfo.flags = 0
	};

	VK_CHECK_RESULT(vkCreateSemaphore(_device, &semaphoreInfo, nullptr, &_imageAvailableSemaphore));
	VK_CHECK_RESULT(vkCreateSemaphore(_device, &semaphoreInfo, nullptr, &_renderFinishedSemaphore));
}

void System::VkRenderer::destroySemaphore()
{
	PRINT("## [VKRENDERER] [" << __FILE__ << "] DESTROY SEMAPHORE");

	vkDestroySemaphore(_device, _imageAvailableSemaphore, nullptr);
	vkDestroySemaphore(_device, _renderFinishedSemaphore, nullptr);
}

// !Vulkan