#include "platform/vulkan/vulkan_device.hpp"
#include "platform/vulkan/vulkan_common.hpp"

#include "platform/vulkan/vulkan_device_instance.hpp"

namespace vee
{
    VulkanDevice* g_vkDevice = nullptr;

    VulkanDevice *vee::VKDevice()
    {
        CheckMsg(g_vkDevice, "Vulkan Device not initialized.");
        return g_vkDevice;
    }

    VulkanDevice::VulkanDevice(bool enableValidationLayers)
        : m_enableValidationLayers(enableValidationLayers)
    {
        m_instance = new VulkanInstance(m_enableValidationLayers);
        m_physicalDevice = new VulkanPhysicalDevice(m_instance);
        m_device = new VulkanLogicalDevice(m_physicalDevice);

        VmaAllocatorCreateInfo allocatorInfo = {};
        allocatorInfo.instance = m_instance->GetVkInstance();
        allocatorInfo.physicalDevice = m_physicalDevice->GetVKPhysicalDevice();
        allocatorInfo.device = m_device->GetVKDevice();
        allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_2;

        vmaCreateAllocator(&allocatorInfo, &m_allocator);
    }

    VulkanDevice::~VulkanDevice()
    {
        FREE(m_device);
        FREE(m_physicalDevice);
        FREE(m_instance);
    }
}