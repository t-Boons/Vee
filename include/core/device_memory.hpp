#pragma once

#include "common.hpp"

namespace vee
{
    enum class DeviceMemoryType
    {
        GPU_ONLY,      // Fast GPU memory, no CPU access
        CPU_TO_GPU,    // CPU writes, GPU reads (upload/staging)
        GPU_TO_CPU,    // GPU writes, CPU reads (readback)
        CPU_ONLY       // Rare, mostly for tools/debug
    };

    struct DeviceMemoryProperties
    {
        DeviceMemoryType Type;
        size_t Size;
        uint32_t alignment;
    };

    class DeviceMemory
    {
    public:
        DeviceMemory(const DeviceMemoryProperties& properties) : m_properties(properties) {}

        virtual void* Map() = 0;
        virtual void UnMap() = 0;

        virtual ~DeviceMemory() = default;

    private:
        DeviceMemoryProperties m_properties;
    };
}
