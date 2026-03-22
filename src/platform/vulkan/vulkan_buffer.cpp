#include "platform/vulkan/vulkan_device_memory.hpp"
#include "platform/vulkan/vulkan_device.hpp"
#include "vk_mem_alloc.h"

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
    }

    VulkanBuffer::VulkanBuffer(const BufferProperties& properties)
        : Buffer(properties)
    {
        VkMemoryPropertyFlags memoryUsage = utils::GetVulkanMemoryPropertyFlags(properties.MemoryType);

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = properties.Size;
        bufferInfo.usage = memoryUsage;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.requiredFlags = memoryUsage;
        allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                        VMA_ALLOCATION_CREATE_MAPPED_BIT;

        VkBuffer buffer;
        VmaAllocation allocation;
        VmaAllocationInfo allocationInfo;

    }

    VulkanBuffer::~VulkanBuffer()
    {
    }

    void* VulkanBuffer::Map()
    {
        return nullptr;
    }

    void VulkanBuffer::UnMap()
    {
    }
}