#include "common.hpp"

namespace vee
{
    enum class ShaderType
    {
        Vertex,
        Fragment,
        Compute,
    };

    class IShader : public NonCopyable
    {
    public:
        IShader(ShaderType type, const std::string& filepath) : m_type(type), m_filepath(filepath) {}
        virtual ~IShader() = default;
    protected:
        ShaderType m_type;
        std::string m_filepath;
    };
}