#pragma once
#include "core/device.hpp"
#include "vulkan/vulkan.hpp"

namespace vee
{
    
    class VulkanDevice : public Device
    {  
    public:
        VulkanDevice();
        
    private:
        VkDevice m_device;
        VkPhysicalDevice m_physicalDevice;
        VkInstance m_instance;
    };

    static VulkanDevice* g_vkDevice;

    static VulkanDevice* VKDevice()
    {
        return g_vkDevice;
    }
}