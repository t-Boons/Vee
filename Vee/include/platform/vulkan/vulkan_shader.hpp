#pragma once
#include "vulkan_common.hpp"
#include "core/shader.hpp"

namespace vee
{
    class VulkanShader : public Shader
    {
    public:
        VulkanShader(ShaderType type, const std::string& filepath);
        ~VulkanShader() override;

        VkPipelineShaderStageCreateInfo GetShaderStageInfo() const {return m_shaderStageInfo; }

    private:
        std::vector<char> m_spirvCode;
        VkShaderModule m_shaderModule;
        VkPipelineShaderStageCreateInfo m_shaderStageInfo;
    };
}   