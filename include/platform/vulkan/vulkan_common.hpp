#pragma once
#include "core/common.hpp"
#include "vulkan/vulkan.hpp"

#define VKValidate(x) \
    if (x != VK_SUCCESS) \
    { \
        vee::Log::Error("Vulkan error: %d", x); \
    }
