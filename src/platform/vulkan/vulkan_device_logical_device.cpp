#include "platform/vulkan/vulkan_device_logical_device.hpp"

namespace vee
{
    VulkanLogicalDevice::~VulkanLogicalDevice()
    {
        vkDestroyDevice(m_device, nullptr);
    }

    VulkanLogicalDevice::VulkanLogicalDevice(VulkanPhysicalDevice *physicalDevice)
    {
        {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = 0; // Assumption that 0 is valid.
            queueCreateInfo.queueCount = 3; // Graphics, Compute, Copy

            std::vector<float> queuePriorities(3, 1.0f);
            queueCreateInfo.pQueuePriorities = queuePriorities.data();

            
            VkDeviceCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            createInfo.pQueueCreateInfos = &queueCreateInfo;
            createInfo.queueCreateInfoCount = 1;
            
            VkPhysicalDeviceFeatures deviceFeatures{};
            createInfo.pEnabledFeatures = &deviceFeatures;

            const std::vector<const char*> requiredDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
            createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtensions.size());
            createInfo.ppEnabledExtensionNames = requiredDeviceExtensions.data();

            VKValidate(vkCreateDevice(physicalDevice->GetVKPhysicalDevice(), &createInfo, nullptr, &m_device));

            vkGetDeviceQueue(m_device, 0, 0, &m_queues[QueueType::Graphics]);
        }

        {
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.queueFamilyIndex = 0;
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	    VKValidate(vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_commandPools[QueueType::Graphics]));
        }

        {
            VkDescriptorPoolSize poolSize{};
            poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            poolSize.descriptorCount = 1;

            VkDescriptorPoolCreateInfo poolInfo{};
            poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            poolInfo.poolSizeCount = 1;
            poolInfo.pPoolSizes = &poolSize;
            poolInfo.maxSets = 1;
            
            vkCreateDescriptorPool(m_device, &poolInfo, nullptr, &m_descriptorPool);
        }
    }
}