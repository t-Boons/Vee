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



        // -------------------------------------------------------
    // Create color + depth image views
    // -------------------------------------------------------
        for (size_t i = 0; i < m_swapChainImageViews.size(); i++)
        {
            // --- Color image view (from swapchain, no allocation needed) ---
            VkImageViewCreateInfo colorViewInfo{};
            colorViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            colorViewInfo.image = m_swapChainImages[i];
            colorViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            colorViewInfo.format = VK_FORMAT_B8G8R8A8_SRGB;
            colorViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            colorViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            colorViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            colorViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            colorViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            colorViewInfo.subresourceRange.baseMipLevel = 0;
            colorViewInfo.subresourceRange.levelCount = 1;
            colorViewInfo.subresourceRange.baseArrayLayer = 0;
            colorViewInfo.subresourceRange.layerCount = 1;

            VKValidate(vkCreateImageView(VKDevice()->GetLogicalDevice()->GetVKDevice(), &colorViewInfo, nullptr, &m_swapChainImageViews[i]));

            // --- Depth image (must be allocated manually) ---
            VkImageCreateInfo depthImageInfo{};
            depthImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            depthImageInfo.imageType = VK_IMAGE_TYPE_2D;
            depthImageInfo.extent.width = window.Width();
            depthImageInfo.extent.height = window.Height();
            depthImageInfo.extent.depth = 1;
            depthImageInfo.mipLevels = 1;
            depthImageInfo.arrayLayers = 1;
            depthImageInfo.format = VK_FORMAT_D32_SFLOAT;
            depthImageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
            depthImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            depthImageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            depthImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
            depthImageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            VmaAllocationCreateInfo depthAllocInfo{};
            depthAllocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
            depthAllocInfo.preferredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

            VKValidate(vmaCreateImage(VKDevice()->GetAllocator(), &depthImageInfo, &depthAllocInfo, &m_swapChainDepthImages[i], &m_swapChainDepthImageAllocations[i], nullptr));

            VkImageViewCreateInfo depthViewInfo{};
            depthViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            depthViewInfo.image = m_swapChainDepthImages[i];
            depthViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            depthViewInfo.format = VK_FORMAT_D32_SFLOAT;
            depthViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            depthViewInfo.subresourceRange.baseMipLevel = 0;
            depthViewInfo.subresourceRange.levelCount = 1;
            depthViewInfo.subresourceRange.baseArrayLayer = 0;
            depthViewInfo.subresourceRange.layerCount = 1;

            VKValidate(vkCreateImageView(VKDevice()->GetLogicalDevice()->GetVKDevice(), &depthViewInfo, nullptr, &m_swapChainDepthImageViews[i]));
        }

        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = VK_FORMAT_B8G8R8A8_SRGB;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = VK_FORMAT_D32_SFLOAT;
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;

        VkAttachmentDescription attachments[] = { colorAttachment, depthAttachment };

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 2;
        renderPassInfo.pAttachments = attachments;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;

        VKValidate(vkCreateRenderPass(VKDevice()->GetLogicalDevice()->GetVKDevice(), &renderPassInfo, nullptr, &m_swapchainRenderPass));

        // Framebuffers
        for (size_t i = 0; i < m_swapChainImageViews.size(); i++)
        {
            VkImageView attachments[] = {
                m_swapChainImageViews[i],
                m_swapChainDepthImageViews[i]
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = m_swapchainRenderPass;
            framebufferInfo.attachmentCount = 2;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = window.Width();
            framebufferInfo.height = window.Height();
            framebufferInfo.layers = 1;

            VKValidate(vkCreateFramebuffer(VKDevice()->GetLogicalDevice()->GetVKDevice(), &framebufferInfo, nullptr, &m_swapChainFrameBuffers[i]));
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