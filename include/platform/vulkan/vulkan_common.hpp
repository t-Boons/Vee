#pragma once
#include "core/common.hpp"

#define VKValidate(x) \
    if (x != VK_SUCCESS) \
    { \
        vee::Log::Assert(false, "Vulkan error: %d", x); \
    }

namespace vee
{
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
    {
        switch(messageSeverity)
        {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                Log::Info("VK Validation layer: %s", pCallbackData->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                Log::Info("VK Validation layer: %s", pCallbackData->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                Log::Warn("VK Validation layer: %s", pCallbackData->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                Log::Error("VK Validation layer: %s", pCallbackData->pMessage);
                break;
            default:
                Log::Info("VK Validation layer: %s", pCallbackData->pMessage);
                break;
        }
        return VK_FALSE;
    }
}