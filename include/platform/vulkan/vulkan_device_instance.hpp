#pragma once
#include "vulkan/vulkan.hpp"

namespace vee
{
    class VulkanInstance
    {
    public:
        VulkanInstance(bool enableValidationLayers);
        ~VulkanInstance();

        const VkInstance& GetInstance() const { return m_instance; }

    private:
        VkInstance m_instance;
    };
}