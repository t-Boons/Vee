#include "platform/vulkan/vulkan_commandlist.hpp"
#include "platform/vulkan/vulkan_device.hpp"

namespace vee
{
    VulkanCommandList::VulkanCommandList(QueueType type)
    : m_type(type)
    {
	    VkCommandBufferAllocateInfo allocInfo{};
	    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	    allocInfo.commandPool = VKDevice()->GetLogicalDevice()->GetCommandPool(type);
	    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	    allocInfo.commandBufferCount = 1;

	    VKValidate(vkAllocateCommandBuffers(VKDevice()->GetLogicalDevice()->GetVKDevice(), &allocInfo, &m_commandBuffer));
    }

    VulkanCommandList::~VulkanCommandList()
    {
        vkFreeCommandBuffers(VKDevice()->GetLogicalDevice()->GetVKDevice(), VKDevice()->GetLogicalDevice()->GetCommandPool(m_type), 1, &m_commandBuffer);
    }

    void VulkanCommandList::Reset()
    {
        VKValidate(vkResetCommandBuffer(m_commandBuffer, 0));
    }

    void VulkanCommandList::Begin()
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0;
        beginInfo.pInheritanceInfo = nullptr;

        VKValidate(vkBeginCommandBuffer(m_commandBuffer, &beginInfo));
    }

    void VulkanCommandList::End()
    {
        VKValidate(vkEndCommandBuffer(m_commandBuffer));
    }

    void VulkanCommandList::BeginRenderPass(const RenderPassInfo& info)
    {
        VkRenderPassBeginInfo renderPassBeginInfo{};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = info.AttachmentLayout->GetRenderPass();
        renderPassBeginInfo.framebuffer = info.RenderTarget;
        renderPassBeginInfo.renderArea.offset = {0, 0};
        renderPassBeginInfo.renderArea.extent = {1280, 720};

        VkClearValue clearColor = {};
        clearColor.color.float32[0] = info.ClearColor.r;
        clearColor.color.float32[1] = info.ClearColor.g;
        clearColor.color.float32[2] = info.ClearColor.b;
        clearColor.color.float32[3] = info.ClearColor.a;

        renderPassBeginInfo.clearValueCount = 1;
        renderPassBeginInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(m_commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    void VulkanCommandList::EndRenderPass()
    {
        vkCmdEndRenderPass(m_commandBuffer);
    }
}