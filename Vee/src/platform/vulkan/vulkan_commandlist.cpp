#include "platform/vulkan/vulkan_commandlist.hpp"
#include "platform/vulkan/vulkan_device.hpp"

namespace vee
{
    VulkanCommandList::VulkanCommandList(CommandListInfo props)
    : m_properties(props)
    {
	    VkCommandBufferAllocateInfo allocInfo{};
	    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	    allocInfo.commandPool = VKDevice()->GetLogicalDevice()->GetCommandPool(props.Type);
	    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	    allocInfo.commandBufferCount = 1;

	    VKValidate(vkAllocateCommandBuffers(VKDevice()->GetLogicalDevice()->GetVKDevice(), &allocInfo, &m_commandBuffer));
		VKDevice()->DebugNameResource(VK_OBJECT_TYPE_COMMAND_BUFFER, (uint64_t)m_commandBuffer, "CommandList_" + m_properties.DebugName);
    }

    VulkanCommandList::~VulkanCommandList()
    {
        vkFreeCommandBuffers(VKDevice()->GetLogicalDevice()->GetVKDevice(), VKDevice()->GetLogicalDevice()->GetCommandPool(m_properties.Type), 1, &m_commandBuffer);
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

    void VulkanCommandList::BeginRender(const RenderPassInfo& info)
    {
        VkRenderingInfo renderInfo{};
		renderInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
        renderInfo.renderArea = {
            .offset = {0, 0},
            .extent = {1280, 720}
        };
        renderInfo.layerCount = 1;
        renderInfo.colorAttachmentCount = static_cast<uint32_t>(info.ColorAttachments.size());
        renderInfo.pColorAttachments = info.ColorAttachments.data();
        renderInfo.pDepthAttachment = &info.DepthAttachment;

        vkCmdBeginRendering(m_commandBuffer, &renderInfo);
    }

    void VulkanCommandList::SetViewport(const glm::vec3& size)
    {
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = size.x;
        viewport.height = size.y;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = size.z;
        vkCmdSetViewport(m_commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = { static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y) };
        vkCmdSetScissor(m_commandBuffer, 0, 1, &scissor);
    }

    void VulkanCommandList::EndRender()
    {
        vkCmdEndRendering(m_commandBuffer);
    }
}