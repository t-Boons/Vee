#pragma once
#include "core/device.hpp"
#include "vulkan_common.hpp"
#include "platform/vulkan/vulkan_device_physical_device.hpp"
#include "platform/vulkan/vulkan_device_logical_device.hpp"

namespace vee
{
    class VulkanDevice : public Device
    {
    public:
        VulkanDevice(bool enableValidationLayers);
        ~VulkanDevice() override;

        VulkanLogicalDevice* GetLogicalDevice() const { return m_device; }
        VulkanPhysicalDevice* GetPhysicalDevice() const { return m_physicalDevice; }
        VulkanInstance* GetInstance() const { return m_instance; }
        bool IsValidationLayersEnabled() const { return m_enableValidationLayers; }


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