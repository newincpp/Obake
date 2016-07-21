#include "MACRO.h"

#include "vulkanTools.h"
#include "vulkanDebug.h"

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
	_enableValidation = VULKAN_DEBUG_LAYER;

	if (_enableValidation)
	{
		PRINT("## INIT DEBUG");
		VkDebug::instance().initDebugging(VK_DEBUG_REPORT_FLAG_BITS_MAX_ENUM_EXT);
	}
	VK_CHECK_RESULT(createInstance(_enableValidation));
}

VkResult
VkRenderer::createInstance(bool enableValidation_)
{	
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
		applicationInfo.apiVersion = VK_MAKE_VERSION(1, 0, 17)
	};
	
	// Enable surface extensions depending on os
	std::vector<const char*> enabledExtensions = {};
	{
		enabledExtensions.push_back(PLATFORM_SURFACE_EXTENSION_NAME);
		enabledExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
		if (enableValidation_)
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