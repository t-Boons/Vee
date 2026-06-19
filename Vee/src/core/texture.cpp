#include "core/texture.hpp"

#include "platform/vulkan/vulkan_texture.hpp"

namespace vee
{
	RefPtr<Texture> Texture::Create(const TextureProperties& properties)
	{
		return MakeRef<VulkanTexture>(properties);
	}

	RefPtr<TextureCube> TextureCube::Create(const TextureCubeProperties& properties)
	{
		return MakeRef<VulkanTextureCube>(properties);
	}
}