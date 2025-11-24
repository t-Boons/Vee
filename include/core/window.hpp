#pragma once
#include "common.hpp"

namespace vee
{
    struct WindowProperties
    {
        std::string Name;
        uint32_t Width;
        uint32_t Height;
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
