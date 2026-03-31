#include "platform/vulkan/vulkan_attachment_layout.hpp"
#include "platform/vulkan/vulkan_device.hpp"

namespace vee
{
    VkAttachmentDescription CreateDefaultColorAttachment()
    {
    	VkAttachmentDescription colorAttachment{};
        colorAttachment.format = VK_FORMAT_B8G8R8A8_SRGB;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        return colorAttachment;
    }

    VulkanAttachmentLayout::VulkanAttachmentLayout()
    {
        auto colorAttachment = CreateDefaultColorAttachment();

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;

        VKValidate(vkCreateRenderPass(VKDevice()->GetLogicalDevice()->GetVKDevice(), &renderPassInfo, nullptr, &m_renderPass));
    }

    VulkanAttachmentLayout::~VulkanAttachmentLayout()
    {
        vkDestroyRenderPass(VKDevice()->GetLogicalDevice()->GetVKDevice(), m_renderPass, nullptr);
    }

    RefPtr<VulkanAttachmentLayout> VulkanAttachmentLayout::GetDefaultColorAttachment()
    {
        if (!s_defaultColorAttachment)
            s_defaultColorAttachment = MakeRef<VulkanAttachmentLayout>();

        return s_defaultColorAttachment;
    }
}