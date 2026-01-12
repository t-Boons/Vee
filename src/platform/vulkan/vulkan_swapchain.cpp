#include "platform/vulkan/vulkan_swapchain.hpp"

#include "platform/vulkan/vulkan_device.hpp"
#include "platform/vulkan/vulkan_surface.hpp"
#include "core/window.hpp"

namespace vee
{
    VulkanSwapchain::VulkanSwapchain(VulkanSurface &surface, Window &window)
    {
        VkSurfaceFormatKHR surfaceFormat = {};
        surfaceFormat.format = VK_FORMAT_B8G8R8A8_SRGB;
        surfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

        VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
        VkExtent2D extent = {};
        extent.width = window.Width();
        extent.height = window.Height();

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface.Surface();

        createInfo.minImageCount = 2;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;

        std::array<uint32_t, 2> queueFamilyIndices = {0, 1};
        createInfo.pQueueFamilyIndices = queueFamilyIndices.data();

        createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        VKValidate(vkCreateSwapchainKHR(VKDevice()->GetLogicalDevice()->GetVKDevice(), &createInfo, nullptr, &m_swapchain));

        uint32_t imageCount = 2;
        VKValidate(vkGetSwapchainImagesKHR(VKDevice()->GetLogicalDevice()->GetVKDevice(), m_swapchain, &imageCount, m_swapChainImages.data()));


        for(size_t i = 0; i < m_swapChainImageViews.size(); i++)
        {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = m_swapChainImages[i];

            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = VK_FORMAT_B8G8R8A8_SRGB;

            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            VKValidate(vkCreateImageView(VKDevice()->GetLogicalDevice()->GetVKDevice(), &createInfo, nullptr, &m_swapChainImageViews[i]));
        }

        // Create the swapchain renderpass.
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = VK_FORMAT_B8G8R8A8_SRGB;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;

        vkCreateRenderPass(VKDevice()->GetLogicalDevice()->GetVKDevice(), &renderPassInfo, nullptr, &m_swapchainRenderPass);

        // Create the framebuffers for rendering.
        for (size_t i = 0; i < m_swapChainImageViews.size(); i++)
        {
            VkImageView attachments[] =
                {
                    m_swapChainImageViews[i]};

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = m_swapchainRenderPass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = window.Width();
            framebufferInfo.height = window.Height();
            framebufferInfo.layers = 1;

            vkCreateFramebuffer(VKDevice()->GetLogicalDevice()->GetVKDevice(), &framebufferInfo, nullptr, &m_swapChainFrameBuffers[i]);
        }
    }

    VulkanSwapchain::~VulkanSwapchain()
    {
        for (uint32_t i = 0; i < 2; i++)
        {
            vkDestroyImageView(VKDevice()->GetLogicalDevice()->GetVKDevice(), m_swapChainImageViews[i], nullptr);
            vkDestroyFramebuffer(VKDevice()->GetLogicalDevice()->GetVKDevice(), m_swapChainFrameBuffers[i], nullptr);
        }

        vkDestroySwapchainKHR(VKDevice()->GetLogicalDevice()->GetVKDevice(), m_swapchain, nullptr);
    }
}