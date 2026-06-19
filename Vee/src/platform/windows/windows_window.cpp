#include "platform/windows/windows_window.hpp"

#define GLFW_INCLUDE_VULKAN
#include <glfw/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw/glfw3native.h>

namespace vee
{
	bool WindowsWindow::ShouldClose() const
	{
		return glfwWindowShouldClose(m_window);
	}

	WindowsWindow::WindowsWindow(WindowProperties properties)
		: m_properties(properties), m_window(nullptr), m_oldTime(0.0f), m_deltaTime(0.0f), m_time(0.0f)
	{
		m_properties = properties;
		InitGLFW();
		BindGLFWCallbacks();
		BindEvents();
	}

	void WindowsWindow::Update()
	{
		m_oldTime = m_time;
		m_time = static_cast<float>(glfwGetTime());
		m_deltaTime = m_time - m_oldTime;
		glfwPollEvents();
	}

	WindowsWindow::~WindowsWindow()
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
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
	}

	void WindowsWindow::BindGLFWCallbacks()
	{
		glfwSetWindowUserPointer(m_window, &m_events);

		glfwSetWindowSizeCallback(m_window, [](GLFWwindow *window, int width, int height)
								  {
				WindowResizeEvent event;
				event.Width = static_cast<uint32_t>(width);
				event.Height = static_cast<uint32_t>(height);

				WindowEvents* data = static_cast<WindowEvents*>(glfwGetWindowUserPointer(window));
				data->OnWindowResize.Broadcast(event); });

		glfwSetKeyCallback(m_window, [](GLFWwindow *window, int key, int, int action, int)
						   {
				if (action != 2)
				{
					KeyEvent event;
					event.Key = key;
					event.Pressed = action;

					WindowEvents* data = static_cast<WindowEvents*>(glfwGetWindowUserPointer(window));
					data->OnKeyEvent.Broadcast(event);
				} });

		glfwSetCursorPosCallback(m_window, [](GLFWwindow *window, double deltaX, double deltaY)
								 {
				MousePointerEvent event;
				event.X = static_cast<int>(deltaX);
				event.Y = static_cast<int>(deltaY);

				WindowEvents* data = static_cast<WindowEvents*>(glfwGetWindowUserPointer(window));
				data->OnPointerEvent.Broadcast(event); });

		glfwSetMouseButtonCallback(m_window, [](GLFWwindow *window, int button, int action, int)
								   {
				MouseButtonEvent event;
				event.Button = button;
				event.Pressed = action;

				WindowEvents* data = static_cast<WindowEvents*>(glfwGetWindowUserPointer(window));
				data->OnMouseEvent.Broadcast(event); });

		glfwSetScrollCallback(m_window, [](GLFWwindow *window, double, double deltaY)
							  {
				MouseScrollEvent event;
				event.Delta = static_cast<int>(deltaY);

				WindowEvents* data = static_cast<WindowEvents*>(glfwGetWindowUserPointer(window));
				data->OnScrollEvent.Broadcast(event); });
	}

	void WindowsWindow::BindEvents()
	{
		m_events.OnWindowResize.Subscribe([&](const WindowResizeEvent &ev)
										  {
				m_properties.Width = ev.Width;
				m_properties.Height = ev.Height; });
	}
}