#pragma once
#include "vulkan_common.hpp"

namespace vee
{
    class VulkanInstance
    {
    public:
        VulkanInstance(bool enableValidationLayers);
        ~VulkanInstance();

        VkInstance& GetVkInstance() { return m_instance; }

    private:
        VkInstance m_instance;
    };
}