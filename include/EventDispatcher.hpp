#pragma once
#include <functional>
#include <vector>

namespace Vee
{
	template <typename InType>
	class EventDispatcher
	{
		using Func = std::function<void(InType)>;
	public:

		void Broadcast(const InType& val)
		{
			for (auto& f : m_funcs)
			{
				f(val);
			}
		}

		void Subscribe(const Func& func)
		{
			m_funcs.push_back(func);
		}

		std::vector<Func> m_funcs;
	};
}