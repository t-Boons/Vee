#include "platform/vulkan/vulkan_device_physical_device.hpp"

namespace vee
{
    bool IsPhysicalDeviceSuitable(VkPhysicalDevice device)
    {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
    }

    int RatePhysicalDevice(VkPhysicalDevice device)
    {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        int score = 0;

        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            score += 1000;
        }

        score += deviceProperties.limits.maxImageDimension2D;

        return score;
    }

    VulkanPhysicalDevice::VulkanPhysicalDevice(VulkanInstance* instance)
    {
        SelectBestPhysicalDevice(instance);
    }

    void VulkanPhysicalDevice::SelectBestPhysicalDevice(VulkanInstance* instance)
    {
        // Get all the physical devices.
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance->GetVkInstance(), &deviceCount, nullptr);
        Log::Assert(deviceCount != 0, "Failed to find GPUs with Vulkan support.");

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance->GetVkInstance(), &deviceCount, devices.data());

        // Select the best device.
        int bestScore = 0;
        for (const auto &device : devices)
        {
            const int score = RatePhysicalDevice(device);
            if (score > bestScore)
            {
                m_physicalDevice = device;
                vkGetPhysicalDeviceProperties(device, &m_deviceProperties);
            }
        }

        Log::Assert(m_physicalDevice != VK_NULL_HANDLE, "Failed to find a suitable GPU.");
    }
}