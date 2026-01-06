#include "platform/vulkan/vulkan_device_logical_device.hpp"

namespace vee
{
    VulkanLogicalDevice::~VulkanLogicalDevice()
    {
        vkDestroyDevice(m_device, nullptr);
    }

    VulkanLogicalDevice::VulkanLogicalDevice(VulkanPhysicalDevice* physicalDevice)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = 0; // Assumption that 0 is valid.
        queueCreateInfo.queueCount = 3; // Graphics, Compute, Copy

        std::vector<float> queuePriorities(3, 1.0f);
        queueCreateInfo.pQueuePriorities = queuePriorities.data();


        VkPhysicalDeviceFeatures deviceFeatures{};


        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = &queueCreateInfo;
        createInfo.queueCreateInfoCount = 1;

        createInfo.pEnabledFeatures = &deviceFeatures;

        const std::vector<const char*> requiredDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
        createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtensions.size());
        createInfo.ppEnabledExtensionNames = requiredDeviceExtensions.data();

        VKValidate(vkCreateDevice(physicalDevice->GetVKPhysicalDevice(), &createInfo, nullptr, &m_device));

        vkGetDeviceQueue(m_device, 0, 0, &m_queues[QueueType::Graphics]);
        vkGetDeviceQueue(m_device, 0, 1, &m_queues[QueueType::Compute]);
        vkGetDeviceQueue(m_device, 0, 2, &m_queues[QueueType::Copy]);
    }
}