#include "core/buffer.hpp"
#include "core/device.hpp"

#include "platform/vulkan/vulkan_buffer.hpp"

namespace vee
{
	RefPtr<Buffer> Buffer::Create(const BufferProperties& properties)
	{
		return MakeRef<VulkanBuffer>(properties);
	}
}