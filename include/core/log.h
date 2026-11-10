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
        initialize_material_resource_pool,
        initialize_mesh_resource_pool,
        initialize_gpu_block_array,
        initialize_free_allocation_array,
        initialize_gpu_block,
        allocate_from_image_resource_pool,
        allocate_from_mesh_resource_pool,
        allocate_from_shader_resource_pool,
        allocate_from_buffer_resource_pool,
        allocate_from_material_resource_pool,
        allocate_from_pipeline_resource_pool,
        allocate_from_image_sampler_resource_pool,
        allocate_from_free_allocation_array,
        allocate_from_gpu_block_array,
        invalid_gpu_memory_type,

        open_resource_package,

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
        missing_index_buffer,
        missing_position_vertex_buffer,
        missing_attribute_vertex_buffer,
        reserve_index_buffer_space,
        reserve_position_vertex_buffer_space,
        reserve_attribute_vertex_buffer_space,

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

        vkCreateFence_gpu_transfer_work,
        vkCreateSemaphore_gpu_transfer_work,
        vkCreateCommandPool_gpu_transfer_work,
        vkAllocateCommandBuffers_gpu_transfer_work,
        create_gpu_transfer_work_staging_buffer,
        vkMapMemory_gpu_transfer_work_staging_buffer,
        vkGetFenceStatus_gpu_transfer_work,
        vkBeginCommandBuffer_gpu_transfer_work,
        vkEndCommandBuffer_gpu_transfer_work,
        vkQueueSubmit2_gpu_transfer_work,

        missing_cull_mesh_pipeline,
        vkCreateSemaphore_frame,
        vkCreateFence_frame,
        vkCreateCommandPool_frame,
        vkAllocateCommandBuffers_frame,
        vkCreateDescriptorPool_frame,
        vkAllocateDescriptorSets_frame,
        create_frame_data_buffer,

        vkWaitForFences_frame,
        vkResetFences_frame,
        vkAcquireNextImageKHR,
        vkBeginCommandBuffer_compute,
        vkEndCommandBuffer_compute,
        vkQueueSubmit2_compute,
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
    void error(code code, const c8* identifier);
    void error(code code, s32 identifier);
    void error(code code, u32 identifier);
    void critical_error(code code, u32 identifier);
    void critical_error(code code, s32 identifier);
}