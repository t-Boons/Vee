#include "common.hpp"
#include "buffer.hpp"

namespace vee
{
    enum class ShaderType
    {
        Vertex,
        Fragment,
        Compute,
    };

    struct ShaderBinding
    {
        RefPtr<Buffer> Buffer;
        uint32_t Index;
    };


    class Shader : public NonCopyable
    {
    public:
        Shader(ShaderType type, const std::string& filepath) : m_type(type), m_filepath(filepath) {}
        virtual ~Shader() = default;
    protected:
        ShaderType m_type;
        std::string m_filepath;
    };
}