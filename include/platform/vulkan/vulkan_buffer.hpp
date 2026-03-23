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
        virtual void Write(void* data, uint32_t size) override;

        VkBuffer GetVKBuffer() const { return m_buffer; } 

    private:
        VkBuffer m_buffer;
        VmaAllocation m_allocation;
    };
}