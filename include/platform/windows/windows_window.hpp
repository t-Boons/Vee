#pragma once
#include "core/common.hpp"
#include "core/event_dispatcher.hpp"
#include "core/window.hpp"

struct GLFWwindow;

namespace vee
{
	struct WindowResizeEvent
	{
		uint32_t Width;
		uint32_t Height;
	};

	struct KeyEvent
	{
		int Key;
		bool Pressed;
	};

	struct MouseButtonEvent
	{
		int Button;
		bool Pressed;
	};

	struct MousePointerEvent
	{
		int X;
		int Y;
	};

	struct MouseScrollEvent
	{
		int Delta;
	};

	class WindowsWindow : public Window
	{
	public:
		struct WindowEvents
		{
			EventDispatcher<WindowResizeEvent> OnWindowResize;
			EventDispatcher<KeyEvent> OnKeyEvent;
			EventDispatcher<MousePointerEvent> OnPointerEvent;
			EventDispatcher<MouseButtonEvent> OnMouseEvent;
			EventDispatcher<MouseScrollEvent> OnScrollEvent;
		};

			WindowsWindow(WindowProperties properties);

			virtual bool ShouldClose() const override;
			virtual void Update() override;

			virtual uint32_t Width() const override { return m_properties.Width; }
			virtual uint32_t Height() const override { return m_properties.Height; }
			virtual std::string Name() const override { return m_properties.Name; }
			GLFWwindow* GLFWWindow() const { return m_window; }

			float Time() const { return m_time; }
			float DeltaTime() const { return m_deltaTime; }

			float AspectRatio() const { return static_cast<float>(m_properties.Width) / static_cast<float>(m_properties.Height); }

			WindowEvents &Events() { return m_events; }

			~WindowsWindow();

		private:
			void InitGLFW();
			void BindGLFWCallbacks();
			void BindEvents();

			float m_oldTime;
			float m_deltaTime;
			float m_time;

			WindowProperties m_properties;
			GLFWwindow *m_window;
			std::string m_name;
			WindowEvents m_events;
		};
}