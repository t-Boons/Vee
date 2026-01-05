#pragma once
#include "core/device.hpp"
#include "vulkan/vulkan.hpp"

namespace vee
{
    
    class VulkanDevice : public Device
    {  
    public:
        VulkanDevice();
        ~VulkanDevice() override;

    private:
        std::vector<const char*> GetExtentions(bool validationLayers);

        void SetupDebugMessenger();

        VkDevice m_device;
        VkPhysicalDevice m_physicalDevice;
        VkInstance m_instance;
        VkDebugUtilsMessengerEXT m_debugMessenger;
        bool m_enableValidationLayers;
    };

    static VulkanDevice* g_vkDevice;

    static VulkanDevice* VKDevice()
    {
        return g_vkDevice;
    }
}