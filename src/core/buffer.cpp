#include "core/buffer.hpp"

#include "platform/vulkan/vulkan_buffer.hpp"

namespace vee
{
	RefPtr<Buffer> Buffer::Create(const BufferProperties& properties)
	{
		switch (CurrentRenderAPI())
		{
		case RenderAPI::Vulkan:
			return MakeRef<VulkanBuffer>(properties);
		default:
			CheckMsg(false, "Unsupported render API.");
			return nullptr;
		}
	}
}