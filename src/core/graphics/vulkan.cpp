#include <core/types.h>


static void create_instance();
static void create_debug_messenger();
static void get_instance_functions();


namespace renderer
{
    void initialize_vulkan()
    {
        create_instance();
        get_instance_functions();
        create_debug_messenger();
    }
}


static void create_instance()
{
    u32 layer_count = 0;
    u32 extension_count = 2;
    const c8 *layers[1];
    const c8 *extensions[3];

    extensions[0] = VK_KHR_SURFACE_EXTENSION_NAME;
    extensions[1] = vulkan::surface_extension_name;
#ifndef NDEBUG
    extensions[2] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
    layers[0] = vulkan::validation_layer_name;
    layer_count = 1;
    extension_count = 3;
#endif


    VkApplicationInfo application_info;
    application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    application_info.pNext = nullptr;
    application_info.pApplicationName = settings::application::name;
    application_info.applicationVersion = settings::application::version.packed;
    application_info.pEngineName = settings::application::name;
    application_info.engineVersion = settings::application::version.packed;
    application_info.apiVersion = vulkan::api_version;


#ifndef NDEBUG
    constexpr u32 validation_features_count = 2;
    constexpr VkValidationFeatureEnableEXT validation_features_array[validation_features_count] =
    {
        VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT,
        VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT,
    };

    VkValidationFeaturesEXT validation_features;
    validation_features.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
    validation_features.pNext = nullptr;
    validation_features.enabledValidationFeatureCount = validation_features_count;
    validation_features.pEnabledValidationFeatures = validation_features_array;
    validation_features.disabledValidationFeatureCount = 0;
    validation_features.pDisabledValidationFeatures = nullptr;
#endif


    VkInstanceCreateInfo instance_create_info;
    instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_create_info.pNext = nullptr;
    instance_create_info.flags = 0;
#ifndef NDEBUG
    instance_create_info.pNext = &validation_features;
#endif
    instance_create_info.pApplicationInfo = &application_info;
    instance_create_info.enabledLayerCount = layer_count;
    instance_create_info.ppEnabledLayerNames = layers;
    instance_create_info.enabledExtensionCount = extension_count;
    instance_create_info.ppEnabledExtensionNames = extensions;


    const auto vkCreateInstance = get_vulkan_dll_function(vkCreateInstance);
    vulkan::vkGetInstanceProcAddr = get_vulkan_dll_function(vkGetInstanceProcAddr);

    const VkResult result = vkCreateInstance(&instance_create_info, nullptr, &renderer::vulkan_instance);
    if (result)
    {
        logger::critical_error(logger::code::vkCreateInstance, result);
    }
}


static void get_instance_functions()
{
    vulkan::vkEnumeratePhysicalDevices = get_vulkan_instance_function(vkEnumeratePhysicalDevices);
    vulkan::vkGetPhysicalDeviceFeatures2 = get_vulkan_instance_function(vkGetPhysicalDeviceFeatures2);
    vulkan::vkGetPhysicalDeviceProperties2 = get_vulkan_instance_function(vkGetPhysicalDeviceProperties2);
    vulkan::vkCreateDevice = get_vulkan_instance_function(vkCreateDevice);
    vulkan::vkGetPhysicalDeviceQueueFamilyProperties = get_vulkan_instance_function(vkGetPhysicalDeviceQueueFamilyProperties);
    vulkan::vkGetPhysicalDeviceSurfaceSupportKHR = get_vulkan_instance_function(vkGetPhysicalDeviceSurfaceSupportKHR);
    vulkan::vkGetPhysicalDeviceMemoryProperties = get_vulkan_instance_function(vkGetPhysicalDeviceMemoryProperties);
    vulkan::vkGetPhysicalDeviceFormatProperties = get_vulkan_instance_function(vkGetPhysicalDeviceFormatProperties);
    vulkan::vkGetPhysicalDeviceSurfaceFormatsKHR = get_vulkan_instance_function(vkGetPhysicalDeviceSurfaceFormatsKHR);
    vulkan::vkGetPhysicalDeviceSurfaceCapabilitiesKHR = get_vulkan_instance_function(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
    vulkan::vkGetPhysicalDeviceSurfacePresentModesKHR = get_vulkan_instance_function(vkGetPhysicalDeviceSurfacePresentModesKHR);
    vulkan::vkCreateWin32SurfaceKHR = get_vulkan_instance_function(vkCreateWin32SurfaceKHR);
}


#ifndef NDEBUG
#include <cstdio>
static VKAPI_ATTR VkBool32 VKAPI_CALL vk_messenger_callback([[maybe_unused]] VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                                                            [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT message_type,
                                                            const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
                                                            [[maybe_unused]] void *user_data)
{
    printf("%s\n", callback_data->pMessage);
    return VK_FALSE;
}
#endif


static void create_debug_messenger()
{
#ifndef NDEBUG
    VkDebugUtilsMessengerCreateInfoEXT debug_messenger_create_info = {};
    debug_messenger_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debug_messenger_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debug_messenger_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debug_messenger_create_info.pfnUserCallback = vk_messenger_callback;

    VkDebugUtilsMessengerEXT debug_messenger;
    const auto vkCreateDebugUtilsMessengerEXT = get_vulkan_instance_function(vkCreateDebugUtilsMessengerEXT);

    const VkResult result = vkCreateDebugUtilsMessengerEXT(renderer::vulkan_instance, &debug_messenger_create_info, nullptr, &debug_messenger);
    if (result)
    {
        logger::critical_error(logger::code::vkCreateDebugUtilsMessengerEXT, result);
    }
#endif
}
