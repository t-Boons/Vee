#include "platform/vulkan/vulkan_device.hpp"
#include "platform/vulkan/vulkan_common.hpp"

#include "platform/vulkan/vulkan_device_instance.hpp"

namespace vee
{
    VulkanDevice* g_vkDevice = nullptr;

    VulkanDevice *vee::VKDevice()
    {
        CheckMsg(g_vkDevice, "Vulkan Device not initialized.");
        return g_vkDevice;
    }

    VulkanDevice::VulkanDevice(bool enableValidationLayers, Window& window)
        : m_enableValidationLayers(enableValidationLayers)
    {
        m_instance = new VulkanInstance(m_enableValidationLayers);
        m_physicalDevice = new VulkanPhysicalDevice(m_instance);
        m_device = new VulkanLogicalDevice(m_physicalDevice);

        g_vkDevice = this;

        VmaAllocatorCreateInfo allocatorInfo = {};
        allocatorInfo.instance = m_instance->GetVkInstance();
        allocatorInfo.physicalDevice = m_physicalDevice->GetVKPhysicalDevice();
        allocatorInfo.device = m_device->GetVKDevice();
        allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_3;
        vmaCreateAllocator(&allocatorInfo, &m_allocator);

        m_surface = new VulkanSurface(*this, window);
        m_swapchain = new VulkanSwapchain(*this, *m_surface, window);

    }

	void VulkanDevice::DebugNameResource(VkObjectType type, uint64_t handle, const std::string& name)
	{
		if (m_enableValidationLayers)
		{
			VkDebugUtilsObjectNameInfoEXT nameInfo{};
			nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
			nameInfo.objectType = type;
			nameInfo.objectHandle = handle;
			nameInfo.pObjectName = name.c_str();
			auto func = (PFN_vkSetDebugUtilsObjectNameEXT)vkGetDeviceProcAddr(m_device->GetVKDevice(), "vkSetDebugUtilsObjectNameEXT");

			if (func != nullptr)
			{
				func(m_device->GetVKDevice(), &nameInfo);
			}
		}
	}

    VulkanDevice::~VulkanDevice()
    {
        FREE(m_device);
        FREE(m_physicalDevice);
        FREE(m_instance);
    }
}