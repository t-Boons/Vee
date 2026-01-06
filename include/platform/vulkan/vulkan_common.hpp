#pragma once
#include "core/common.hpp"

#define VK_USE_PLATFORM_WIN32_KHR 
#include "vulkan/vulkan.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#define VKValidate(x) \
    if (x != VK_SUCCESS) \
    { \
        vee::Log::Error("Vulkan error: %d", x); \
        assert(false); \
    }
