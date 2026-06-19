#include "platform/vulkan/vulkan_sampler.hpp"
#include "platform/vulkan/vulkan_device.hpp"

namespace vee
{
	VulkanSampler::VulkanSampler()
	{
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_FALSE;
		samplerInfo.maxAnisotropy = 16;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		VKValidate(vkCreateSampler(VKDevice()->GetLogicalDevice()->GetVKDevice(), &samplerInfo, nullptr, &m_sampler));
	}

	VulkanSampler::~VulkanSampler()
	{
		vkDestroySampler(VKDevice()->GetLogicalDevice()->GetVKDevice(), m_sampler, nullptr);
	}
}