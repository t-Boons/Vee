#pragma once

namespace vee
{
	class Layer
	{
	public:
		virtual void OnInit() = 0;
		virtual void OnTick(float deltaTime) = 0;
		virtual void OnShutdown() = 0;
	};
}