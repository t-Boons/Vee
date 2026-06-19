#include "platform/vulkan/vulkan_surface.hpp"

#include "platform/vulkan/vulkan_device.hpp"
#include "platform/windows/windows_window.hpp"

namespace vee
{
    VulkanSurface::VulkanSurface(VulkanDevice& device, Window &window)
        : m_device(device)
    {
        WindowsWindow &win = static_cast<WindowsWindow &>(window);
        VKValidate(glfwCreateWindowSurface(device.GetInstance()->GetVkInstance(), win.GLFWWindow(), nullptr, &m_surface));
    }

    VulkanSurface::~VulkanSurface()
    {
        vkDestroySurfaceKHR(m_device.GetInstance()->GetVkInstance(), m_surface, nullptr);
    }
}