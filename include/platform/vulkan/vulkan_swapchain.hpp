#pragma once
#include "vulkan_common.hpp"

namespace vee
{
    class VulkanSurface;
    class Window;

    class VulkanSwapchain 
    {
    public:
        VulkanSwapchain(VulkanSurface& surface, Window& window);
        VkSwapchainKHR& GetVKSwapchain() { return m_swapchain; }
        ~VulkanSwapchain();

    private:
        VkSwapchainKHR m_swapchain;
        std::array<VkImage, 2> m_swapChainImages;
        std::array<VkImageView, 2> m_swapChainImageViews;
    };
}