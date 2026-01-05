#include "platform/vulkan/vulkan_shader.hpp"

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
    }

}