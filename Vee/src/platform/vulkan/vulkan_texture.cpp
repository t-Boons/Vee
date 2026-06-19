#include "platform/vulkan/vulkan_texture.hpp"
#include "platform/vulkan/vulkan_device.hpp"
#include "platform/vulkan/vulkan_commandlist.hpp"
#include "platform/vulkan/vulkan_buffer.hpp"
#include "platform/vulkan/vulkan_fence.hpp"
#include "tinygltf/stb_image.h"

namespace vee
{
    VulkanTexture::VulkanTexture(const TextureProperties& properties)
        : m_properties(properties)
    {
		CheckMsg(properties.Data != nullptr, "Texture data cannot be null");

        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = properties.Width;
        imageInfo.extent.height = properties.Height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
        allocInfo.preferredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

        VKValidate(vmaCreateImage(VKDevice()->GetAllocator(), &imageInfo, &allocInfo, &m_image, &m_allocation, nullptr));
        VKDevice()->DebugNameResource(VK_OBJECT_TYPE_IMAGE, (uint64_t)m_image, "Texture_" + properties.DebugName);

        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = m_image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = imageInfo.format;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        VKValidate(vkCreateImageView(VKDevice()->GetLogicalDevice()->GetVKDevice(), &viewInfo, nullptr, &m_imageView));
		VKDevice()->DebugNameResource(VK_OBJECT_TYPE_IMAGE_VIEW, (uint64_t)m_imageView, "TextureImageView_" + properties.DebugName);

        BufferProperties stagingBufferProperties{};
        stagingBufferProperties.Size = properties.Width * properties.Height * properties.NumChannels;
        stagingBufferProperties.Usage = BufferUsage::TransferSrc;
        stagingBufferProperties.MemoryType = MemoryType::Dynamic;
        stagingBufferProperties.Data = properties.Data;
		stagingBufferProperties.DebugName = "Staging_" + properties.DebugName;
        VulkanBuffer* stagingBuffer = new VulkanBuffer(stagingBufferProperties);

        VulkanCommandList commandList({ QueueType::Graphics, "Staging_" + properties.DebugName});
        commandList.Begin();

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = m_image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        vkCmdPipelineBarrier(commandList.GetVKCommandBuffer(), VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = {0, 0, 0};
        region.imageExtent = {(uint32_t)properties.Width, (uint32_t)properties.Height, 1};
        vkCmdCopyBufferToImage(commandList.GetVKCommandBuffer(), stagingBuffer->GetVKBuffer(), m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        vkCmdPipelineBarrier(commandList.GetVKCommandBuffer(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

        commandList.End();
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandList.GetVKCommandBuffer();

        VulkanFence fence;
        VKValidate(vkQueueSubmit(VKDevice()->GetLogicalDevice()->GetQueue(QueueType::Graphics), 1, &submitInfo, fence.GetVKFence()));
        fence.Wait();
        fence.Reset();
        commandList.Reset();
        delete stagingBuffer;
    }

    VulkanTexture::~VulkanTexture()
    {
        vkDestroyImageView(VKDevice()->GetLogicalDevice()->GetVKDevice(), m_imageView, nullptr);
        vmaDestroyImage(VKDevice()->GetAllocator(), m_image, m_allocation);
    }




    VulkanTextureCube::VulkanTextureCube(const TextureCubeProperties& properties)
        : m_properties(properties)
    {
		for (uint32_t i = 0; i < CUBEMAP_FACE_COUNT; i++)
        {
            CheckMsg(properties.Data[i] != nullptr, "Texture data cannot be null");
        }

        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = properties.Width;
        imageInfo.extent.height = properties.Height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = CUBEMAP_FACE_COUNT;
        imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
        allocInfo.preferredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

        VKValidate(vmaCreateImage(VKDevice()->GetAllocator(), &imageInfo, &allocInfo, &m_image, &m_allocation, nullptr));

        VKDevice()->DebugNameResource(VK_OBJECT_TYPE_IMAGE, (uint64_t)m_image, "TextureCube_" + properties.DebugName);

        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = m_image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
        viewInfo.format = imageInfo.format;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = CUBEMAP_FACE_COUNT;

        VKValidate(vkCreateImageView(VKDevice()->GetLogicalDevice()->GetVKDevice(), &viewInfo, nullptr, &m_imageView));
        VKDevice()->DebugNameResource(VK_OBJECT_TYPE_IMAGE_VIEW, (uint64_t)m_imageView, "TextureCubeImageView_" + properties.DebugName);

		const uint32_t faceSize = properties.Width * properties.Height * properties.NumChannels;
        const uint32_t totalBytes = faceSize * CUBEMAP_FACE_COUNT;

        BufferProperties stagingBufferProperties{};
        stagingBufferProperties.Size = totalBytes;
        stagingBufferProperties.Usage = BufferUsage::TransferSrc;
        stagingBufferProperties.MemoryType = MemoryType::Dynamic;
        stagingBufferProperties.Data = nullptr;
		stagingBufferProperties.DebugName = "Staging_" + properties.DebugName;
        VulkanBuffer* stagingBuffer = new VulkanBuffer(stagingBufferProperties);

        void* mapped = stagingBuffer->Map();
        for (uint32_t i = 0; i < CUBEMAP_FACE_COUNT; i++)
        {
            memcpy((uint8_t*)mapped + i * faceSize, properties.Data[i], faceSize);
        }
        stagingBuffer->UnMap();

        VulkanCommandList commandList({ QueueType::Graphics, "Staging_" + properties.DebugName });
        commandList.Begin();

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = m_image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = CUBEMAP_FACE_COUNT;
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        vkCmdPipelineBarrier(commandList.GetVKCommandBuffer(), VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = CUBEMAP_FACE_COUNT;
        region.imageOffset = { 0, 0, 0 };
        region.imageExtent = { (uint32_t)properties.Width, (uint32_t)properties.Height, 1 };
        vkCmdCopyBufferToImage(commandList.GetVKCommandBuffer(), stagingBuffer->GetVKBuffer(), m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        vkCmdPipelineBarrier(commandList.GetVKCommandBuffer(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

        commandList.End();
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandList.GetVKCommandBuffer();

        VulkanFence fence;
        VKValidate(vkQueueSubmit(VKDevice()->GetLogicalDevice()->GetQueue(QueueType::Graphics), 1, &submitInfo, fence.GetVKFence()));
        fence.Wait();
        fence.Reset();
        commandList.Reset();
        delete stagingBuffer;
    }

	VulkanTextureCube::~VulkanTextureCube()
	{
        vkDestroyImageView(VKDevice()->GetLogicalDevice()->GetVKDevice(), m_imageView, nullptr);
        vmaDestroyImage(VKDevice()->GetAllocator(), m_image, m_allocation);
	}
}