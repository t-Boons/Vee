#include "core/device.hpp"
#include "platform/vulkan/vulkan_device.hpp"

namespace vee
{
    RenderAPI g_currentRenderAPI = RenderAPI::None;

    RenderAPI vee::CurrentRenderAPI()
    {
        return g_currentRenderAPI;
    }

    void vee::InitDevice(RenderAPI api)
    {
        switch (api)
        {
        case RenderAPI::Vulkan:
			g_currentRenderAPI = RenderAPI::Vulkan;
            g_vkDevice = new VulkanDevice(true);
            break;
        }
    }
}