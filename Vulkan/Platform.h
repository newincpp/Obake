#pragma once

#ifdef _WIN32

#include <Windows.h>
#define VK_USE_PLATFORM_WIN32_KHR

#elif __linux__
#define VK_USE_PLATFORM_XCB_KHR

#endif

//#include <vulkan\vulkan.h>
#include "vulkan\vulkan.h"
