#pragma once

#include "vulkan_common.hpp"

namespace vee
{
    class VulkanAttachmentLayout
    {
    public:
        VulkanAttachmentLayout();
        ~VulkanAttachmentLayout();

        VkRenderPass& GetRenderPass() { return m_renderPass; }
        
        static RefPtr<VulkanAttachmentLayout> GetDefaultColorAttachment();

    private:
        inline static RefPtr<VulkanAttachmentLayout> s_defaultColorAttachment;

        VkRenderPass m_renderPass;
    };
}