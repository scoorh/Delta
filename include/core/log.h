#pragma once


// ==================================================
// log codes
// ==================================================
namespace logger
{
    enum class code : u32
    {
        initialize_temporary_memory_block_1,
        initialize_temporary_memory_block_2,
        initialize_core_memory_block,

        load_vulkan_dll,

        RegisterClassExA,
        CreateWindowExA,

        vkCreateWin32SurfaceKHR,
        vkCreateInstance,
        vkCreateDebugUtilsMessengerEXT,
        vkEnumeratePhysicalDevices,
        vkCreateDevice,
        create_device_queue_mutex,

        vkCreateSwapchainKHR,
        vkGetSwapchainImagesKHR,
        vkCreateImageView_swapchain,

        vkCreateSemaphore_frame,
        vkCreateFence_frame,
        vkCreateCommandPool_frame,
        vkAllocateCommandBuffers_frame,

        vkWaitForFences_frame,
        vkResetFences_frame,
        vkAcquireNextImageKHR,
        vkBeginCommandBuffer_draw_to_swapchain,
        vkEndCommandBuffer_draw_to_swapchain,
        vkQueueSubmit2_draw_to_swapchain,
        vkQueuePresentKHR
    };
}


// ==================================================
// log variables
// ==================================================
namespace logger
{

}


// ==================================================
// log functions
// ==================================================
namespace logger
{
    void error(code code, u32 identifier);
    void critical_error(code code, u32 identifier);
    void critical_error(code code, s32 identifier);
}