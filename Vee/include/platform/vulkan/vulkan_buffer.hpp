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

        VkBuffer GetVKBuffer() const { return m_buffer; } 
        VkDescriptorBufferInfo GetVKDescriptorBufferInfo() const;

		uint32_t GetSize() const { return m_properties.Size; }

    private:
        VkBuffer m_buffer;
        VmaAllocation m_allocation;
        BufferProperties m_properties;
    };
}