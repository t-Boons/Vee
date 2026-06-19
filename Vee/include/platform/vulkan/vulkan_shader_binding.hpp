#pragma once

#include "vulkan_common.hpp"

namespace vee
{
    class VulkanShaderBinding
    {
    public:
        VulkanShaderBinding();
        ~VulkanShaderBinding();

        void AddBinding(uint32_t binding, VkDescriptorType type);
        void CompileLayout();
        VkDescriptorSetLayout& GetDescriptorSetLayout() { return m_descriptorSetLayout; }

    private:
        VkDescriptorSetLayout m_descriptorSetLayout;
        std::vector<VkDescriptorSetLayoutBinding> m_bindings;
    };
}