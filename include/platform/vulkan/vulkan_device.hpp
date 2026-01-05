#pragma once
#include "core/device.hpp"
#include "vulkan/vulkan.hpp"
#include "platform/vulkan/vulkan_device_physical_device.hpp"

namespace vee
{
    class VulkanDevice : public Device
    {
    public:
        VulkanDevice(bool enableValidationLayers);
        ~VulkanDevice() override;

    private:
        VkDevice m_device;
        VulkanInstance* m_instance;
        VulkanPhysicalDevice* m_physicalDevice;
        bool m_enableValidationLayers;
    };

    static VulkanDevice *g_vkDevice;

    static VulkanDevice *VKDevice()
    {
        return g_vkDevice;
    }
}