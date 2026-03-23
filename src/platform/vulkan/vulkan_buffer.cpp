#include "platform/vulkan/vulkan_buffer.hpp"
#include "platform/vulkan/vulkan_device.hpp"

namespace vee
{
    namespace utils
    {
        // Taken from https://vulkan-tutorial.com/
        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
        {
            VkPhysicalDeviceMemoryProperties memProperties;
            vkGetPhysicalDeviceMemoryProperties(vee::VKDevice()->GetPhysicalDevice()->GetVKPhysicalDevice(), &memProperties);

            for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
            {
                if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
                {
                    return i;
                }
            }
            return 0;
        }

        inline VkMemoryPropertyFlags GetVulkanMemoryPropertyFlags(MemoryType type)
        {
            switch (type)
            {
                case MemoryType::Static:
                    return VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
                case MemoryType::Dynamic:
                    return VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
                default:
                    return 0;
            }
        }

        inline VkBufferUsageFlagBits GetVulkanBufferUsageFlagBits(BufferUsage usage)
        {
            switch (usage)
            {
                case BufferUsage::Vertex:
                    return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
                case BufferUsage::Index:
                    return VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
                case BufferUsage::Uniform:
                    return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
                case BufferUsage::TransferSrc:
                    return VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
                case BufferUsage::TransferDst:
                    return VK_BUFFER_USAGE_TRANSFER_DST_BIT;
                default:
                    vee::Log::Error("Unknown buffer usage type");
                    return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            }
        }
    }

    VulkanBuffer::VulkanBuffer(const BufferProperties& properties)
        : Buffer(properties)
    {
    VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    bufferInfo.size = properties.Size;
    bufferInfo.usage = utils::GetVulkanBufferUsageFlagBits(properties.Usage);
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = VMA_MEMORY_USAGE_AUTO;

    VKValidate(vmaCreateBuffer(VKDevice()->GetAllocator(), &bufferInfo, &allocInfo, &m_buffer, &m_allocation, nullptr));
}

    VulkanBuffer::~VulkanBuffer()
    {
        vmaDestroyBuffer(VKDevice()->GetAllocator(), m_buffer, m_allocation);
    }

    void* VulkanBuffer::Map()
    {
        Check(m_properties.MemoryType == MemoryType::Dynamic && "Only dynamic buffers can be mapped.");

        void* mappedData;
        VKValidate(vmaMapMemory(VKDevice()->GetAllocator(), m_allocation, &mappedData));
        return mappedData;
    }

    void VulkanBuffer::UnMap()
    {
        vmaUnmapMemory(VKDevice()->GetAllocator(), m_allocation);
    }

    void VulkanBuffer::Write(void* data, uint32_t size)
    {
        Check(size <= m_properties.Size && "Data size exceeds buffer size.");

                // If GPU only create a staging buffer to upload data.
        if (m_properties.MemoryType == MemoryType::Static)
        {
            BufferProperties stagingBufferProperties = m_properties;
            stagingBufferProperties.MemoryType = MemoryType::Dynamic;
            stagingBufferProperties.Usage = BufferUsage::TransferSrc;
            VulkanBuffer* stagingBuffer = new VulkanBuffer(stagingBufferProperties);
            void* stagingBufferData = stagingBuffer->Map();
            memcpy(stagingBufferData, data, size);
            stagingBuffer->UnMap();

            VkBufferCopy copyRegion{};
            copyRegion.size = size;
            vkCmdCopyBuffer(VKDevice()->GetDeviceName()->(), stagingBuffer->GetVKBuffer(), GetVKBuffer(), 1, &copyRegion);
            delete stagingBuffer; // TODO fix this sht.
        }
        else
        {
            void* mappedData = Map();
            memcpy(mappedData, data, size);
            UnMap();
        }
    }
}