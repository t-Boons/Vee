#include "vulkan/vulkan.hpp"
#include "platform/windows/windows_window.hpp"

namespace vee
{
    bool WindowsWindow::ShouldClose() const
	{
		return glfwWindowShouldClose(m_window);
	}

	void WindowsWindow::Initialize(WindowProperties properties)
	{
		m_properties = properties;
		InitGLFW();
		BindGLFWCallbacks();
		BindEvents();


		const char* extensions[6] = {
			VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
			VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
			VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
			VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME, 
			VK_KHR_SPIRV_1_4_EXTENSION_NAME,             
			VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME  
		};
    }


	void WindowsWindow::Update()
	{
		m_oldTime = m_time;
		m_time = static_cast<float>(glfwGetTime());
		m_deltaTime = m_time - m_oldTime;
		glfwPollEvents();
	}

	void WindowsWindow::InitGLFW()
	{
		if (!glfwInit())
		{
			Throw("Failed to initialize GLTF.");
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		m_window = glfwCreateWindow(m_properties.Width, m_properties.Height, m_properties.Name.c_str(), nullptr, nullptr);
		if (!m_window)
		{
			glfwTerminate();
			Throw("Failed to initialize GLTF WindowsWindow.");
		}

		glfwShowWindow(m_window);
		glfwMakeContextCurrent(m_window);
		m_hwnd = glfwGetWin32Window(m_window);
	}

	void WindowsWindow::BindGLFWCallbacks()
	{
		glfwSetWindowUserPointer(m_window, &m_events);

		glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height)
			{
				WindowResizeEvent event;
				event.Width = static_cast<uint32_t>(width);
				event.Height = static_cast<uint32_t>(height);

				WindowEvents* data = static_cast<WindowEvents*>(glfwGetWindowUserPointer(window));
				data->OnWindowResize.Broadcast(event);
			});

		glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int, int action, int)
			{
				if (action != 2)
				{
					KeyEvent event;
					event.Key = key;
					event.Pressed = action;

					WindowEvents* data = static_cast<WindowEvents*>(glfwGetWindowUserPointer(window));
					data->OnKeyEvent.Broadcast(event);
				}
			});

		glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double deltaX, double deltaY)
			{
				MousePointerEvent event;
				event.X = static_cast<int>(deltaX);
				event.Y = static_cast<int>(deltaY);

				WindowEvents* data = static_cast<WindowEvents*>(glfwGetWindowUserPointer(window));
				data->OnPointerEvent.Broadcast(event);
			});

		glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int)
			{
				MouseButtonEvent event;
				event.Button = button;
				event.Pressed = action;

				WindowEvents* data = static_cast<WindowEvents*>(glfwGetWindowUserPointer(window));
				data->OnMouseEvent.Broadcast(event);
			});

		glfwSetScrollCallback(m_window, [](GLFWwindow* window, double, double deltaY)
			{
				MouseScrollEvent event;
				event.Delta = static_cast<int>(deltaY);

				WindowEvents* data = static_cast<WindowEvents*>(glfwGetWindowUserPointer(window));
				data->OnScrollEvent.Broadcast(event);
			});

	}

	void WindowsWindow::BindEvents()
	{
		m_events.OnWindowResize.Subscribe([&](const WindowResizeEvent& ev)
			{
				m_properties.Width = ev.Width;
				m_properties.Height = ev.Height;
			});
	}
}