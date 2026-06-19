#pragma once
#include "common.hpp"

namespace vee
{
    enum class RenderAPI
    {
        None,
        Vulkan,
    };


    class Window;

    class Device : public NonCopyable
    {
    public:

        static RefPtr<Device> Create(Window& window);

        virtual RenderAPI CurrentRenderAPI() = 0;
        virtual ~Device() = default;
    };
}