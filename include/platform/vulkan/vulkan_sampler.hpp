#pragma once

#include "vulkan_common.hpp"

namespace vee
{
	class VulkanSampler
	{
	public:
		VulkanSampler();
		~VulkanSampler();
		VkSampler& GetVulkanSampler() { return m_sampler; }
	private:
		VkSampler m_sampler;
	};
}