
#pragma once
#include "vulkan_common.hpp"
#include "vulkan_device_logical_device.hpp"

namespace vee
{
    class VulkanCommandList
    {
    public:
        VulkanCommandList(QueueType type);
        ~VulkanCommandList();
        VkCommandBuffer& GetVKCommandBuffer() { return commandBuffer; }
    private:
        VkCommandBuffer commandBuffer;
        QueueType m_type;
    }
}