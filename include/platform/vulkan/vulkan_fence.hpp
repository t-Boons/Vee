#pragma once

#include "platform/vulkan/vulkan_common.hpp"

namespace vee
{
    class VulkanFence
    {
    public:
        VulkanFence(bool signaled = false);
        ~VulkanFence();

        void Wait(uint64_t timeout = UINT64_MAX);
        void Reset();
        VkFence& GetVKFence() { return m_fence; }

    private:
        VkFence m_fence;
    };
}