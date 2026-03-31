#include "platform/vulkan/vulkan_semaphore.hpp"
#include "platform/vulkan/vulkan_device.hpp"

namespace vee
{
    VulkanSemaphore::VulkanSemaphore()
    {
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VKValidate(vkCreateSemaphore(VKDevice()->GetLogicalDevice()->GetVKDevice(), &semaphoreInfo, nullptr, &m_semaphore));
    }

    VulkanSemaphore::~VulkanSemaphore()
    {
        vkDestroySemaphore(VKDevice()->GetLogicalDevice()->GetVKDevice(), m_semaphore, nullptr);
    }
}