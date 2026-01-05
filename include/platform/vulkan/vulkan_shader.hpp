#include "core/shader.hpp"

namespace vee
{
    class VulkanShader : public Shader
    {
    public:
        VulkanShader(ShaderType type, const std::string& filepath);
        ~VulkanShader() override {}

    private:
        std::vector<char> m_spirvCode;
    };
}   