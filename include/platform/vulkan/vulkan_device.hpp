#pragma once
#include "core/device.hpp"
#include "vulkan/vulkan.hpp"
#include "platform/vulkan/vulkan_device_physical_device.hpp"
#include "platform/vulkan/vulkan_device_logical_device.hpp"

namespace vee
{
    class VulkanDevice : public Device
    {
    public:
        VulkanDevice(bool enableValidationLayers);
        ~VulkanDevice() override;

        std::string GetDeviceName() const { return m_physicalDevice->GetDeviceName(); }

    private:
        VulkanLogicalDevice* m_device;
        VulkanInstance* m_instance;
        VulkanPhysicalDevice* m_physicalDevice;
        bool m_enableValidationLayers;
    };

    extern VulkanDevice* g_vkDevice;
    VulkanDevice* VKDevice();


}