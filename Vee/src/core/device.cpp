#include "core/device.hpp"
#include "platform/vulkan/vulkan_device.hpp"
#include "core/window.hpp"

namespace vee
{
    RefPtr<Device> Device::Create(Window& window)
    {
        return MakeRef<VulkanDevice>(true, window);
    }
}