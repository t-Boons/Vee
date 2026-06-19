#pragma once
#include "vulkan_common.hpp"
#include "platform/vulkan/vulkan_device_instance.hpp"

namespace vee
{
    class VulkanPhysicalDevice
    {
    public:
        VulkanPhysicalDevice(VulkanInstance* instance);

        void SelectBestPhysicalDevice(VulkanInstance* instance);
        std::string GetDeviceName() const { return m_deviceProperties.deviceName; }
        VkPhysicalDevice GetVKPhysicalDevice() const { return m_physicalDevice; }
    private:
        VkPhysicalDevice m_physicalDevice;
        VkPhysicalDeviceProperties m_deviceProperties;

    };
}