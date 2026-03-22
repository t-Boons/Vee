#pragma once

#include "vulkan_common.hpp"
#include "core/buffer.hpp"

namespace vee
{
    class VulkanBuffer : public Buffer
    {
    public:
        VulkanBuffer(const BufferProperties& properties);
        ~VulkanBuffer();

        virtual void* Map() override;
        virtual void UnMap() override;

    private:
        VkDeviceMemory m_memory;
    };
}