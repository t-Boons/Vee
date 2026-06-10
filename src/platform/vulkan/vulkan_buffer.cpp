#include "platform/vulkan/vulkan_buffer.hpp"
#include "platform/vulkan/vulkan_device.hpp"
#include "platform/vulkan/vulkan_commandlist.hpp"

namespace vee
{
    namespace utils
    {
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

        inline VkBufferUsageFlags GetVulkanBufferUsageFlagBits(BufferUsage usage)
        {
            switch (usage)
            {
                case BufferUsage::Vertex:
                    return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
                case BufferUsage::Index:
                    return VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
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
    allocInfo.preferredFlags = utils::GetVulkanMemoryPropertyFlags(properties.MemoryType);
    allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

    VKValidate(vmaCreateBuffer(VKDevice()->GetAllocator(), &bufferInfo, &allocInfo, &m_buffer, &m_allocation, nullptr));
	VKDevice()->DebugNameResource(VK_OBJECT_TYPE_BUFFER, (uint64_t)m_buffer, "Buffer_" + properties.DebugName);

    // If GPU only create a staging buffer to upload data.
    if (m_properties.MemoryType == MemoryType::Static)
    {
        CheckMsg(m_properties.Data != nullptr, "Static buffers must have initial data. (BufferProperties.Data cannot be null)");

        BufferProperties stagingBufferProperties = m_properties;
        stagingBufferProperties.MemoryType = MemoryType::Dynamic;
        stagingBufferProperties.Usage = BufferUsage::TransferSrc;
        stagingBufferProperties.DebugName = "Straging_" + properties.DebugName;
        VulkanBuffer *stagingBuffer = new VulkanBuffer(stagingBufferProperties);
        void *stagingBufferData = stagingBuffer->Map();
        memcpy(stagingBufferData, m_properties.Data, m_properties.Size);
        stagingBuffer->UnMap();

        VulkanCommandList list({ QueueType::Graphics, "Straging_" + properties.DebugName });

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(list.GetVKCommandBuffer(), &beginInfo);

        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        copyRegion.size = properties.Size;

        vkCmdCopyBuffer(list.GetVKCommandBuffer(), stagingBuffer->GetVKBuffer(), m_buffer, 1, &copyRegion);

        vkEndCommandBuffer(list.GetVKCommandBuffer());

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &list.GetVKCommandBuffer();

        vkQueueSubmit(VKDevice()->GetLogicalDevice()->GetQueue(QueueType::Graphics), 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(VKDevice()->GetLogicalDevice()->GetQueue(QueueType::Graphics));
    }

	if (properties.Data && properties.MemoryType == MemoryType::Dynamic)
	{
		void* mappedData = Map();
		memcpy(mappedData, properties.Data, properties.Size);
		UnMap();
	}
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

    VkDescriptorBufferInfo VulkanBuffer::GetVKDescriptorBufferInfo() const
    {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = m_buffer;
        bufferInfo.offset = 0;
        bufferInfo.range = m_properties.Size;
        return bufferInfo;
    }
}