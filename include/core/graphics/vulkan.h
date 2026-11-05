#pragma once


// ==================================================
// vulkan constants
// ==================================================
namespace vulkan
{
    constexpr auto validation_layer_name = "VK_LAYER_KHRONOS_validation";
    constexpr auto surface_extension_name = "VK_KHR_win32_surface";

    constexpr auto api_version = VK_API_VERSION_1_3;
}


// ==================================================
// vulkan functions
// ==================================================
namespace vulkan
{
    inline PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = nullptr;

    inline PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices = nullptr;
    inline PFN_vkGetPhysicalDeviceFeatures2 vkGetPhysicalDeviceFeatures2 = nullptr;
    inline PFN_vkGetPhysicalDeviceProperties2 vkGetPhysicalDeviceProperties2 = nullptr;
    inline PFN_vkCreateDevice vkCreateDevice = nullptr;
    inline PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties = nullptr;
    inline PFN_vkGetPhysicalDeviceSurfaceSupportKHR vkGetPhysicalDeviceSurfaceSupportKHR = nullptr;
    inline PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties = nullptr;
    inline PFN_vkGetPhysicalDeviceFormatProperties vkGetPhysicalDeviceFormatProperties = nullptr;
    inline PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHR = nullptr;
    inline PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilitiesKHR = nullptr;
    inline PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vkGetPhysicalDeviceSurfacePresentModesKHR = nullptr;
    inline PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR = nullptr;

    inline PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr = nullptr;

    inline PFN_vkGetDeviceQueue vkGetDeviceQueue = nullptr;

    inline PFN_vkAllocateMemory vkAllocateMemory = nullptr;
    inline PFN_vkCreateFence vkCreateFence = nullptr;
    inline PFN_vkCreateSemaphore vkCreateSemaphore = nullptr;
    inline PFN_vkCreateCommandPool vkCreateCommandPool = nullptr;
    inline PFN_vkAllocateCommandBuffers vkAllocateCommandBuffers = nullptr;
    inline PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR = nullptr;
    inline PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR = nullptr;
    inline PFN_vkCreateImage vkCreateImage = nullptr;
    inline PFN_vkGetImageMemoryRequirements vkGetImageMemoryRequirements = nullptr;
    inline PFN_vkBindImageMemory vkBindImageMemory = nullptr;
    inline PFN_vkCreateImageView vkCreateImageView = nullptr;
    inline PFN_vkDestroyImageView vkDestroyImageView = nullptr;
    inline PFN_vkDestroyImage vkDestroyImage = nullptr;
    inline PFN_vkCreateBuffer vkCreateBuffer = nullptr;
    inline PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirements = nullptr;
    inline PFN_vkBindBufferMemory vkBindBufferMemory = nullptr;
    inline PFN_vkMapMemory vkMapMemory = nullptr;
    inline PFN_vkDestroyBuffer vkDestroyBuffer = nullptr;
    inline PFN_vkCreateSampler vkCreateSampler = nullptr;
    inline PFN_vkDestroySampler vkDestroySampler = nullptr;
    inline PFN_vkCreateGraphicsPipelines vkCreateGraphicsPipelines = nullptr;
    inline PFN_vkCreateComputePipelines vkCreateComputePipelines = nullptr;
    inline PFN_vkDestroyPipeline vkDestroyPipeline = nullptr;
    inline PFN_vkCreateShaderModule vkCreateShaderModule = nullptr;;
    inline PFN_vkDestroyShaderModule vkDestroyShaderModule = nullptr;
    inline PFN_vkCreatePipelineLayout vkCreatePipelineLayout = nullptr;
    inline PFN_vkCreateDescriptorSetLayout vkCreateDescriptorSetLayout = nullptr;
    inline PFN_vkCreateDescriptorPool vkCreateDescriptorPool = nullptr;
    inline PFN_vkAllocateDescriptorSets vkAllocateDescriptorSets = nullptr;
    inline PFN_vkUpdateDescriptorSets vkUpdateDescriptorSets = nullptr;

    inline PFN_vkWaitForFences vkWaitForFences = nullptr;
    inline PFN_vkResetFences vkResetFences = nullptr;
    inline PFN_vkAcquireNextImageKHR vkAcquireNextImageKHR = nullptr;
    inline PFN_vkResetCommandPool vkResetCommandPool = nullptr;
    inline PFN_vkBeginCommandBuffer vkBeginCommandBuffer = nullptr;
    inline PFN_vkCmdCopyBuffer2 vkCmdCopyBuffer2 = nullptr;
    inline PFN_vkCmdPipelineBarrier2 vkCmdPipelineBarrier2 = nullptr;
    inline PFN_vkCmdBeginRendering vkCmdBeginRendering = nullptr;
    inline PFN_vkCmdSetViewport vkCmdSetViewport = nullptr;
    inline PFN_vkCmdSetScissor vkCmdSetScissor = nullptr;
    inline PFN_vkCmdBindPipeline vkCmdBindPipeline = nullptr;
    inline PFN_vkCmdBindDescriptorSets vkCmdBindDescriptorSets = nullptr;
    inline PFN_vkCmdPushConstants vkCmdPushConstants = nullptr;
    inline PFN_vkCmdBindIndexBuffer vkCmdBindIndexBuffer = nullptr;
    inline PFN_vkCmdDraw vkCmdDraw = nullptr;
    inline PFN_vkCmdDrawIndexed vkCmdDrawIndexed = nullptr;
    inline PFN_vkCmdDrawIndexedIndirect vkCmdDrawIndexedIndirect = nullptr;
    inline PFN_vkCmdDispatch vkCmdDispatch = nullptr;
    inline PFN_vkCmdEndRendering vkCmdEndRendering = nullptr;
    inline PFN_vkEndCommandBuffer vkEndCommandBuffer = nullptr;
    inline PFN_vkQueueSubmit2 vkQueueSubmit2 = nullptr;
    inline PFN_vkQueuePresentKHR vkQueuePresentKHR = nullptr;
}


// ==================================================
// get function macros
// ==================================================
#define get_vulkan_instance_function(f) (PFN_##f)vulkan::vkGetInstanceProcAddr(renderer::vulkan_instance, #f)