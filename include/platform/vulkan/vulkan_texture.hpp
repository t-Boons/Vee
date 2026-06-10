#pragma once

#include "core/texture.hpp"
#include "vulkan_common.hpp"

namespace vee
{
    class VulkanTexture : public Texture
    {
    public:
        VulkanTexture(const TextureProperties& properties);
        ~VulkanTexture();

        VkImageView& GetImageView() { return m_imageView; }
        VkImage& GetImage() { return m_image; }
        VmaAllocation& GetAllocation() { return m_allocation; }

    private:
        VkFormat m_format;
        VkImage m_image;
        VkImageView m_imageView;
        VmaAllocation m_allocation;
        TextureProperties m_properties;
    };

    class VulkanTextureCube : public TextureCube
    {
    public:
        VulkanTextureCube(const TextureCubeProperties& properties);
        ~VulkanTextureCube();

        VkImageView& GetImageView() { return m_imageView; }
        VkImage& GetImage() { return m_image; }
        VmaAllocation& GetAllocation() { return m_allocation; }

    private:
        VkFormat m_format;
        VkImage m_image;
        VkImageView m_imageView;
        VmaAllocation m_allocation;
        TextureCubeProperties m_properties;
    };
}