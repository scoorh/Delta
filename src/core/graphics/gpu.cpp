#include <variant>
#include <core/types.h>


static void pick_device();
static void create_handle();
static void get_functions();
static void get_queues();
static void create_mutex();


namespace renderer
{
    void initialize_device()
    {
        pick_device();
        create_handle();
        get_functions();
        get_queues();
        create_mutex();
    }


    void lock_queue_mutex()
    {
        WaitForSingleObject(queue_mutex, INFINITE);
    }


    void unlock_queue_mutex()
    {
        ReleaseMutex(queue_mutex);
    }
}


static void pick_device()
{
    for (u32 g = 0; g < gpu_info::aggregate_count; ++g)
    {
        //TODO:  instead of valid 0 or 1  do scoring, if 0 then skip and pick best suited
        if (!gpu_info::aggregate_valid_array[g])
        {
            continue;
        }

        gpu_info::basic = &gpu_info::aggregate_array[g].basic;
        gpu_info::queue = &gpu_info::aggregate_array[g].queue;
        gpu_info::features = &gpu_info::aggregate_array[g].features;
        gpu_info::format = &gpu_info::aggregate_array[g].format;
        gpu_info::limit = &gpu_info::aggregate_array[g].limit;
        gpu_info::memory = &gpu_info::aggregate_array[g].memory;
        gpu_info::swapchain = &gpu_info::aggregate_array[g].swapchain;

        if (gpu_info::basic->device_id == settings::renderer::device_id &&
            gpu_info::basic->vendor_id == settings::renderer::device_vendor_id)
        {
            break;
        }
    }
}


static void create_handle()
{
    u32 layer_count = 0;
    u32 extension_count = 1;
    const c8* layers[1];
    const c8* extensions[1];

    extensions[0] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
#ifndef NDEBUG
    layers[0] = vulkan::validation_layer_name;
    layer_count = 1;
#endif


    VkPhysicalDeviceFeatures2 features2 = {};
    VkPhysicalDeviceVulkan12Features features12 = {};
    VkPhysicalDeviceVulkan13Features features13 = {};

    features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features2.pNext = &features12;
    features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    features12.pNext = &features13;
    features13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;

    features2.features.samplerAnisotropy = VK_TRUE;
    features2.features.independentBlend = VK_TRUE;
    features12.bufferDeviceAddress = VK_TRUE;
    features12.runtimeDescriptorArray = VK_TRUE;
    features12.descriptorBindingPartiallyBound = VK_TRUE;
    features12.descriptorBindingVariableDescriptorCount = VK_TRUE;
    features12.descriptorBindingSampledImageUpdateAfterBind = VK_TRUE;
    features12.descriptorBindingStorageBufferUpdateAfterBind = VK_TRUE;
    features12.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
    features12.descriptorIndexing = VK_TRUE;
    features13.synchronization2 = VK_TRUE;
    features13.dynamicRendering = VK_TRUE;


    f32 priorities[1] = { 1.0f };
    u32 queue_count = 1;
    VkDeviceQueueCreateInfo default_queue_create_info = {};
    default_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    default_queue_create_info.queueFamilyIndex = gpu_info::queue->default_family_idx;
    default_queue_create_info.queueCount = 1;
    default_queue_create_info.pQueuePriorities = priorities;


    VkDeviceCreateInfo device_create_info = {};
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.pNext = &features2;
    device_create_info.queueCreateInfoCount = queue_count;
    device_create_info.pQueueCreateInfos = &default_queue_create_info;
    device_create_info.enabledLayerCount = layer_count;
    device_create_info.ppEnabledLayerNames = layers;
    device_create_info.enabledExtensionCount = extension_count;
    device_create_info.ppEnabledExtensionNames = extensions;


    const VkResult result = vulkan::vkCreateDevice(gpu_info::basic->handle, &device_create_info, nullptr, &renderer::device);
    if (result) {
        logger::critical_error(logger::code::vkCreateDevice, result);
    }
}


static void get_functions()
{
    vulkan::vkGetDeviceProcAddr = get_vulkan_instance_function(vkGetDeviceProcAddr);

    vulkan::vkGetDeviceQueue = get_vulkan_device_function(vkGetDeviceQueue);

    vulkan::vkAllocateMemory = get_vulkan_device_function(vkAllocateMemory);
    vulkan::vkCreateFence = get_vulkan_device_function(vkCreateFence);
    vulkan::vkCreateSemaphore = get_vulkan_device_function(vkCreateSemaphore);
    vulkan::vkCreateCommandPool = get_vulkan_device_function(vkCreateCommandPool);
    vulkan::vkAllocateCommandBuffers = get_vulkan_device_function(vkAllocateCommandBuffers);
    vulkan::vkCreateSwapchainKHR = get_vulkan_device_function(vkCreateSwapchainKHR);
    vulkan::vkGetSwapchainImagesKHR = get_vulkan_device_function(vkGetSwapchainImagesKHR);
    vulkan::vkCreateSampler = get_vulkan_device_function(vkCreateSampler);
    vulkan::vkDestroySampler = get_vulkan_device_function(vkDestroySampler);
    vulkan::vkCreateImage = get_vulkan_device_function(vkCreateImage);
    vulkan::vkGetImageMemoryRequirements = get_vulkan_device_function(vkGetImageMemoryRequirements);
    vulkan::vkBindImageMemory = get_vulkan_device_function(vkBindImageMemory);
    vulkan::vkDestroyImageView = get_vulkan_device_function(vkDestroyImageView);
    vulkan::vkDestroyImage = get_vulkan_device_function(vkDestroyImage);
    vulkan::vkCreateImageView = get_vulkan_device_function(vkCreateImageView);
    vulkan::vkCreateBuffer = get_vulkan_device_function(vkCreateBuffer);
    vulkan::vkGetBufferMemoryRequirements = get_vulkan_device_function(vkGetBufferMemoryRequirements);
    vulkan::vkBindBufferMemory = get_vulkan_device_function(vkBindBufferMemory);
    vulkan::vkMapMemory = get_vulkan_device_function(vkMapMemory);
    vulkan::vkDestroyBuffer = get_vulkan_device_function(vkDestroyBuffer);
    vulkan::vkCreateGraphicsPipelines = get_vulkan_device_function(vkCreateGraphicsPipelines);
    vulkan::vkCreateComputePipelines = get_vulkan_device_function(vkCreateComputePipelines);
    vulkan::vkDestroyPipeline = get_vulkan_device_function(vkDestroyPipeline);
    vulkan::vkCreateShaderModule = get_vulkan_device_function(vkCreateShaderModule);
    vulkan::vkDestroyShaderModule = get_vulkan_device_function(vkDestroyShaderModule);
    vulkan::vkCreatePipelineLayout = get_vulkan_device_function(vkCreatePipelineLayout);
    vulkan::vkCreateDescriptorSetLayout = get_vulkan_device_function(vkCreateDescriptorSetLayout);
    vulkan::vkCreateDescriptorPool = get_vulkan_device_function(vkCreateDescriptorPool);
    vulkan::vkAllocateDescriptorSets = get_vulkan_device_function(vkAllocateDescriptorSets);
    vulkan::vkUpdateDescriptorSets = get_vulkan_device_function(vkUpdateDescriptorSets);

    vulkan::vkWaitForFences = get_vulkan_device_function(vkWaitForFences);
    vulkan::vkResetFences = get_vulkan_device_function(vkResetFences);
    vulkan::vkAcquireNextImageKHR = get_vulkan_device_function(vkAcquireNextImageKHR);
    vulkan::vkResetCommandPool = get_vulkan_device_function(vkResetCommandPool);
    vulkan::vkBeginCommandBuffer = get_vulkan_device_function(vkBeginCommandBuffer);
    vulkan::vkCmdCopyBuffer2 = get_vulkan_device_function(vkCmdCopyBuffer2);
    vulkan::vkCmdPipelineBarrier2 = get_vulkan_device_function(vkCmdPipelineBarrier2);
    vulkan::vkCmdBeginRendering = get_vulkan_device_function(vkCmdBeginRendering);
    vulkan::vkCmdSetViewport = get_vulkan_device_function(vkCmdSetViewport);
    vulkan::vkCmdSetScissor = get_vulkan_device_function(vkCmdSetScissor);
    vulkan::vkCmdBindPipeline = get_vulkan_device_function(vkCmdBindPipeline);
    vulkan::vkCmdBindDescriptorSets = get_vulkan_device_function(vkCmdBindDescriptorSets);
    vulkan::vkCmdPushConstants = get_vulkan_device_function(vkCmdPushConstants);
    vulkan::vkCmdBindIndexBuffer = get_vulkan_device_function(vkCmdBindIndexBuffer);
    vulkan::vkCmdDraw = get_vulkan_device_function(vkCmdDraw);
    vulkan::vkCmdDrawIndexed = get_vulkan_device_function(vkCmdDrawIndexed);
    vulkan::vkCmdDrawIndexedIndirect = get_vulkan_device_function(vkCmdDrawIndexedIndirect);
    vulkan::vkCmdDispatch = get_vulkan_device_function(vkCmdDispatch);
    vulkan::vkCmdEndRendering = get_vulkan_device_function(vkCmdEndRendering);
    vulkan::vkEndCommandBuffer = get_vulkan_device_function(vkEndCommandBuffer);
    vulkan::vkQueueSubmit2 = get_vulkan_device_function(vkQueueSubmit2);
    vulkan::vkQueuePresentKHR = get_vulkan_device_function(vkQueuePresentKHR);
}


static void get_queues()
{
    vulkan::vkGetDeviceQueue(renderer::device, gpu_info::queue->default_family_idx, 0, &renderer::default_queue);
}


static void create_mutex()
{
    renderer::queue_mutex = CreateMutexA(nullptr, 0, nullptr);
    if (!renderer::queue_mutex)
    {
        logger::critical_error(logger::code::create_device_queue_mutex, 0);
    }
}