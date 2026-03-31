#include "platform/vulkan/vulkan_shader_binding.hpp"
#include "platform/vulkan/vulkan_device.hpp"

namespace vee
{
    VulkanShaderBinding::VulkanShaderBinding()
    {
    }

    VulkanShaderBinding::~VulkanShaderBinding()
    {
        vkDestroyDescriptorSetLayout(VKDevice()->GetLogicalDevice()->GetVKDevice(), m_descriptorSetLayout, nullptr);
    }

    void VulkanShaderBinding::AddBinding(uint32_t binding, VkDescriptorType type)
    {
        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding = binding;
        layoutBinding.descriptorType = type;
        layoutBinding.descriptorCount = 1;
        layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	    layoutBinding.pImmutableSamplers = nullptr;
        
        m_bindings.push_back(layoutBinding);
    }

    void VulkanShaderBinding::CompileLayout()
    {
        CheckMsg(!m_bindings.empty(), "No bindings added to shader binding");

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(m_bindings.size());
        layoutInfo.pBindings = m_bindings.data();

        VKValidate(vkCreateDescriptorSetLayout(VKDevice()->GetLogicalDevice()->GetVKDevice(), &layoutInfo, nullptr, &m_descriptorSetLayout));
    }
}