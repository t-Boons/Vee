#include "platform/vulkan/vulkan_device.hpp"
#include "platform/vulkan/vulkan_common.hpp"
#include "vulkan/vulkan.hpp"
#include <GLFW/glfw3.h>

namespace vee
{
    // https://vulkan-tutorial.com/en/Drawing_a_triangle/Setup/Validation_layers
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    // https://vulkan-tutorial.com/en/Drawing_a_triangle/Setup/Validation_layers
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
    {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        func(instance, debugMessenger, pAllocator);
    }
}
    // https://vulkan-tutorial.com/en/Drawing_a_triangle/Setup/Validation_layers
    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
    {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = DebugCallback;
    }

    void PopulateApplicationInfo(VkApplicationInfo& appInfo)
    {
        appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Vee Application";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Vee Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;
    }

    bool CheckValidationLayerSupport(const std::vector<const char*>& validationLayers)
    {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : validationLayers) {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) {
                return false;
            }
        }

        return true;
    }

    #ifdef NDEBUG
        const bool enableValidationLayers = false;
    #else
        const bool enableValidationLayers = true;
    #endif

    VulkanDevice::VulkanDevice()
    {
        m_enableValidationLayers = enableValidationLayers;

        VkApplicationInfo appInfo;
        PopulateApplicationInfo(appInfo);

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        std::vector<const char *> validationLayers = {
            "VK_LAYER_KHRONOS_validation"};

        if (m_enableValidationLayers && !CheckValidationLayerSupport(validationLayers))
        {
            Log::Error("Validation layers requested but not available.");
            m_enableValidationLayers = false;
        }

        if (m_enableValidationLayers)
        {

            VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();

            PopulateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
        } 
        else
        {
            createInfo.enabledLayerCount = 0;
            createInfo.pNext = nullptr;
        }

        // Extentions
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

        Log::Info("Available Vulkan extensions:");
        for (const auto& extension : extensions)
        {
            Log::Info("  %s", extension.extensionName);
        }
        Log::Info("");

        createInfo.enabledExtensionCount = 0;
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledExtensionNames = nullptr;

        std::vector<const char*> extensionsNames = GetExtentions(enableValidationLayers);

        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensionsNames.size());
        createInfo.ppEnabledExtensionNames = extensionsNames.data();



        VkDebugUtilsMessengerCreateInfoEXT validationLayercreateInfo{};
        validationLayercreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        validationLayercreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        validationLayercreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        validationLayercreateInfo.pfnUserCallback = DebugCallback;
        validationLayercreateInfo.pUserData = nullptr; // Optional
        
        VKValidate(vkCreateInstance(&createInfo, nullptr, &m_instance));
    }

    void VulkanDevice::SetupDebugMessenger()
    {

        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        PopulateDebugMessengerCreateInfo(createInfo);

        CreateDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &m_debugMessenger);
    }

    std::vector<const char*> VulkanDevice::GetExtentions(bool validationLayers)
    {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (validationLayers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    VulkanDevice::~VulkanDevice()
    {
        if(m_enableValidationLayers)
        {
            DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
        }
        
        vkDestroyInstance(m_instance, nullptr);
    }
}
