#pragma once


// ==================================================
// resource types
// ==================================================
namespace resource
{
    struct buffer_t
    {

    };


    namespace image
    {
        struct setup_t
        {
            u16 id;
            u8 gpu_local_memory;
            u8 _pad0;
            VkFormat format;
            u16 width;
            u16 height;
            VkImageUsageFlags usage;
            VkImageAspectFlags aspect;
        };
        static_assert(offsetof(setup_t, id) == 0);
        static_assert(sizeof(setup_t) % 4 == 0);
        static_assert(sizeof(setup_t::usage) == sizeof(u32));
        static_assert(sizeof(setup_t::format) == sizeof(u32));
        static_assert(sizeof(setup_t::aspect) == sizeof(u32));
    }

    struct image_t
    {
        VkImage handle;
        VkImageView view;
        memory::allocation_t allocation;
        VkImageUsageFlags usage;
        u16 descriptor_index;
    };


    namespace image_sampler
    {
        struct setup_t {
            u16 id;
            u16 _pad0;
            VkFilter min_filter;
            VkFilter mag_filter;
            VkSamplerMipmapMode mipmap_mode;
            VkSamplerAddressMode address_mode;
        };
        static_assert(offsetof(setup_t, id) == 0);
        static_assert(sizeof(setup_t) % 4 == 0);
        static_assert(sizeof(setup_t::min_filter) == sizeof(u32));
        static_assert(sizeof(setup_t::mag_filter) == sizeof(u32));
        static_assert(sizeof(setup_t::mipmap_mode) == sizeof(u32));
        static_assert(sizeof(setup_t::address_mode) == sizeof(u32));
    }

    struct image_sampler_t
    {
        VkSampler handle;
        u16 descriptor_index;
    };


    namespace shader
    {
        struct setup_t
        {
            u16 id;
            u16 _pad0;
            u32 code_size;
            u32 package_data_offset;
        };
        static_assert(offsetof(setup_t, id) == 0);
        static_assert(sizeof(setup_t) % 4 == 0);
    }

    struct shader_t
    {
        VkShaderModule handle;
    };


    struct pipeline_t
    {

    };
}


// ==================================================
// buffer functions
// ==================================================
namespace resource::buffer
{

}


// ==================================================
// image functions
// ==================================================
namespace resource::image
{
    image_t* create(const setup_t* setup);
    void destroy(image_t* image);
    image_t* find(u16 id);
}


// ==================================================
// image sampler functions
// ==================================================
namespace resource::image_sampler
{
    image_sampler_t* create(const setup_t* setup);
    void destroy(image_sampler_t* image_sampler);
    image_sampler_t* find(u16 id);
}


// ==================================================
// shader functions
// ==================================================
namespace resource::shader
{
    shader_t* create(const setup_t* setup, const u32* spirv_code);
    void destroy(shader_t* shader);
    shader_t* find(u16 id);
    VkShaderModule find_module(u16 id);
}


// ==================================================
// pipeline functions
// ==================================================
namespace resource::pipeline
{

}