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
        VkFramebuffer& GetSwapchainFrameBufferFromIndex(uint32_t index) {return m_swapChainFrameBuffers[index];}
        ~VulkanSwapchain();

    private:
        VkRenderPass m_swapchainRenderPass;
        VkSwapchainKHR m_swapchain;
        std::array<VkImage, 2> m_swapChainImages;
        std::array<VkImageView, 2> m_swapChainImageViews;
        std::array<VkFramebuffer, 2> m_swapChainFrameBuffers;

        std::array<VkImage, 2> m_swapChainDepthImages;
        std::array<VkImageView, 2> m_swapChainDepthImageViews;
        std::array<VkFramebuffer, 2> m_swapChainDepthFrameBuffers;
		std::array<VmaAllocation, 2> m_swapChainDepthImageAllocations;
    };
}