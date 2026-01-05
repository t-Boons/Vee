#include "Vee.hpp"
#include "platform/windows/windows_window.hpp"
#include "platform/vulkan/vulkan_device.hpp"
#include "platform/vulkan/vulkan_shader.hpp"

using namespace std;

int main()
{
	vee::Log::Info("Starting Vee Engine...");

	vee::InitDevice(vee::RenderAPI::Vulkan);
	vee::IShader* vertexShader = new vee::VulkanShader(vee::ShaderType::Vertex, "../../../assets/shaders/simple.vert.spv");
	vee::IShader* fragmentShader = new vee::VulkanShader(vee::ShaderType::Fragment, "../../../assets/shaders/simple.frag.spv");
	vee::WindowProperties props;
	props.Name = "Vee Engine";
	props.Width = 1280;
	props.Height = 720;
	vee::IWindow* window = new vee::WindowsWindow(props);
	
	while (!window->ShouldClose())
	{
		window->Update();
	}
}
