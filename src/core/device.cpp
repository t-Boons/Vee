#include "core/device.hpp"
#include "platform/vulkan/vulkan_device.hpp"

void vee::InitDevice(RenderAPI api)
{
    switch(api)
    {
    case RenderAPI::Vulkan:
        g_vkDevice = new VulkanDevice(false);
        break;
    }
}