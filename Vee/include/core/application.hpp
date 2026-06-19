#pragma once
#include "core/common.hpp"

namespace vee
{
	class Device;
	class Window;
	class Layer;

	class Application : public NonCopyable
	{
	public:
		virtual void Initialize();
		virtual void Tick();
		virtual void ShutDown();

		void AddLayer(const RefPtr<Layer>& layer);

		RefPtr<Device> GetDevice() { return m_device; }
		RefPtr<Window> GetWindow() { return m_window; }

		static Application* Get() { return s_app; }

	private:
		RefPtr<Device> m_device;
		RefPtr<Window> m_window;
		std::vector<RefPtr<Layer>> m_layers;

		float m_newFrameTime = 0.0f;
		float m_lastFrameTime = 0.0f;
		float m_time = 0.0f;

		inline static Application* s_app;
	};
}