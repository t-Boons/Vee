#pragma once

#include "common.hpp"
#include "shader.hpp"
#include "buffer.hpp"

namespace vee
{
    struct PipelineStateProperties
    {
        RefPtr<Shader> VertexShader;
        RefPtr<Shader> FragmentShader;
        RefPtr<Shader> ComputeShader;

        RefPtr<Buffer> VertexBuffer;
        RefPtr<Buffer> IndexBuffer;
        std::unordered_map<RefPtr<Buffer>, uint32_t> UniformBuffers;
    };

    class PipelineState : public NonCopyable
    {
    public:
        PipelineState(const PipelineStateProperties& properties)
        : m_properties(properties)
        {}

        virtual ~PipelineState() = default;

        PipelineStateProperties m_properties;
    };
}