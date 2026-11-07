#include <core/types.h>


static void get_basic_properties_and_limits(gpu_info::aggregate_t *t);
static void get_features(gpu_info::aggregate_t *t);
static void get_present_modes(gpu_info::aggregate_t *t);
static void get_swapchain_capabilities(gpu_info::aggregate_t *t);
static void get_swapchain_format(gpu_info::aggregate_t *t);
static void get_formats(gpu_info::aggregate_t *t);
static void get_memory_types(gpu_info::aggregate_t *t);
static void get_queues(gpu_info::aggregate_t *t);
static b8 validate(const gpu_info::aggregate_t *t);


namespace gpu_info
{
    void enumerate()
    {
        vulkan::vkEnumeratePhysicalDevices(renderer::vulkan_instance, &aggregate_count, nullptr);
        if (aggregate_count == 0)
        {
            logger::critical_error(logger::code::vkEnumeratePhysicalDevices, 0);
        }

        const auto handle_array = memory::allocate_from_temporary_block_1<VkPhysicalDevice>(aggregate_count);
        vulkan::vkEnumeratePhysicalDevices(renderer::vulkan_instance, &aggregate_count, handle_array);

        aggregate_valid_array = memory::allocate_from_core_block<b8>(aggregate_count);
        aggregate_array = memory::allocate_from_core_block<aggregate_t>(aggregate_count);

        for (u32 g = 0; g < aggregate_count; ++g)
        {
            aggregate_t *aggregate = &aggregate_array[g];

            aggregate->basic.handle = handle_array[g];

            get_basic_properties_and_limits(aggregate);
            get_features(aggregate);
            get_present_modes(aggregate);
            get_swapchain_capabilities(aggregate);
            get_swapchain_format(aggregate);
            get_formats(aggregate);
            get_memory_types(aggregate);
            get_queues(aggregate);

            aggregate_valid_array[g] = validate(aggregate);
        }

        memory::reset_temporary_block_1();
    }
}


static b8 validate(const gpu_info::aggregate_t *t)
{
    if (t->basic.type != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) return false;

    if (!t->features.synchronization2) return false;
    if (!t->features.dynamic_rendering) return false;
    if (!t->features.buffer_device_address) return false;
    if (!t->features.runtime_descriptor_array) return false;
    if (!t->features.descriptor_binding_partially_bound) return false;
    if (!t->features.descriptor_binding_sampled_image_update_after_bind) return false;
    if (!t->features.descriptor_binding_storage_buffer_update_after_bind) return false;
    if (!t->features.descriptor_binding_variable_descriptor_count) return false;
    if (!t->features.shader_sampled_image_array_non_uniform_indexing) return false;
    if (!t->features.descriptor_indexing) return false;
    if (!t->features.sampler_anisotropy) return false;
    if (!t->features.independent_blend) return false;

    if (t->format.depth_buffer == VK_FORMAT_UNDEFINED) return false;
    if (t->format.interaction_buffer == VK_FORMAT_UNDEFINED) return false;

    if (t->memory.device_local_type_bits == 0) return false;

    if (t->queue.default_family_idx == gpu_info::invalid_queue_family_idx) return false;

    if (t->swapchain.format == VK_FORMAT_UNDEFINED) return false;

    return true;
}


static void get_basic_properties_and_limits(gpu_info::aggregate_t *t)
{
    VkPhysicalDeviceSubgroupProperties subgroup_properties = {};
    subgroup_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_PROPERTIES;
    subgroup_properties.pNext = nullptr;

    VkPhysicalDeviceProperties2 device_properties2;
    device_properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    device_properties2.pNext = &subgroup_properties;

    vulkan::vkGetPhysicalDeviceProperties2(t->basic.handle, &device_properties2);

    t->basic.device_id = device_properties2.properties.deviceID;
    t->basic.vendor_id = device_properties2.properties.vendorID;
    t->basic.type = device_properties2.properties.deviceType;
    t->limit.min_uniform_buffer_offset_alignment = static_cast<u16>(device_properties2.properties.limits.minUniformBufferOffsetAlignment);
    t->limit.min_storage_buffer_offset_alignment = static_cast<u16>(device_properties2.properties.limits.minStorageBufferOffsetAlignment);
}


static void get_features(gpu_info::aggregate_t *t)
{
    VkPhysicalDeviceFeatures2 features2;
    VkPhysicalDeviceVulkan12Features features12;
    VkPhysicalDeviceVulkan13Features features13;

    features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features2.pNext = &features12;
    features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    features12.pNext = &features13;;
    features13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    features13.pNext = nullptr;

    vulkan::vkGetPhysicalDeviceFeatures2(t->basic.handle, &features2);

    t->features.synchronization2 = features13.synchronization2;
    t->features.dynamic_rendering = features13.dynamicRendering;
    t->features.buffer_device_address = features12.bufferDeviceAddress;
    t->features.runtime_descriptor_array = features12.runtimeDescriptorArray;
    t->features.descriptor_binding_partially_bound = features12.descriptorBindingPartiallyBound;
    t->features.descriptor_binding_variable_descriptor_count = features12.descriptorBindingVariableDescriptorCount;
    t->features.descriptor_binding_sampled_image_update_after_bind = features12.descriptorBindingSampledImageUpdateAfterBind;
    t->features.descriptor_binding_storage_buffer_update_after_bind = features12.descriptorBindingStorageBufferUpdateAfterBind;
    t->features.shader_sampled_image_array_non_uniform_indexing = features12.shaderSampledImageArrayNonUniformIndexing;
    t->features.descriptor_indexing = features12.descriptorIndexing;
    t->features.sampler_anisotropy = features2.features.samplerAnisotropy;
    t->features.independent_blend = features2.features.independentBlend;
}


static void get_present_modes(gpu_info::aggregate_t *t)
{
    u32 mode_count;
    vulkan::vkGetPhysicalDeviceSurfacePresentModesKHR(t->basic.handle, renderer::window_surface, &mode_count, nullptr);

    const auto mode_array = memory::allocate_from_temporary_block_1<VkPresentModeKHR>(mode_count);
    vulkan::vkGetPhysicalDeviceSurfacePresentModesKHR(t->basic.handle, renderer::window_surface, &mode_count, mode_array);

    for (u32 i = 0; i < mode_count; ++i)
    {
        if (mode_array[i] == VK_PRESENT_MODE_MAILBOX_KHR)
            t->swapchain.mailbox_present_mode = true;
        if (mode_array[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)
            t->swapchain.immediate_present_mode = true;
    }
}


static void get_swapchain_capabilities(gpu_info::aggregate_t *t)
{
    VkSurfaceCapabilitiesKHR capabilities;
    vulkan::vkGetPhysicalDeviceSurfaceCapabilitiesKHR(t->basic.handle, renderer::window_surface, &capabilities);

    t->swapchain.min_image_count = capabilities.minImageCount;
}


static void get_swapchain_format(gpu_info::aggregate_t *t)
{
    u32 format_count = 0;
    vulkan::vkGetPhysicalDeviceSurfaceFormatsKHR(t->basic.handle, renderer::window_surface, &format_count, nullptr);

    if (format_count == 0)
    {
        t->swapchain.format = VK_FORMAT_UNDEFINED;
        return;
    }

    const auto format_array = memory::allocate_from_temporary_block_1<VkSurfaceFormatKHR>(format_count);
    vulkan::vkGetPhysicalDeviceSurfaceFormatsKHR(t->basic.handle, renderer::window_surface, &format_count, format_array);

    u32 index = 0;

    for (u32 i = 0; i < format_count; ++i)
    {
        if (format_array[i].format == VK_FORMAT_B8G8R8A8_SRGB &&
            format_array[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            index = i;
            break;
        }
    }

    t->swapchain.format = format_array[index].format;
    t->swapchain.color_space = format_array[index].colorSpace;
}


static void get_formats(gpu_info::aggregate_t *t)
{
    vulkan::vkGetPhysicalDeviceFormatProperties(t->basic.handle, VK_FORMAT_D32_SFLOAT, &t->format.D32_SFLOAT);
    vulkan::vkGetPhysicalDeviceFormatProperties(t->basic.handle, VK_FORMAT_D32_SFLOAT_S8_UINT, &t->format.D32_SFLOAT_S8_UINT);
    vulkan::vkGetPhysicalDeviceFormatProperties(t->basic.handle, VK_FORMAT_D24_UNORM_S8_UINT, &t->format.D24_UNORM_S8_UINT);
    vulkan::vkGetPhysicalDeviceFormatProperties(t->basic.handle, VK_FORMAT_R32_UINT, &t->format.R32_UINT);

    t->format.depth_buffer = VK_FORMAT_UNDEFINED;
    t->format.interaction_buffer = VK_FORMAT_UNDEFINED;

    if (t->format.D32_SFLOAT.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
    {
        t->format.depth_buffer = VK_FORMAT_D32_SFLOAT;
    }
    else if (t->format.D32_SFLOAT_S8_UINT.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
    {
        t->format.depth_buffer = VK_FORMAT_D32_SFLOAT_S8_UINT;
    }
    else if (t->format.D24_UNORM_S8_UINT.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
    {
        t->format.depth_buffer = VK_FORMAT_D24_UNORM_S8_UINT;
    }
    if (t->format.R32_UINT.optimalTilingFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT)
    {
        t->format.interaction_buffer = VK_FORMAT_R32_UINT;
    }
}


static void get_memory_types(gpu_info::aggregate_t *t)
{
    VkPhysicalDeviceMemoryProperties device_memory;
    vulkan::vkGetPhysicalDeviceMemoryProperties(t->basic.handle, &device_memory);

    u32 device_local = 0;
    u32 host_visible = 0;
    u32 host_coherent = 0;

    for (u32 i = 0; i < device_memory.memoryTypeCount; ++i)
    {
        if (device_memory.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
        {
            device_local |= 1 << i;
        }
        if (device_memory.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
        {
            host_visible |= 1 << i;
        }
        if (device_memory.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
        {
            host_coherent |= 1 << i;
        }
    }

    t->memory.device_local_type_bits = device_local;
    t->memory.host_visible_type_bits = host_visible;
    t->memory.host_coherent_type_bits = host_coherent;
}


static void get_queues(gpu_info::aggregate_t *t)
{
    u32 family_count = 0;
    vulkan::vkGetPhysicalDeviceQueueFamilyProperties(t->basic.handle, &family_count, nullptr);

    const auto family_array = memory::allocate_from_temporary_block_1<VkQueueFamilyProperties>(family_count);
    vulkan::vkGetPhysicalDeviceQueueFamilyProperties(t->basic.handle, &family_count, family_array);

    t->queue.default_family_idx = gpu_info::invalid_queue_family_idx;

    for (u32 f = 0; f < family_count; ++f)
    {
        const auto flag = family_array[f].queueFlags;

        VkBool32 can_present;
        vulkan::vkGetPhysicalDeviceSurfaceSupportKHR(t->basic.handle, f, renderer::window_surface, &can_present);

        if (0 == (flag & VK_QUEUE_GRAPHICS_BIT) ||
            0 == (flag & VK_QUEUE_COMPUTE_BIT) ||
            0 == (flag & VK_QUEUE_TRANSFER_BIT) ||
            can_present == VK_FALSE)
            continue;

        t->queue.default_family_idx = f;
        break;
    }
}
