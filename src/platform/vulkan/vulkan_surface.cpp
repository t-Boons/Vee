#include "platform/vulkan/vulkan_surface.hpp"

#include "platform/vulkan/vulkan_device.hpp"
#include "platform/windows/windows_window.hpp"

namespace vee
{
    VulkanSurface::VulkanSurface(Window &window)
    {
        WindowsWindow &win = static_cast<WindowsWindow &>(window);
        VKValidate(glfwCreateWindowSurface(VKDevice()->GetInstance()->GetVkInstance(), win.GLFWWindow(), nullptr, &m_surface));
    }

    VulkanSurface::~VulkanSurface()
    {
        vkDestroySurfaceKHR(VKDevice()->GetInstance()->GetVkInstance(), m_surface, nullptr);
    }
}