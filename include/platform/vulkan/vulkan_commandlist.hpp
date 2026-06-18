
#pragma once
#include "vulkan_common.hpp"
#include "vulkan_device_logical_device.hpp"
#include "vulkan_pipeline.hpp"
#include <glm/glm.hpp>

namespace vee
{
    struct RenderPassInfo
    {
        float DepthClearValue = 1.0f;
        uint8_t StencilClearValue = 0;
        glm::vec4 ClearColor;
        std::vector<VkRenderingAttachmentInfo> ColorAttachments;
        VkRenderingAttachmentInfo DepthAttachment;
    };

	struct CommandListInfo
	{
        QueueType Type = QueueType::Graphics;
		std::string DebugName = "CommandList";
	};

    class VulkanCommandList
    {
    public:
        VulkanCommandList(CommandListInfo props = CommandListInfo());
        ~VulkanCommandList();

        void Reset();

        void Begin();
        void End();

        void BeginRender(const RenderPassInfo& info);
        void EndRender();

        void SetViewport(const glm::vec3& size);

        VkCommandBuffer& GetVKCommandBuffer() { return m_commandBuffer; }
    private:
        VkCommandBuffer m_commandBuffer;
        CommandListInfo m_properties;
    };
}