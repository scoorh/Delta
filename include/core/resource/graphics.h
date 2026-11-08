#pragma once


// ==================================================
// resource types
// ==================================================
namespace resource
{
    // ==================================================
    // buffer
    // ==================================================
    namespace buffer
    {
        struct setup_t
        {
            u16 id;
            b8 gpu_local_memory;
            b8 _pad0;
            u32 size;
            VkBufferUsageFlags usage;
        };

        static_assert(offsetof(setup_t, id) == 0);
        static_assert(sizeof(setup_t) % 4 == 0);
        static_assert(sizeof(setup_t::usage) == sizeof(u32));
    }


    struct buffer_t
    {
        VkBuffer handle;
        memory::allocation_t allocation;
        u32 used;
        VkBufferUsageFlags usage;
        u16 descriptor_index;
    };


    // ==================================================
    // image
    // ==================================================
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


    // ==================================================
    // image sampler
    // ==================================================
    namespace image_sampler
    {
        struct setup_t
        {
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


    // ==================================================
    // shader
    // ==================================================
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


    // ==================================================
    // pipeline
    // ==================================================
    namespace pipeline
    {
        constexpr u32 compute_shader_module_idx = 0;
        constexpr u32 vertex_shader_module_idx = 0;
        constexpr u32 fragment_shader_module_idx = 0;

        struct setup_t
        {
            u16 id;
            b8 is_compute;
            b8 primitive_restart_enabled;

            union
            {
                struct
                {
                    u16 compute_shader_id;
                    u16 _pad1;
                };

                struct
                {
                    u16 vertex_shader_id;
                    u16 fragment_shader_id;
                };
            };

            VkPrimitiveTopology primitive_topology;
            VkPolygonMode polygon_mode;
            VkCullModeFlags cull_mode;
            VkFrontFace front_face;
            b8 depth_test_enabled;
            b8 depth_write_enabled;
            u8 layout_idx;
            u8 _pad0;
        };

        static_assert(offsetof(setup_t, id) == 0);
        static_assert(sizeof(setup_t) % 4 == 0);
        static_assert(sizeof(setup_t::primitive_topology) == sizeof(u32));
        static_assert(sizeof(setup_t::polygon_mode) == sizeof(u32));
        static_assert(sizeof(setup_t::cull_mode) == sizeof(u32));
        static_assert(sizeof(setup_t::front_face) == sizeof(u32));
    }


    struct pipeline_t
    {
        VkPipeline handle;
        VkPipelineLayout layout;
    };


    // ==================================================
    // material
    // ==================================================
    namespace material
    {
        struct setup_t
        {
            u16 id;
            u16 _pad0;
            u32 color;
        };

        static_assert(offsetof(setup_t, id) == 0);
        static_assert(sizeof(setup_t) % 4 == 0);
    }


    struct material_t
    {
        u32 color;
    };
}


// ==================================================
// material functions
// ==================================================
namespace resource::material
{
    material_t *create(const setup_t *setup);
    void destroy(material_t *material);
    material_t *find(u16 id);
}


// ==================================================
// buffer functions
// ==================================================
namespace resource::buffer
{
    buffer_t *create(const setup_t *setup);
    void destroy(buffer_t *buffer);
    buffer_t *find(u16 id);
}


// ==================================================
// image functions
// ==================================================
namespace resource::image
{
    image_t *create(const setup_t *setup);
    void destroy(image_t *image);
    image_t *find(u16 id);
}


// ==================================================
// image sampler functions
// ==================================================
namespace resource::image_sampler
{
    image_sampler_t *create(const setup_t *setup);
    void destroy(image_sampler_t *image_sampler);
    image_sampler_t *find(u16 id);
}


// ==================================================
// shader functions
// ==================================================
namespace resource::shader
{
    shader_t *create(const setup_t *setup, const u32 *spirv_code);
    void destroy(shader_t *shader);
    shader_t *find(u16 id);
    VkShaderModule find_module(u16 id);
}


// ==================================================
// pipeline functions
// ==================================================
namespace resource::pipeline
{
    pipeline_t *create(const setup_t *setup, const VkShaderModule *shader_module_array);
    void destroy(pipeline_t *pipeline);
    pipeline_t *find(u16 id);
}


// ==================================================
// resource variables
// ==================================================
namespace resource
{
    inline memory::resource_pool_id16_t buffers = {};
    inline memory::resource_pool_id16_t images = {};
    inline memory::resource_pool_id16_t image_samplers = {};
    inline memory::resource_pool_id16_t shaders = {};
    inline memory::resource_pool_id16_t pipelines = {};
    inline memory::resource_pool_id16_t materials = {};
    inline descriptor_set_layout_t descriptor_set_layouts = {};
    inline pipeline_layout_t pipeline_layouts = {};
}
