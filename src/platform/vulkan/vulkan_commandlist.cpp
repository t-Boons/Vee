#include "platform/vulkan/vulkan_commandlist.hpp"
#include "platform/vulkan/vulkan_device.hpp"

namespace vee
{
    VulkanCommandList::VulkanCommandList(QueueType type)
    : m_type(type)
    {
        VkCommandBuffer commandBuffer;

	    VkCommandBufferAllocateInfo allocInfo{};
	    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	    allocInfo.commandPool = VKDevice()->GetLogicalDevice()->GetCommandPool(type);
	    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	    allocInfo.commandBufferCount = 1;

	    VKValidate(vkAllocateCommandBuffers(VKDevice()->GetLogicalDevice()->GetVKDevice(), &allocInfo, &commandBuffer));
    }

    VulkanCommandList::~VulkanCommandList()
    {
        vkFreeCommandBuffers(VKDevice()->GetLogicalDevice()->GetVKDevice(), VKDevice()->GetLogicalDevice()->GetCommandPool(m_type), 1, &commandBuffer);
    }
}