#pragma once
#include "common.hpp"

namespace vee
{
    enum class RenderAPI
    {
        Vulkan,
    };

    void InitDevice(RenderAPI api);

    class Device : public NonCopyable
    {
    public:
        virtual ~Device() = default;
    };
}