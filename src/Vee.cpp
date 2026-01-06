#include "Vee.hpp"
#include "platform/windows/windows_window.hpp"
#include "platform/vulkan/vulkan_device.hpp"
#include "platform/vulkan/vulkan_shader.hpp"
#include "platform/vulkan/vulkan_surface.hpp"
#include "platform/vulkan/vulkan_swapchain.hpp"

using namespace std;

int main()
{
	vee::Log::Info("Starting Vee Engine...");

	vee::WindowProperties props;
	props.Name = "Vee Engine";
	props.Width = 1280;
	props.Height = 720;
	vee::Window* window = new vee::WindowsWindow(props);

	vee::InitDevice(vee::RenderAPI::Vulkan);
	vee::Shader* vertexShader = new vee::VulkanShader(vee::ShaderType::Vertex, "../../../assets/shaders/simple.vert.spv");
	vee::Shader* fragmentShader = new vee::VulkanShader(vee::ShaderType::Fragment, "../../../assets/shaders/simple.frag.spv");

	vee::VulkanSurface* surface = new vee::VulkanSurface(*window);
	vee::VulkanSwapchain* swapchain = new vee::VulkanSwapchain(*surface, *window);
	
    vee::Log::Info("Vulkan selected GPU: %s", vee::VKDevice()->GetDeviceName().c_str());

	while (!window->ShouldClose())
	{
		window->Update();
	}
}
