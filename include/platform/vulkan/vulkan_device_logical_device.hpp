#pragma once
#include "platform/vulkan/vulkan_common.hpp"
#include "platform/vulkan/vulkan_device_instance.hpp"
#include "platform/vulkan/vulkan_device_physical_device.hpp"


namespace vee
{
    enum class QueueType
    {
        Graphics,
        Compute,
        Copy
    };

    class VulkanLogicalDevice
    {
    public:
        VulkanLogicalDevice(VulkanPhysicalDevice* physicalDevice);
        ~VulkanLogicalDevice();
        VkDevice& GetVKDevice() { return m_device; }
        VkQueue& GetQueue(QueueType type) { return m_queues[type]; }
        VkCommandPool& GetCommandPool(QueueType type) {return m_commandPools[type]};

    private:
        std::unordered_map<QueueType, VkQueue> m_queues;
        std::unordered_map<QueueType, VkCommandPool> m_commandPools;
        VkDevice m_device;
    };
}