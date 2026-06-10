#include "platform/vulkan/vulkan_shader.hpp"
#include "platform/vulkan/vulkan_device.hpp"

namespace vee
{
    VulkanShader::VulkanShader(ShaderType type, const std::string &filepath)
        : Shader(type, filepath)
    {
        // Load SPIR-V code from file.
        std::ifstream file(filepath, std::ios::ate | std::ios::binary);

        CheckMsg(file.is_open(), "Failed to open shader file: %s working directory: %s", filepath.c_str(), std::filesystem::current_path().string().c_str());

        const uint32_t fileSize = static_cast<uint32_t>(file.tellg());
        m_spirvCode.resize(fileSize);

        file.seekg(0);
        file.read(m_spirvCode.data(), fileSize);
        file.close();
        Log::Info("Loaded shader: %s (%d bytes)", filepath.c_str(), fileSize);


        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = m_spirvCode.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(m_spirvCode.data());

        VKValidate(vkCreateShaderModule(VKDevice()->GetLogicalDevice()->GetVKDevice(), &createInfo, nullptr, &m_shaderModule));
		std::string shaderName = std::filesystem::path(filepath).stem().string();
		VKDevice()->DebugNameResource(VK_OBJECT_TYPE_SHADER_MODULE, (uint64_t)m_shaderModule, "Shader_" + shaderName);  

        VkShaderStageFlagBits stage = VK_SHADER_STAGE_ALL;
        switch (type)
        {
        case ShaderType::Vertex:
            stage = VK_SHADER_STAGE_VERTEX_BIT;
            break;
        case ShaderType::Fragment:
            stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            break;
        default:
            break;
        }

        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = stage;
        vertShaderStageInfo.module = m_shaderModule;
        vertShaderStageInfo.pName = "main";
        m_shaderStageInfo = vertShaderStageInfo;
    }

    VulkanShader::~VulkanShader()
    {
        vkDestroyShaderModule(VKDevice()->GetLogicalDevice()->GetVKDevice(), m_shaderModule, nullptr);
    }

}