#pragma once
#include "vulkan_common.hpp"
#include "platform/vulkan/vulkan_shader.hpp"

namespace vee
{
    class VertexLayout
    {
    public:
        VkPipelineVertexInputStateCreateInfo GetPipelineVertexInputStateCreateInfo()
        {
            m_vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            m_vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(m_bindingDescriptions.size());
            m_vertexInputInfo.pVertexBindingDescriptions = m_bindingDescriptions.data();
            m_vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_attributes.size());
            m_vertexInputInfo.pVertexAttributeDescriptions = m_attributes.data();

            return m_vertexInputInfo;
        }

        std::vector<VkVertexInputAttributeDescription> m_attributes;
        std::vector<VkVertexInputBindingDescription> m_bindingDescriptions{};
    private:
        VkPipelineVertexInputStateCreateInfo m_vertexInputInfo{};
    };


    struct VulkanPipelineInfo
    {
        RefPtr<VulkanShader> VertexShader;
        RefPtr<VulkanShader> FragmentShader;
        RefPtr<VertexLayout> VertexInputInfo;
        std::vector<VkDescriptorSetLayout> DescriptorSetLayouts;

        bool EnableDepth = true;
        bool Cull = true;
    };

    class VulkanPipeline
    {
        public:
            VulkanPipeline(const VulkanPipelineInfo& info);
            ~VulkanPipeline();

            VkPipeline& GetPipeline() { return m_graphicsPipeline; }
            VkRenderPass& GetRenderPass() { return m_renderPass; }
            VkPipelineLayout& GetPipelineLayout() { return m_pipelineLayout; }

        private:
            VkRenderPass m_renderPass;
            VkPipeline m_graphicsPipeline;
            VulkanPipelineInfo m_info;
            VkPipelineLayout m_pipelineLayout;
    };
}