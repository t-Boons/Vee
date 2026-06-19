#pragma once
#include "core/device.hpp"
#include "vulkan_common.hpp"
#include "platform/vulkan/vulkan_device_physical_device.hpp"
#include "platform/vulkan/vulkan_device_logical_device.hpp"
#include "platform/vulkan/vulkan_surface.hpp"
#include "platform/vulkan/vulkan_swapchain.hpp"

namespace vee
{
    class VulkanDevice : public Device
    {
    public:
        VulkanDevice(bool enableValidationLayers, Window& window);
        ~VulkanDevice() override;

        VulkanLogicalDevice* GetLogicalDevice() const { return m_device; }
        VulkanPhysicalDevice* GetPhysicalDevice() const { return m_physicalDevice; }
        VulkanInstance* GetInstance() const { return m_instance; }
        VmaAllocator GetAllocator() const { return m_allocator; }
        bool IsValidationLayersEnabled() const { return m_enableValidationLayers; }
        void DebugNameResource(VkObjectType type, uint64_t handle, const std::string& name);
        std::string GetDeviceName() const { return m_physicalDevice->GetDeviceName(); }
        VulkanSwapchain* GetSwapchain() const { return m_swapchain; }

        virtual RenderAPI CurrentRenderAPI() { return RenderAPI::Vulkan; }

    private:
        VulkanLogicalDevice* m_device;
        VulkanInstance* m_instance;
        VulkanPhysicalDevice* m_physicalDevice;
        VulkanSurface* m_surface;
        VulkanSwapchain* m_swapchain;
        VmaAllocator m_allocator;
        bool m_enableValidationLayers;
    };

    extern VulkanDevice* g_vkDevice;
    VulkanDevice* VKDevice();
}