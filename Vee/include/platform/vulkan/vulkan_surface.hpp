#pragma once
#include "vulkan_common.hpp"

namespace vee
{
    class VulkanDevice;
    class Window;

    class VulkanSurface 
    {
    public:
        VulkanSurface(VulkanDevice& device, Window& window);
        ~VulkanSurface();

        VkSurfaceKHR& Surface() { return m_surface; }
    private:
        VkSurfaceKHR m_surface;
        VulkanDevice& m_device;
    };
}