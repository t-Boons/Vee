#pragma once
#include "core/render_context.hpp"

namespace vee
{
    class VulkanRenderContext : public IRenderContext
    {  
    public:
        virtual void Initialize() override
        {
        }

        virtual void Shutdown() override
        {
        }
    };
}