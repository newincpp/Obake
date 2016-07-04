#include "VkRenderer.hh"

//#include "debug.h"

using namespace System;

VkRenderer::VkRenderer(bool enableValidation_)
{
	if (enableValidation_)
	{
		PRINT("##INIT DEBUG");
		VkDebug::instance().initDebugging(VK_DEBUG_REPORT_FLAG_BITS_MAX_ENUM_EXT);
	}
	PRINT("##INIT INSTANCE");
	initVulkan(enableValidation_);
	//	if (enableValidation_)
	//	{
	//		setupConsole("VulkanExample");
	//	}
}

VkRenderer::~VkRenderer()
{

}

// Event System
void
VkRenderer::initialize()
{
	PRINT("Initialise");
	return;

	// Create & bind an event
	_core->eventsManager.bindEvent("Vulkan Event", this, &VkRenderer::vulkanEvent);
	_core->eventsManager.bindEvent("Vulkan Event", this, &VkRenderer::sendWinHandleEvent);
	// Call window event
	_core->eventsManager.executeEvent<void>("Window Event");

	OBAKE_ADD(&VkRenderer::eInitVulkan);
}

void
VkRenderer::registerCore(Obake::Core* core_)
{
	ASystem::registerCore(core_);
}

void
VkRenderer::unload()
{

}

void
VkRenderer::vulkanEvent()
{
	std::cout << "VULKAN EVENT" << std::endl;
}

void
VkRenderer::eInitVulkan()
{
	//initVulkan(true);

	_core->eventsManager.executeEvent<void>("Create Window");
	_core->eventsManager.executeEvent<void>("Get Windows Handle");

	initSwapchain();
}

void
VkRenderer::sendWinHandleEvent(HWND winHandle_, HINSTANCE winInstance_)
{
	_hwnd = winHandle_;
	_hInstance = winInstance_;
	std::cout << "### GET WIN HANDLE ### winHandle : " << _hwnd << " winInstance : " << _hInstance << "###" << std::endl;
}

void
VkRenderer::createSurface()
{

}
// - - !Event System

// Init Stuff
void
VkRenderer::initVulkan(bool enableValidation_)
{
	VkResult err;

	// Creating a Vulkan Instance
	{
		err = createInstance(enableValidation_);
		if (err)
		{
			vkTools::exitFatal("Could not create Vulkan instance : \n" + vkTools::errorString(err), "Fatal error");
		}
	}

	// Creating a Vulkan Device
	{
		// Physical device
		uint32_t gpuCount = 0;
		// Get number of available physical devices
		VK_CHECK_RESULT(vkEnumeratePhysicalDevices(_instance, &gpuCount, nullptr));
		assert(gpuCount > 0);
		// Enumerate devices
		std::vector<VkPhysicalDevice> physicalDevices(gpuCount);
		err = vkEnumeratePhysicalDevices(_instance, &gpuCount, physicalDevices.data());
		if (err)
		{
			vkTools::exitFatal("Could not enumerate phyiscal devices : \n" + vkTools::errorString(err), "Fatal error");
		}

		// Note :
		// This example will always use the first physical device reported,
		// change the vector index if you have multiple Vulkan devices installed
		// and want to use another one
		_physicalDevice = physicalDevices[0];

		// Find a queue that supports graphics operations
		uint32_t graphicsQueueIndex = 0;
		uint32_t queueCount;
		vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &queueCount, NULL);
		assert(queueCount >= 1);

		std::vector<VkQueueFamilyProperties> queueProps;
		queueProps.resize(queueCount);
		vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &queueCount, queueProps.data());

		for (graphicsQueueIndex = 0; graphicsQueueIndex < queueCount; graphicsQueueIndex++)
		{
			if (queueProps[graphicsQueueIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT)
				break;
		}
		assert(graphicsQueueIndex < queueCount);

		_graphicsQueueIndex = graphicsQueueIndex;

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

		// Vulkan device
		std::array<float, 1> queuePriorities = { 0.0f };
		VkDeviceQueueCreateInfo queueCreateInfo =
		{
			// (MANDATORY) sType is the type of the structure
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			// (MANDATORY) pNext is NULL or a pointer to an extension-specific structure.
			queueCreateInfo.pNext = NULL,
			// - Is reserved for future use -
			queueCreateInfo.flags = 0,
			// An unsigned integer indicating the index of the queue family to create on this device.
			queueCreateInfo.queueFamilyIndex = graphicsQueueIndex,
			// The number of queues to create in the queue family
			queueCreateInfo.queueCount = 1,
			// An array of queueCount normalized floating point values, specifying priorities of work that will be submitted to each created queue.
			queueCreateInfo.pQueuePriorities = queuePriorities.data()
		};

		VK_CHECK_RESULT(createDevice(queueCreateInfo, _enableValidation));
	}

	// Checking the physical device's features
	{
		// Store properties (including limits) and features of the phyiscal device
		// So examples can check against them and see if a feature is actually supported
		vkGetPhysicalDeviceProperties(_physicalDevice, &_deviceProperties);
		vkGetPhysicalDeviceFeatures(_physicalDevice, &_deviceFeatures);

		// Gather physical device memory properties
		vkGetPhysicalDeviceMemoryProperties(_physicalDevice, &_deviceMemoryProperties);

		// Get the graphics queue
		vkGetDeviceQueue(_device, _graphicsQueueIndex, 0, &_queue);

		// Find a suitable depth format
		VkBool32 validDepthFormat = vkTools::getSupportedDepthFormat(_physicalDevice, &_depthFormat);
		assert(validDepthFormat);

		_swapChain.connect(_instance, _physicalDevice, _device);
	}

	// Creating semaphores for synchronization
	{
		// Create synchronization objects
		VkSemaphoreCreateInfo semaphoreCreateInfo = vkTools::initializers::semaphoreCreateInfo();
		// Create a semaphore used to synchronize image presentation
		// Ensures that the image is displayed before we start submitting new commands to the queu
		VK_CHECK_RESULT(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_semaphores.presentComplete));
		// Create a semaphore used to synchronize command submission
		// Ensures that the image is not presented until all commands have been sumbitted and executed
		VK_CHECK_RESULT(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_semaphores.renderComplete));
		// Create a semaphore used to synchronize command submission
		// Ensures that the image is not presented until all commands for the text overlay have been sumbitted and executed
		// Will be inserted after the render complete semaphore if the text overlay is enabled
		VK_CHECK_RESULT(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_semaphores.textOverlayComplete));

		// Set up submit info structure
		// Semaphores will stay the same during application lifetime
		// Command buffer submission info is set by each example
		_submitInfo = vkTools::initializers::submitInfo();
		_submitInfo.pWaitDstStageMask = &_submitPipelineStages;
		_submitInfo.waitSemaphoreCount = 1;
		_submitInfo.pWaitSemaphores = &_semaphores.presentComplete;
		_submitInfo.signalSemaphoreCount = 1;
		_submitInfo.pSignalSemaphores = &_semaphores.renderComplete;
	}
}


void
VkRenderer::initSwapchain()
{
	_swapChain.initSurface(_hInstance, _hwnd);
}
// - - !Init stuff

// Setup Stuff

void
VkRenderer::setupConsole(std::string title)
{
#ifdef _WIN32
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	FILE *stream;
	freopen_s(&stream, "CONOUT$", "w+", stdout);
	SetConsoleTitle(TEXT(title.c_str()));
	if (_enableValidation)
	{
		std::cout << "Validation enabled:\n";
	}
#endif
}

void
VkRenderer::setupSwapChain()
{
	_swapChain.create(_setupCmdBuffer, &width, &height);
}

void
VkRenderer::createCommandBuffers()
{
	// Create one command buffer per frame buffer
	// in the swap chain
	// Command buffers store a reference to the
	// frame buffer inside their render pass info
	// so for static usage withouth having to rebuild
	// them each frame, we use one per frame buffer

	_drawCmdBuffers.resize(_swapChain.imageCount);

	VkCommandBufferAllocateInfo cmdBufAllocateInfo =
		vkTools::initializers::commandBufferAllocateInfo(
			_cmdPool,
			VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			(uint32_t)_drawCmdBuffers.size());

	VK_CHECK_RESULT(vkAllocateCommandBuffers(_device, &cmdBufAllocateInfo, _drawCmdBuffers.data()));

	// Command buffers for submitting present barriers
	cmdBufAllocateInfo.commandBufferCount = 1;
	// Pre present
	VK_CHECK_RESULT(vkAllocateCommandBuffers(_device, &cmdBufAllocateInfo, &_prePresentCmdBuffer));
	// Post present
	VK_CHECK_RESULT(vkAllocateCommandBuffers(_device, &cmdBufAllocateInfo, &_postPresentCmdBuffer));
}

// - - !Setup Stuff

// Create Stuff

VkResult
VkRenderer::createInstance(bool enableValidation_)
{
	_enableValidation = enableValidation_;

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
		applicationInfo.pEngineName = nullptr,
		// A developer-supplied version number of the engine
		applicationInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0),
		// (MANDATORY) The version of the Vulkan API that is being used
		applicationInfo.apiVersion = VK_MAKE_VERSION(1, 0, 13)
	};

	std::vector<const char*> enabledExtensions = {};

	// Enable surface extensions depending on os
	enabledExtensions.push_back(PLATFORM_SURFACE_EXTENSION_NAME);
	enabledExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
	if (enableValidation_)
	{
		enabledExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	}
	std::cout << "[SECONF] If the following string is empty, then FUCK YOU : " << VkDebug::instance()._debugCallbackCreateInfo.sType << std::endl;

	// Contains info in regards to the Vulkan instance
	VkInstanceCreateInfo instanceCreateInfo =
	{
		// (MANDATORY) sType is the type of the structure
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		// (MANDATORY) pNext is NULL or a pointer to an extension-specific structure.
		instanceCreateInfo.pNext = &(VkDebug::instance()._debugCallbackCreateInfo),
		// - Is reserved for future use -
		instanceCreateInfo.flags = 0,
		// A pointer to a valid VkApplicationInfo structure
		instanceCreateInfo.pApplicationInfo = &applicationInfo,
		// The number of layers that are enabled
		instanceCreateInfo.enabledLayerCount = (_enableValidation) ? (VkDebug::instance()._validationLayerCount) : (0),
		// An array of pointer that contains the names of the layers to activate
		instanceCreateInfo.ppEnabledLayerNames = (_enableValidation) ? (VkDebug::instance()._validationLayerNames.data()) : (nullptr),
		// The number of extensions that are enabled
		instanceCreateInfo.enabledExtensionCount = enabledExtensions.size(),
		// An array of pointers that contains the names of the extensions to activate
		instanceCreateInfo.ppEnabledExtensionNames = enabledExtensions.data()
	};

	return vkCreateInstance(&instanceCreateInfo, nullptr, &_instance);
}

VkResult
VkRenderer::createDevice(VkDeviceQueueCreateInfo requestedQueues_, bool enableValidation_)
{
	std::vector<const char*> enabledExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

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
		deviceCreateInfo.pQueueCreateInfos = &requestedQueues_,
		// (DEPRECATED)
		deviceCreateInfo.enabledLayerCount = (_enableValidation) ? (VkDebug::instance()._validationLayerCount) : (0),
		// (DEPRECATED)
		deviceCreateInfo.ppEnabledLayerNames = (_enableValidation) ? (VkDebug::instance()._validationLayerNames.data()) : (nullptr),
		// The number of extensions that are enabled
		deviceCreateInfo.enabledExtensionCount = enabledExtensions.size(),
		// An array of pointers that contains the names of the extensions to activate
		deviceCreateInfo.ppEnabledExtensionNames = enabledExtensions.data(),
		// Is NULL or a pointer to a VkPhysicalDeviceFeatures structure that contains boolean indicators of all the features to be enabled
		deviceCreateInfo.pEnabledFeatures = NULL
	};

	// enable the debug marker extension if it is present (likely meaning a debugging tool is present)
	if (vkTools::checkDeviceExtensionPresent(_physicalDevice, VK_EXT_DEBUG_MARKER_EXTENSION_NAME))
	{
		enabledExtensions.push_back(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);
		_enableDebugMarkers = true;
	}

	return vkCreateDevice(_physicalDevice, &deviceCreateInfo, nullptr, &_device);
}

void
VkRenderer::createCommandPool()
{
	VkCommandPoolCreateInfo cmdPoolInfo =
	{
		// (MANDATORY) sType is the type of the structure
		cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		// (MANDATORY) pNext is NULL or a pointer to an extension-specific structure.
		cmdPoolInfo.pNext = nullptr,
		// A bitmask indicating usage behavior for the pool and command buffers allocated from it
		cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		// (NOTE) All command buffers created from this command pool must be submitted on queues from the same queue family.
		cmdPoolInfo.queueFamilyIndex = _graphicsQueueIndex
	};

	VK_CHECK_RESULT(vkCreateCommandPool(_device, &cmdPoolInfo, nullptr, &_cmdPool));
}

void
VkRenderer::createSetupCommandBuffer()
{
	if (_setupCmdBuffer != VK_NULL_HANDLE)
	{
		vkFreeCommandBuffers(_device, _cmdPool, 1, &_setupCmdBuffer);
		_setupCmdBuffer = VK_NULL_HANDLE; // todo : check if still necessary
	}

	VkCommandBufferAllocateInfo cmdBufAllocateInfo =
		vkTools::initializers::commandBufferAllocateInfo(
			_cmdPool,
			VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			1);

	VK_CHECK_RESULT(vkAllocateCommandBuffers(_device, &cmdBufAllocateInfo, &_setupCmdBuffer));

	VkCommandBufferBeginInfo cmdBufInfo = {};
	cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	VK_CHECK_RESULT(vkBeginCommandBuffer(_setupCmdBuffer, &cmdBufInfo));
}

// - - !Create Stuff

void
VkRenderer::prepare()
{
	if (_enableValidation)
	{
		std::cout << "Setting up debug" << std::endl;
		VkDebug::instance().setupDebugging(_instance, VK_NULL_HANDLE);
	}
	//	if (_enableDebugMarkers)
	//	{
	//		std::cout << "Setting up Markers" << std::endl;
	//		_vkDebug.DebugMarker::setup(_device);
	//	}
	createCommandPool();
	createSetupCommandBuffer();
	setupSwapChain();
	createCommandBuffers();
}

// # # WIN32 # #


// Vulkan tests
/*
{
auto device = vkRender._device;
auto queue = vkRender._queue;

// SYNCHRONISATION VAR
// Waits for the GPU to be ready on the CPU side
VkFence fence;
{
VkFenceCreateInfo fenceCreateInfo =
{
// (MANDATORY) sType is the type of the structure
fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
// (MANDATORY) pNext is NULL or a pointer to an extension-specific structure.
fenceCreateInfo.pNext = nullptr,
// A flag that defines the initial state and behavior of the fence. Either signaled or unsignaled.
fenceCreateInfo.flags = 0
};
vkCreateFence(device, &fenceCreateInfo, nullptr, &fence);
}

// Tells the GPU when other process in the GPU are completed
VkSemaphore semaphore;
{
VkSemaphoreCreateInfo semaphoreCreateInfo =
{
// (MANDATORY) sType is the type of the structure
semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
// (MANDATORY) pNext is NULL or a pointer to an extension-specific structure.
semaphoreCreateInfo.pNext = nullptr,
// Flag reserved for future use
semaphoreCreateInfo.flags = 0
};

vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &semaphore);
}
// - - -

VkCommandBuffer commandBuffer[2];
{
VkCommandBufferAllocateInfo commandBufferInfo =
{
// (MANDATORY) sType is the type of the structure
commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
// (MANDATORY) pNext is NULL or a pointer to an extension-specific structure.
commandBufferInfo.pNext = nullptr,
// The command pool that will be used to allocate the command buffer(s)
commandBufferInfo.commandPool = commandPool,
// Determines whether the command buffers are primary or secondary command buffers. Secondary command buffers are contained in primary command buffers.
commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
// The number of command buffers you want to allocate
commandBufferInfo.commandBufferCount = 2
};

vkAllocateCommandBuffers(device, &commandBufferInfo, commandBuffer);
}
// - - -

// RECORDING IN COMMAND BUFFER(S)
{
{
VkCommandBufferBeginInfo beginInfo =
{
// (MANDATORY) sType is the type of the structure
beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
// (MANDATORY) pNext is NULL or a pointer to an extension-specific structure.
beginInfo.pNext = nullptr,
// A bitmask indicating usage behavior for the command buffer.
beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
// A pointer to a VkCommandBufferInheritanceInfo structure, which is used if commandBuffer is a secondary command buffer.
beginInfo.pInheritanceInfo = nullptr
};
vkBeginCommandBuffer(commandBuffer[0], &beginInfo);

vkCmdPipelineBarrier(
commandBuffer[0],
VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
0,
0, nullptr,
0, nullptr,
0, nullptr);

// Filling the command buffer
{
VkViewport viewport =
{
viewport.maxDepth = 1.0f,
viewport.minDepth = 0.0f,
viewport.width = 512,
viewport.height = 512,
viewport.x = 0,
viewport.y = 0
};
vkCmdSetViewport(commandBuffer[0], 0, 1, &viewport);
}

vkEndCommandBuffer(commandBuffer[0]);
}

{
VkCommandBufferBeginInfo beginInfo =
{
// (MANDATORY) sType is the type of the structure
beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
// (MANDATORY) pNext is NULL or a pointer to an extension-specific structure.
beginInfo.pNext = nullptr,
// A bitmask indicating usage behavior for the command buffer.
beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
// A pointer to a VkCommandBufferInheritanceInfo structure, which is used if commandBuffer is a secondary command buffer.
beginInfo.pInheritanceInfo = nullptr
};
vkBeginCommandBuffer(commandBuffer[1], &beginInfo);

VkViewport viewport{};
viewport.maxDepth = 1.0f;
viewport.minDepth = 0.0f;
viewport.width = 512;
viewport.height = 512;
viewport.x = 0;
viewport.y = 0;
vkCmdSetViewport(commandBuffer[1], 0, 1, &viewport);

vkEndCommandBuffer(commandBuffer[1]);
}

{
VkSubmitInfo submitInfo =
{
// (MANDATORY) sType is the type of the structure
submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
// (MANDATORY) pNext is NULL or a pointer to an extension-specific structure.
submitInfo.pNext = nullptr,
// The number of semaphores upon which to wait before executing the command buffers for the batch.
submitInfo.waitSemaphoreCount = 0,
// A pointer to an array of semaphores upon which to wait before executing the command buffers in the batch
submitInfo.pWaitSemaphores = nullptr,
// A pointer to an array of pipeline stages at which each corresponding semaphore wait will occur.
submitInfo.pWaitDstStageMask = nullptr,
// Contains the number of command buffers to execute in the batch
submitInfo.commandBufferCount = 1,
// A pointer to an array of command buffers to execute in the batch.
// The command buffers submitted in a batch begin execution in the order they appear in pCommandBuffers, but may complete out of order
submitInfo.pCommandBuffers = &commandBuffer[0],
// The number of semaphores to be signaled once the commands specified in pCommandBuffers have completed execution.
submitInfo.signalSemaphoreCount = 1,
// A pointer to an array of semaphores which will be signaled when the command buffers for this batch have completed execution.
submitInfo.pSignalSemaphores = &semaphore
};

vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
}
{
VkPipelineStageFlags flags[]{ VK_PIPELINE_STAGE_ALL_COMMANDS_BIT };
VkSubmitInfo submitInfo =
{
// (MANDATORY) sType is the type of the structure
submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
// (MANDATORY) pNext is NULL or a pointer to an extension-specific structure.
submitInfo.pNext = nullptr,
// The number of semaphores upon which to wait before executing the command buffers for the batch.
submitInfo.waitSemaphoreCount = 1,
// A pointer to an array of semaphores upon which to wait before executing the command buffers in the batch
submitInfo.pWaitSemaphores = &semaphore,
// A pointer to an array of pipeline stages at which each corresponding semaphore wait will occur.
submitInfo.pWaitDstStageMask = flags,
// Contains the number of command buffers to execute in the batch
submitInfo.commandBufferCount = 1,
// A pointer to an array of command buffers to execute in the batch.
// The command buffers submitted in a batch begin execution in the order they appear in pCommandBuffers, but may complete out of order
submitInfo.pCommandBuffers = &commandBuffer[1],
// The number of semaphores to be signaled once the commands specified in pCommandBuffers have completed execution.
submitInfo.signalSemaphoreCount = 1,
// A pointer to an array of semaphores which will be signaled when the command buffers for this batch have completed execution.
submitInfo.pSignalSemaphores = &semaphore
};

vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
}
}

// Synchronisation
{
//			vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);
vkQueueWaitIdle(queue);
}

// Freeing up things that have been allocated
{
vkDestroyCommandPool(device, commandPool, nullptr);
vkDestroyFence(device, fence, nullptr);
vkDestroySemaphore(device, semaphore, nullptr);
}
}
*/

HWND VkRenderer::setupWindow(HINSTANCE hinstance, WNDPROC wndproc)
{
	this->_hInstance = hinstance;

	bool fullscreen = false;

	// Check command line arguments
	for (int32_t i = 0; i < __argc; i++)
	{
		if (__argv[i] == std::string("-fullscreen"))
		{
			fullscreen = true;
		}
	}

	WNDCLASSEX wndClass;

	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = wndproc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hinstance;
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = "Window";
	wndClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

	if (!RegisterClassEx(&wndClass))
	{
		std::cout << "Could not register window class!\n";
		fflush(stdout);
		exit(1);
	}

	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	if (fullscreen)
	{
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = screenWidth;
		dmScreenSettings.dmPelsHeight = screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		if ((width != screenWidth) && (height != screenHeight))
		{
			if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
			{
				if (MessageBox(NULL, "Fullscreen Mode not supported!\n Switch to window mode?", "Error", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
				{
					fullscreen = FALSE;
				}
				else
				{
					return FALSE;
				}
			}
		}

	}

	DWORD dwExStyle;
	DWORD dwStyle;

	if (fullscreen)
	{
		dwExStyle = WS_EX_APPWINDOW;
		dwStyle = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	}

	RECT windowRect;
	if (fullscreen)
	{
		windowRect.left = (long)0;
		windowRect.right = (long)screenWidth;
		windowRect.top = (long)0;
		windowRect.bottom = (long)screenHeight;
	}
	else
	{
		windowRect.left = (long)screenWidth / 2 - width / 2;
		windowRect.right = (long)width;
		windowRect.top = (long)screenHeight / 2 - height / 2;
		windowRect.bottom = (long)height;
	}

	AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

	std::string windowTitle = "Window";
	_hwnd = CreateWindowEx(0,
		"Window",
		windowTitle.c_str(),
		dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		windowRect.left,
		windowRect.top,
		windowRect.right,
		windowRect.bottom,
		NULL,
		NULL,
		hinstance,
		NULL);

	if (!_hwnd)
	{
		printf("Could not create window!\n");
		fflush(stdout);
		return 0;
		exit(1);
	}

	ShowWindow(_hwnd, SW_SHOW);
	SetForegroundWindow(_hwnd);
	SetFocus(_hwnd);

	return _hwnd;
}

void VkRenderer::handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	/*
	switch (uMsg)
	{
	case WM_CLOSE:
		prepared = false;
		DestroyWindow(hWnd);
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		ValidateRect(window, NULL);
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 0x50:
			paused = !paused;
			break;
		case VK_F1:
			if (enableTextOverlay)
			{
				textOverlay->visible = !textOverlay->visible;
			}
			break;
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		}
		keyPressed((uint32_t)wParam);
		break;
	case WM_RBUTTONDOWN:
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
		mousePos.x = (float)LOWORD(lParam);
		mousePos.y = (float)HIWORD(lParam);
		break;
	case WM_MOUSEWHEEL:
	{
		short wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		zoom += (float)wheelDelta * 0.005f * zoomSpeed;
		viewChanged();
		break;
	}
	case WM_MOUSEMOVE:
		if (wParam & MK_RBUTTON)
		{
			int32_t posx = LOWORD(lParam);
			int32_t posy = HIWORD(lParam);
			zoom += (mousePos.y - (float)posy) * .005f * zoomSpeed;
			mousePos = glm::vec2((float)posx, (float)posy);
			viewChanged();
		}
		if (wParam & MK_LBUTTON)
		{
			int32_t posx = LOWORD(lParam);
			int32_t posy = HIWORD(lParam);
			rotation.x += (mousePos.y - (float)posy) * 1.25f * rotationSpeed;
			rotation.y -= (mousePos.x - (float)posx) * 1.25f * rotationSpeed;
			mousePos = glm::vec2((float)posx, (float)posy);
			viewChanged();
		}
		if (wParam & MK_MBUTTON)
		{
			int32_t posx = LOWORD(lParam);
			int32_t posy = HIWORD(lParam);
			cameraPos.x -= (mousePos.x - (float)posx) * 0.01f;
			cameraPos.y -= (mousePos.y - (float)posy) * 0.01f;
			viewChanged();
			mousePos.x = (float)posx;
			mousePos.y = (float)posy;
		}
		break;
	case WM_SIZE:
		if ((prepared) && (wParam != SIZE_MINIMIZED))
		{
			destWidth = LOWORD(lParam);
			destHeight = HIWORD(lParam);
			if ((wParam == SIZE_MAXIMIZED) || (wParam == SIZE_MINIMIZED))
			{
				windowResize();
			}
		}
		break;
	case WM_EXITSIZEMOVE:
		if ((prepared) && ((destWidth != width) || (destHeight != height)))
		{
			windowResize();
		}
		break;
	}
	*/
}

void VkRenderer::dieDieDie()
{
	vkDestroyDevice(_device, nullptr);
	VkDebug::instance().dieDieDie();
}