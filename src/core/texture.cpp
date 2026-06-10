#include "core/texture.hpp"

#include "platform/vulkan/vulkan_texture.hpp"

namespace vee
{
	RefPtr<Texture> Texture::Create(const TextureProperties& properties)
	{
		switch (CurrentRenderAPI())
		{
		case RenderAPI::Vulkan:
			return MakeRef<VulkanTexture>(properties);
		default:
			CheckMsg(false, "Unsupported render API.");
			return nullptr;
		}
	}

	RefPtr<TextureCube> TextureCube::Create(const TextureCubeProperties& properties)
	{
		switch (CurrentRenderAPI())
		{
		case RenderAPI::Vulkan:
			return MakeRef<VulkanTextureCube>(properties);
		default:
			CheckMsg(false, "Unsupported render API.");
			return nullptr;
		}
	}
}