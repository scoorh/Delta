#pragma once


// ==================================================
// gpu info constants
// ==================================================
namespace gpu_info
{
    constexpr u32 invalid_queue_family_idx = 0xffffffff;
}


// ==================================================
// types
// ==================================================
namespace gpu_info
{
    struct basic_t
    {
        VkPhysicalDevice handle;
        VkPhysicalDeviceType type;
        u32 device_id;
        u32 vendor_id;
    };


    struct queue_t
    {
        u32 default_family_idx;
    };


    struct format_t
    {
        VkFormat depth_buffer;
        VkFormat interaction_buffer;

        VkFormatProperties D32_SFLOAT;
        VkFormatProperties D32_SFLOAT_S8_UINT;
        VkFormatProperties D24_UNORM_S8_UINT;
        VkFormatProperties R32_UINT;
    };


    struct features_t
    {
        b8 synchronization2;
        b8 dynamic_rendering;
        b8 buffer_device_address;
        b8 runtime_descriptor_array;
        b8 descriptor_binding_partially_bound;
        b8 descriptor_binding_variable_descriptor_count;
        b8 descriptor_binding_sampled_image_update_after_bind;
        b8 descriptor_binding_storage_buffer_update_after_bind;
        b8 shader_sampled_image_array_non_uniform_indexing;
        b8 descriptor_indexing;
        b8 sampler_anisotropy;
        b8 independent_blend;
    };


    struct limit_t
    {
        u16 min_uniform_buffer_offset_alignment;
        u16 min_storage_buffer_offset_alignment;
    };


    struct memory_t
    {
        u32 device_local_type_bits;
        u32 host_visible_type_bits;
        u32 host_coherent_type_bits;
    };


    struct swapchain_t
    {
        b8 mailbox_present_mode;
        b8 immediate_present_mode;
        u32 min_image_count;
        VkFormat format;
        VkColorSpaceKHR color_space;
    };


    struct aggregate_t
    {
        basic_t basic;
        queue_t queue;
        features_t features;
        format_t format;
        limit_t limit;
        memory_t memory;
        swapchain_t swapchain;
    };
}


// ==================================================
// gpu info variables
// ==================================================
namespace gpu_info
{
    inline u32 aggregate_count = 0;
    inline b8* aggregate_valid_array = nullptr;
    inline aggregate_t* aggregate_array = nullptr;

    inline basic_t* basic = nullptr;
    inline queue_t* queue = nullptr;
    inline features_t* features = nullptr;
    inline format_t* format = nullptr;
    inline limit_t* limit = nullptr;
    inline memory_t* memory = nullptr;
    inline swapchain_t* swapchain = nullptr;
}


// ==================================================
// gpu_info functions
// ==================================================
namespace gpu_info
{
    void enumerate();
}