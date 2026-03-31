#include "platform/vulkan/vulkan_fence.hpp"
#include "platform/vulkan/vulkan_device.hpp"

namespace vee
{
    VulkanFence::VulkanFence(bool signaled)
    {
        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        if (signaled)
            fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        VKValidate(vkCreateFence(VKDevice()->GetLogicalDevice()->GetVKDevice(), &fenceInfo, nullptr, &m_fence));
    }

    VulkanFence::~VulkanFence()
    {
        vkDestroyFence(VKDevice()->GetLogicalDevice()->GetVKDevice(), m_fence, nullptr);
    }

    void VulkanFence::Wait(uint64_t timeout)
    {
        VKValidate(vkWaitForFences(VKDevice()->GetLogicalDevice()->GetVKDevice(), 1, &m_fence, VK_TRUE, timeout));
    }

    void VulkanFence::Reset()
    {
        VKValidate(vkResetFences(VKDevice()->GetLogicalDevice()->GetVKDevice(), 1, &m_fence));
    }
}