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
        initialize_buffer_resource_pool,
        initialize_image_resource_pool,
        initialize_image_sampler_resource_pool,
        initialize_shader_resource_pool,
        initialize_pipeline_resource_pool,
        initialize_gpu_block_array,
        initialize_free_allocation_array,
        initialize_gpu_block,
        allocate_from_image_resource_pool,
        allocate_from_shader_resource_pool,
        allocate_from_buffer_resource_pool,
        allocate_from_pipeline_resource_pool,
        allocate_from_image_sampler_resource_pool,
        allocate_from_free_allocation_array,
        allocate_from_gpu_block_array,
        invalid_gpu_memory_type,

        vkCreateImage,
        vkCreateImageView,
        vkCreateShaderModule,
        vkCreateSampler,
        vkCreateBuffer,
        vkAllocateMemory,
        create_depth_buffer,
        create_interaction_buffer,
        vkCreateGraphicsPipelines,
        vkCreateComputePipelines,
        bad_graphics_pipeline_layout,
        missing_vertex_shader,
        missing_fragment_shader,
        missing_compute_shader,
        vkCreateDescriptorSetLayout_frame_data,
        vkCreateDescriptorSetLayout_image_data,
        vkCreateDescriptorSetLayout_ui_data,
        vkCreateDescriptorSetLayout_mesh_data,
        vkCreateDescriptorSetLayout_mesh_cull_data,
        vkCreatePipelineLayout_draw_mesh,
        vkCreatePipelineLayout_draw_ui,
        vkCreatePipelineLayout_cull_mesh,

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
    void error(code code, s32 identifier);
    void error(code code, u32 identifier);
    void critical_error(code code, u32 identifier);
    void critical_error(code code, s32 identifier);
}