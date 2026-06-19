#pragma once

#include "vulkan_common.hpp"

namespace vee
{
    class VulkanSemaphore
    {
    public:
        VulkanSemaphore();
        ~VulkanSemaphore();

        VkSemaphore& GetVKSempahore() { return m_semaphore; }

    private:
        VkSemaphore m_semaphore;
    };
}