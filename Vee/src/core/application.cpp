#include "core/application.hpp"
#include "core/device.hpp"
#include "core/window.hpp"
#include "core/layer.hpp"

#include "platform/windows/windows_window.hpp"
#include "glfw/glfw3.h"

namespace vee
{
	void Application::Initialize()
	{
		vee::Log::Info("Starting Vee Engine...");

		WindowProperties windowProperties;
		windowProperties.Name = "VeeApp";
		windowProperties.Width = 1280;
		windowProperties.Height = 720;
		m_window = MakeRef<WindowsWindow>(windowProperties);

		m_device = Device::Create(*m_window.get());

		s_app = this;
	}

	void Application::Tick()
	{
		while (!m_window->ShouldClose())
		{
			float deltaTime = m_newFrameTime - m_lastFrameTime;
			m_lastFrameTime = m_newFrameTime;
			m_newFrameTime = (float)glfwGetTime();

			for (auto& layer : m_layers)
			{
				layer->OnTick(deltaTime);
			}
			m_window->Update();
		}
	}

	void Application::ShutDown()
	{
		for (auto& layer : m_layers)
		{
			layer->OnShutdown();
		}
	}

	void Application::AddLayer(const RefPtr<Layer>& layer)
	{
		m_layers.push_back(layer);
		layer->OnInit();
	}
}