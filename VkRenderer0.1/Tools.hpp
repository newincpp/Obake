#pragma once

#include "MACRO.h"
#include "PLATFORM.h"

// Macro to check and display Vulkan return results
#define VK_CHECK_RESULT(f)																												\
{																																		\
	VkResult res = (f);																													\
	if (res != VK_SUCCESS)																												\
	{																																	\
		std::cout << "Fatal : VkResult is \"" << Tools::errorString(res) << "\" in " << __FILE__ << " at line " << __LINE__ << '\n';	\
		assert(res == VK_SUCCESS);																										\
	}																																	\
}

namespace Tools
{
	std::string errorString(VkResult errorCode);
	std::vector<char> readFile(const std::string & filename_);
}
