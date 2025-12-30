#pragma once
#include "common.hpp"

namespace vee
{
    class IRenderContext : public NonCopyable
    {
    public:
        virtual void Initialize() = 0;
        virtual void Shutdown() = 0;
    };
}