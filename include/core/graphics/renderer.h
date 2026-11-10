#pragma once


// ==================================================
// renderer constants
// ==================================================
namespace renderer
{
    constexpr u32 frames_in_flight = 2;

    constexpr u32 max_swapchain_image_count = 4;
}


// ==================================================
// renderer variables
// ==================================================
namespace renderer
{
    inline HWND window_handle = nullptr;
    inline HINSTANCE window_instance = nullptr;
    inline VkInstance vulkan_instance = nullptr;
    inline VkSurfaceKHR window_surface = nullptr;

    inline u32 current_frame = 0;

    inline u16 window_width = 0;
    inline u16 window_height = 0;
    inline VkSwapchainKHR swapchain = nullptr;
    inline u32 swapchain_image_count = 0;
    inline VkImage swapchain_image_array[max_swapchain_image_count] = {};
    inline VkImageView swapchain_image_view_array[max_swapchain_image_count] = {};
    inline VkImage interaction_buffer_image = nullptr;
    inline VkImage depth_buffer_image = nullptr;
    inline VkImageView interaction_buffer_image_view = nullptr;
    inline VkImageView depth_buffer_image_view = nullptr;

    struct frame_t
    {
        VkDescriptorPool descriptor_pool;
        struct
        {
            VkDescriptorSet frame_data;
            VkDescriptorSet mesh_data;
            VkDescriptorSet image_data;
            VkDescriptorSet ui_data;
            VkDescriptorSet mesh_cull_data;
            VkDescriptorSet draw_mesh_array[resource::pipeline_layout::draw_mesh::descriptor_count];
            VkDescriptorSet cull_mesh_array[resource::pipeline_layout::cull_mesh::descriptor_count];
            VkDescriptorSet draw_ui_array[resource::pipeline_layout::draw_ui::descriptor_count];
        } descriptor_set;

        resource::gpu_transfer_work_t* gpu_transfer_work;

        VkBuffer data_buffer;
        struct
        {
            u32 frame_uniform;
            u32 mesh_indirect_draw_cmds_data;
            u32 mesh_indirect_draw_cmds;
            u32 mesh_instance_indices;
            u32 mesh_instances;
            u32 materials;
            u32 ui_instances;
        } data_offset;

        VkFence work_completed;
        u32 acquired_swapchain_image_idx;
        VkSemaphore acquired_swapchain_image;
        VkSemaphore draw_to_swapchain_completed;
        VkSemaphore gpu_transfer_completed;
        VkSemaphore compute_completed;

        VkPipeline cull_mesh_pipeline;
        VkPipelineLayout cull_mesh_pipeline_layout;
        u32 compute_shader_wave_size;

        VkCommandPool default_command_pool;
        VkCommandBuffer draw_to_swapchain_cmd_buffer;
        VkCommandBuffer compute_cmd_buffer;
    };
    inline frame_t frames[frames_in_flight] = {};

    inline VkDevice device = nullptr;
    inline HANDLE queue_mutex = nullptr;
    inline VkQueue default_queue = nullptr;
}


// ==================================================
// renderer functions
// ==================================================
namespace renderer
{
    void initialize();
    void initialize_vulkan();
    void initialize_device();
    void initialize_swapchain();
    void initialize_frames();

    void lock_queue_mutex();
    void unlock_queue_mutex();

    void draw_frame();
}