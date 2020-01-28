#pragma once

//platform specific
#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#pragma comment(linker, "/subsystem:windows")
#define WIN32_LEAN_AND_MEAN  
#include <windows.h>
#include <fcntl.h>
#include <io.h>
#elif defined(__ANDROID__)
#define VK_USE_PLATFORM_ANDROID_KHR
#define VK_NO_PROTOTYPES
#include <android/native_activity.h>
#include <android/asset_manager.h>
#include <InternalVulkan/vulkan_android.hpp>
#include <InternalVulkan/android_native_app_glue.h>
#include <cstdlib>
#include <math.h>
#endif

//std
#include <string>
#include <locale>
#include <memory>
#include <vector>
#include <cassert>
#include <cstdint>
#include <unordered_map>
#include <functional>

//vulkan
#include <vulkan/vulkan.h>
#include <VulkanMemoryAllocator/src/vk_mem_alloc.h>

//halcyonic
#include <halcyonic_debug.hpp>

#ifdef _WIN32
#include<codecvt>
#endif