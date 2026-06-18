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
        VkImageView& GetSwapChainImageView(uint32_t index) {return m_swapChainImageViews[index];}
        VkImage& GetSwapChainImage(uint32_t index) { return m_swapChainImages[index]; }
        VkImageView& GetDepthImageView(uint32_t index) { return m_swapChainDepthImageViews[index]; }
        VkImage& GetDepthImage(uint32_t index) { return m_swapChainDepthImages[index]; }
        ~VulkanSwapchain();

    private:
        VkSwapchainKHR m_swapchain;
        std::array<VkImage, 2> m_swapChainImages;
        std::array<VkImageView, 2> m_swapChainImageViews;

        std::array<VkImage, 2> m_swapChainDepthImages;
        std::array<VkImageView, 2> m_swapChainDepthImageViews;
		std::array<VmaAllocation, 2> m_swapChainDepthImageAllocations;
    };
}