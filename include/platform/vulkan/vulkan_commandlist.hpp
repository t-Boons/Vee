
#pragma once
#include "vulkan_common.hpp"
#include "vulkan_device_logical_device.hpp"
#include "vulkan_pipeline.hpp"
#include "vulkan_attachment_layout.hpp"
#include <glm/glm.hpp>

namespace vee
{
    struct RenderPassInfo
    {
        float DepthClearValue = 1.0f;
        uint8_t StencilClearValue = 0;
        glm::vec4 ClearColor;
        VkFramebuffer RenderTarget;
        RefPtr<VulkanPipeline> Pipeline;
        RefPtr<VulkanAttachmentLayout> AttachmentLayout;
    };

    class VulkanCommandList
    {
    public:
        VulkanCommandList(QueueType type);
        ~VulkanCommandList();

        void Reset();

        void Begin();
        void End();

        void BeginRenderPass(const RenderPassInfo& info);
        void EndRenderPass();


        VkCommandBuffer& GetVKCommandBuffer() { return m_commandBuffer; }
    private:
        VkCommandBuffer m_commandBuffer;
        QueueType m_type;
    };
}