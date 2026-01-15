#pragma once
#include "vulkan_common.hpp"
#include "platform/vulkan/vulkan_shader.hpp"

namespace vee
{
    struct VulkanPipelineInfo
    {
        RefPtr<VulkanShader> VertexShader;
        RefPtr<VulkanShader> FragmentShader;
        VkPipelineVertexInputStateCreateInfo VertexInputInfo;
    };

    class VulkanPipeline
    {
        public:
            VulkanPipeline(const VulkanPipelineInfo& info);
            ~VulkanPipeline();

            VkPipeline& GetPipeline() { return m_graphicsPipeline; }
            VkRenderPass& GetRenderPass() { return m_renderPass; }

        private:
            VkRenderPass m_renderPass;
            VkPipeline m_graphicsPipeline;
            VulkanPipelineInfo m_info;
    };
}