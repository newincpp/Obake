#pragma once

#ifdef _WIN32

#define VK_USE_PLATFORM_WIN32_KHR 1
#define PLATFORM_SURFACE_EXTENSION_NAME VK_KHR_WIN32_SURFACE_EXTENSION_NAME

#define NOMINMAX 1
#include <windows.h>

#undef max
#undef min

#include <fcntl.h>
#include <io.h>

#elif __linux__

#define VK_USE_PLATFORM_XCB_KHR 1
#define PLATFORM_SURFACE_EXTENSION_NAME VK_KHR_XCB_SURFACE_EXTENSION_NAME

#include <xcb/xcb.h>

#elif defined(__ANDROID__)

// Not sure if this is correct
#include <android/native_activity.h>
#include <android/asset_manager.h>
#include <android_native_app_glue.h>
#include "vulkanandroid.h"

#else
// Android, PS4, ...
#endif

#include <iostream>
#include <cstdio>
#include <sstream>
#include <fstream>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"
#include "gli/gli.hpp"

#include "vulkan/vulkan.h"
