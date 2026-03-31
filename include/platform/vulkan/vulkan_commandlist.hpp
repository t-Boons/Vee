
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

        void Reset();

        void Begin();
        void End();


        VkCommandBuffer& GetVKCommandBuffer() { return m_commandBuffer; }
    private:
        VkCommandBuffer m_commandBuffer;
        QueueType m_type;
    };
}