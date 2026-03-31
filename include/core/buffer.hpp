#pragma once

#include "common.hpp"

namespace vee
{
    enum class BufferUsage
    {
        Vertex,
        Index,
        Uniform,
        TransferSrc,
        TransferDst
    };

    enum class MemoryType
    {
        Static,
        Dynamic
    };



    struct BufferProperties
    {
        uint32_t Size;
        void* Data = nullptr;
        BufferUsage Usage;
        MemoryType MemoryType;
    };

    class Buffer
    {
    public:
        Buffer(const BufferProperties& properties) : m_properties(properties) {}
        virtual ~Buffer() = default;

        virtual void* Map() = 0;
        virtual void UnMap() = 0;
        
    protected:
        BufferProperties m_properties;
    };
}
