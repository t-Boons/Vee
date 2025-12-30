#pragma once
#include "common.hpp"
#include "render_context.hpp"

namespace vee
{
    struct WindowProperties
    {
        std::string Name = "Vee Window";
        uint32_t Width = 1280;
        uint32_t Height = 720;
        RefPtr<IRenderContext> RenderContext = nullptr;
    };

    class IWindow : public NonCopyable
    {
    public:
        virtual void Initialize(WindowProperties properties) = 0;
        virtual void Update() = 0;
        virtual bool ShouldClose() const = 0;
        virtual uint32_t Width() const = 0;
        virtual uint32_t Height() const = 0;
        virtual std::string Name() const = 0;
        virtual ~IWindow() = default;
    };
}
