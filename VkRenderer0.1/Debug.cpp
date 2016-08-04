#include <iostream>

#include "MACRO.h"

#include "Debug.hpp"

using namespace Platy;

Debug* Debug::s_instance = nullptr;

#ifdef __linux__
void MessageBox(void*, const char* msg, const char* title, unsigned int){
	std::cerr << title << ": " << msg << '\n';
}
#endif

VKAPI_ATTR VkBool32 VKAPI_CALL
messageCallback(
	VkDebugReportFlagsEXT flags,
	VkDebugReportObjectTypeEXT objType,
	uint64_t srcObject,
	size_t location,
	int32_t msgCode,
	const char* pLayerPrefix,
	const char* pMsg,
	void* pUserData)
{
	std::ostringstream stream;

	stream << "VKDBG: ";
	if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
		stream << "INFO: ";
	if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
		stream << "WARNING: ";
	if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
		stream << "PERFORMANCE: ";
	if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
		stream << "ERROR: ";
	if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
		stream << "DEBUG: ";
	stream << "@[" << pLayerPrefix << "]: ";
	stream << pMsg << std::endl;

	std::cout << stream.str();

	if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
	{
		MessageBox(NULL, stream.str().c_str(), "Vulkan Error!", 0);
	}
	return false;
}

Debug::Debug()
{
	_validationLayerNames.push_back("VK_LAYER_LUNARG_standard_validation");
	_validationLayerCount = _validationLayerNames.size();
	setupDebugging();
}

Debug::~Debug()
{

}

void
Debug::setupDebugging(VkDebugReportFlagsEXT flags_)
{
	VkDebugReportCallbackCreateInfoEXT dbgCreateInfo =
	{
		dbgCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT,
		dbgCreateInfo.pNext = nullptr,
		dbgCreateInfo.flags = flags_,
		dbgCreateInfo.pfnCallback = (PFN_vkDebugReportCallbackEXT)messageCallback,
		dbgCreateInfo.pUserData = nullptr
	};

	_debugCallbackCreateInfo = dbgCreateInfo;
}

void
Debug::createDebugging(VkInstance instance_)
{
	PRINT("## [DEBUG] [" << __FUNCTION__ << "] CREATE DEBUGGING");
	CreateDebugReportCallback = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance_, "vkCreateDebugReportCallbackEXT");
	DestroyDebugReportCallback = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance_, "vkDestroyDebugReportCallbackEXT");

	if (CreateDebugReportCallback == VK_NULL_HANDLE || DestroyDebugReportCallback == VK_NULL_HANDLE)
	{
		assert(0 && "## Vulkan ERROR: Can't fetch debug function pointers.");
	}

	VK_CHECK_RESULT(CreateDebugReportCallback(instance_, &_debugCallbackCreateInfo, nullptr, &_msgCallback));
}

void
Debug::freeDebugCallback(VkInstance instance_)
{
	PRINT("## [DEBUG] [" << __FUNCTION__ << "] DESTROY DEBUGGING");
	if (_msgCallback != VK_NULL_HANDLE)
	{
		DestroyDebugReportCallback(instance_, _msgCallback, nullptr);
	}
}


