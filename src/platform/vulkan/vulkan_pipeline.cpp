#include "platform/vulkan/vulkan_pipeline.hpp"
#include "platform/vulkan/vulkan_device.hpp"
#include "platform/vulkan/vulkan_attachment_layout.hpp"

namespace vee
{
    VkPipelineRasterizationStateCreateInfo CreateDefaultRasterizer()
    {
        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f;
        rasterizer.depthBiasClamp = 0.0f;
        rasterizer.depthBiasSlopeFactor = 0.0f;
        return rasterizer;
    }

    VkPipelineViewportStateCreateInfo CreateDefaultViewportState()
    {
        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = nullptr;
        viewportState.scissorCount = 1;
        viewportState.pScissors = nullptr;
        return viewportState;
    }

    VkPipelineColorBlendAttachmentState CreateDefaultColorBlendAttachment()
    {
        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; 
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; 
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; 

        return colorBlendAttachment;
    }

    VkPipelineColorBlendStateCreateInfo CreateDefaultColorBlending(VkPipelineColorBlendAttachmentState& colorBlendAttachment)
    {
        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f; 
        colorBlending.blendConstants[3] = 0.0f;

        return colorBlending;
    }

    VkPipelineMultisampleStateCreateInfo CreateDefaultMultisampling()
    {
        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading = 1.0f;
        multisampling.pSampleMask = nullptr;
        multisampling.alphaToCoverageEnable = VK_FALSE;
        multisampling.alphaToOneEnable = VK_FALSE;
        return multisampling;
    }

    VkPipelineDynamicStateCreateInfo CreateDefaultDynamicState(std::vector<VkDynamicState>& dynamicStates)
    {
        dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        return dynamicState;
    }

    VkPipelineInputAssemblyStateCreateInfo CreateDefaultInputAssembly()
    {
        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;
        return inputAssembly;
    }

	VkPipelineDepthStencilStateCreateInfo CreateDefaultDepthStencil()
	{
		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = VK_TRUE;
		depthStencil.depthWriteEnable = VK_TRUE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.stencilTestEnable = VK_FALSE;
		return depthStencil;
	}


    VulkanPipeline::VulkanPipeline(const VulkanPipelineInfo& info)
    {
        std::vector<VkDynamicState> dynamicStates;
        VkPipelineDynamicStateCreateInfo dynamicState = CreateDefaultDynamicState(dynamicStates);

        VkPipelineViewportStateCreateInfo viewportState = CreateDefaultViewportState();
        VkPipelineRasterizationStateCreateInfo rasterizer = CreateDefaultRasterizer();

		rasterizer.cullMode = info.Cull ? VK_CULL_MODE_BACK_BIT : VK_CULL_MODE_NONE;

        VkPipelineColorBlendAttachmentState colorBlendAttachment = CreateDefaultColorBlendAttachment();
        VkPipelineColorBlendStateCreateInfo colorBlending = CreateDefaultColorBlending(colorBlendAttachment);
        VkPipelineMultisampleStateCreateInfo multisampling = CreateDefaultMultisampling();
        VkPipelineInputAssemblyStateCreateInfo inputAssembly = CreateDefaultInputAssembly();
		VkPipelineDepthStencilStateCreateInfo depthStencil = CreateDefaultDepthStencil();

		depthStencil.depthWriteEnable = info.EnableDepth ? VK_TRUE : VK_FALSE;
		depthStencil.depthCompareOp = info.EnableDepth ? VK_COMPARE_OP_LESS : VK_COMPARE_OP_ALWAYS;


        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        std::vector<VkPipelineShaderStageCreateInfo> infos = 
        {(info.VertexShader->GetShaderStageInfo()), (info.FragmentShader->GetShaderStageInfo())};
        pipelineInfo.stageCount = (uint32_t)infos.size();
        pipelineInfo.pStages = infos.data();

        VkPipelineVertexInputStateCreateInfo vertexInputInfo = info.VertexInputInfo->GetPipelineVertexInputStateCreateInfo();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = &depthStencil;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(info.DescriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = info.DescriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;

        
        VKValidate(vkCreatePipelineLayout(vee::VKDevice()->GetLogicalDevice()->GetVKDevice(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout));
		VKDevice()->DebugNameResource(VK_OBJECT_TYPE_PIPELINE_LAYOUT, reinterpret_cast<uint64_t>(m_pipelineLayout), "PipelineLayout_" + info.DebugName);

        pipelineInfo.layout = m_pipelineLayout;
        pipelineInfo.renderPass =  VulkanAttachmentLayout::GetDefaultColorAttachment()->GetRenderPass();


	    VKValidate(vkCreateGraphicsPipelines(vee::VKDevice()->GetLogicalDevice()->GetVKDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline));
		VKDevice()->DebugNameResource(VK_OBJECT_TYPE_PIPELINE, reinterpret_cast<uint64_t>(m_graphicsPipeline), "Pipeline_" + info.DebugName);
    }

    VulkanPipeline::~VulkanPipeline()
    {
        vkDestroyPipeline(vee::VKDevice()->GetLogicalDevice()->GetVKDevice(), m_graphicsPipeline, nullptr);
    }
}