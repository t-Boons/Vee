#pragma once

#include "common.hpp"
#include "device_memory.hpp"

namespace vee
{
    struct BufferProperties
    {
        uint32_t BytesPerElement; // stride for each logical element
        uint32_t NumElements;     // number of elements
    };

    class Buffer
    {
    public:
        Buffer(const BufferProperties& properties) : m_properties(properties) {}

        

        virtual ~Buffer() = default;
    protected:
        BufferProperties m_properties;
        RefPtr<DeviceMemory> m_memory;
    };
}
