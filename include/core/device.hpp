#pragma once
#include "common.hpp"

namespace vee
{
    enum class RenderAPI
    {
        None,   
        Vulkan,
    };

    void InitDevice(RenderAPI api);
	RenderAPI CurrentRenderAPI();

    class Device : public NonCopyable
    {
    public:
        virtual ~Device() = default;
    };
}