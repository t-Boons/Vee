#include "platform/vulkan/vulkan_device.hpp"
#include "platform/vulkan/vulkan_common.hpp"

#include "platform/vulkan/vulkan_device_instance.hpp"

namespace vee
{
    VulkanDevice::VulkanDevice(bool enableValidationLayers)
    : m_enableValidationLayers(enableValidationLayers)
    {
        m_instance = new VulkanInstance(m_enableValidationLayers);
        m_physicalDevice = new VulkanPhysicalDevice(m_instance);

        Log::Info("Vulkan selected GPU: %s", m_physicalDevice->GetDeviceName().c_str());
    }

    VulkanDevice::~VulkanDevice()
    {
        FREE(m_physicalDevice);
        FREE(m_instance);
    }
}
