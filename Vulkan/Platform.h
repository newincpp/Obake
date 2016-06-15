#pragma once

#ifdef _WIN32

#define VK_USE_PKATFORM_WIN32_KHR 1
#include <Windows.h>

#elif __linux__

#define VK_USE_PLATFORM_XCB_KHR 1
#include <xcb/xcb.h>

#endif

//#include <vulkan\vulkan.h>
#include "vulkan\vulkan.h"
