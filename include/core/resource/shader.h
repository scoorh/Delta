#pragma once


// ==================================================
// resource types
// ==================================================
namespace resource
{
    // ==================================================
    // shader types
    // ==================================================
    namespace shader
    {
        // ==================================================
        // frame uniform
        // ==================================================
        struct frame_uniform_t
        {
            math::matrix4x4_t view;
            math::matrix4x4_t projection;
            math::matrix4x4_t view_projection;

            union
            {
                math::vector4_t packed0;

                struct
                {
                    struct
                    {
                        f32 width;
                        f32 height;
                    } viewport_size;

                    struct
                    {
                        f32 width;
                        f32 height;
                    } ndc_scale;
                };
            };
        };

        static_assert(sizeof(frame_uniform_t) % 16 == 0);


        // ==================================================
        // material
        // ==================================================
        struct material_t
        {
            u32 color;
            u32 _pad0;
            u32 _pad1;
            u32 _pad2;
        };

        static_assert(sizeof(material_t) % 16 == 0);


        // ==================================================
        // mesh instance
        // ==================================================
        struct mesh_instance_t
        {
            union
            {
                math::vector4_t packed0;

                struct
                {
                    f32 world_matrix_c0_x;
                    f32 world_matrix_c1_x;
                    f32 world_matrix_c2_x;
                    f32 world_matrix_c3_x;
                };
            };

            union
            {
                math::vector4_t packed1;

                struct
                {
                    f32 world_matrix_c0_y;
                    f32 world_matrix_c1_y;
                    f32 world_matrix_c2_y;
                    f32 world_matrix_c3_y;
                };
            };

            union
            {
                math::vector4_t packed2;

                struct
                {
                    f32 world_matrix_c0_z;
                    f32 world_matrix_c1_z;
                    f32 world_matrix_c2_z;
                    f32 world_matrix_c3_z;
                };
            };

            union
            {
                u32 packed3;

                struct
                {
                    u16 position_vertex_ssbo_idx;
                    u16 attribute_vertex_ssbo_idx;
                };
            };

            union
            {
                u32 packed4;

                struct
                {
                    u16 material_idx;
                    u16 interaction_id;
                };
            };

            u32 _pad2;
            u32 _pad3;
        };

        static_assert(sizeof(mesh_instance_t) % 16 == 0);


        // ==================================================
        // ui instance
        // ==================================================
        struct ui_instance_t
        {
            union
            {
                math::vector4_t packed0;

                struct
                {
                    f32 position_x;
                    f32 position_y;
                    f32 width;
                    f32 height;
                };
            };

            union
            {
                u32 packed1;

                struct
                {
                    u16 interaction_id;
                    u16 _pad0;
                };
            };

            u32 _pad1;
            u32 _pad3;
            u32 _pad2;
        };

        static_assert(sizeof(ui_instance_t) % 16 == 0);


        // ==================================================
        // mesh indirect draw cmd
        // ==================================================
        struct mesh_indirect_draw_cmd_t
        {
            u32 index_count;
            u32 instance_count;
            u32 first_index;
            u32 vertex_offset;
            u32 first_instance;
        };

        static_assert(sizeof(mesh_indirect_draw_cmd_t) % 4 == 0);


        // ==================================================
        // mesh indirect draw cmd data
        // ==================================================
        struct mesh_indirect_draw_cmd_data_t
        {
            u32 initial_instance_count;
        };

        static_assert(sizeof(mesh_indirect_draw_cmd_data_t) % 4 == 0);


        // ==================================================
        // mesh instance index
        // ==================================================
        struct mesh_instance_index_t
        {
            u32 index;
        };

        static_assert(sizeof(mesh_instance_index_t) % 4 == 0);
    }


    // ==================================================
    // descriptor set layout
    // ==================================================
    namespace descriptor_set_layout
    {
        constexpr u32 count = 5;


        // ==================================================
        // frame data
        // ==================================================
        namespace frame_data
        {
            constexpr u32 idx = 0;

            constexpr u32 uniform_binding_idx = 0;

            constexpr u32 uniform_buffer_count = 1;
            constexpr u32 sampler_count = 0;
            constexpr u32 sampled_image_count = 0;
            constexpr u32 storage_buffer_count = 0;
        }


        // ==================================================
        // mesh cull data
        // ==================================================
        namespace mesh_cull_data
        {
            constexpr u32 idx = 1;

            constexpr u32 instance_indices_binding_idx = 0;
            constexpr u32 indirect_draw_cmd_data_binding_idx = 1;
            constexpr u32 indirect_draw_cmd_binding_idx = 2;
            constexpr u32 instances_binding_idx = 3;

            constexpr u32 uniform_buffer_count = 0;
            constexpr u32 sampler_count = 0;
            constexpr u32 sampled_image_count = 0;
            constexpr u32 storage_buffer_count = 4;
        }


        // ==================================================
        // mesh data
        // ==================================================
        namespace mesh_data
        {
            constexpr u32 idx = 2;

            constexpr u32 vertex_ssbo_array_binding_idx = 0;
            constexpr u32 instance_indices_binding_idx = 1;
            constexpr u32 instances_binding_idx = 2;
            constexpr u32 materials_binding_idx = 3;

            constexpr u32 vertex_ssbo_array_binding_size = 1024;

            constexpr u32 uniform_buffer_count = 0;
            constexpr u32 sampler_count = 0;
            constexpr u32 sampled_image_count = 0;
            constexpr u32 storage_buffer_count = 3 + vertex_ssbo_array_binding_size;
        }


        // ==================================================
        // image data
        // ==================================================
        namespace image_data
        {
            constexpr u32 idx = 3;

            constexpr u32 sampled_images_binding_idx = 0;
            constexpr u32 samplers_binding_idx = 1;

            constexpr u32 sampled_images_binding_size = 1024;
            constexpr u32 samplers_binding_size = 128;

            constexpr u32 uniform_buffer_count = 0;
            constexpr u32 sampler_count = samplers_binding_size;
            constexpr u32 sampled_image_count = sampled_images_binding_size;
            constexpr u32 storage_buffer_count = 0;
        }


        // ==================================================
        // ui data
        // ==================================================
        namespace ui_data
        {
            constexpr u32 instances_binding_idx = 0;

            constexpr u32 uniform_buffer_count = 0;
            constexpr u32 sampler_count = 0;
            constexpr u32 sampled_image_count = 0;
            constexpr u32 storage_buffer_count = 1;
        }
    }


    union descriptor_set_layout_t
    {
        struct
        {
            VkDescriptorSetLayout frame_data;
            VkDescriptorSetLayout mesh_data;
            VkDescriptorSetLayout mesh_cull_data;
            VkDescriptorSetLayout ui_data;
            VkDescriptorSetLayout image_data;
        };

        VkDescriptorSetLayout array[descriptor_set_layout::count];
    };


    // ==================================================
    // pipeline layout
    // ==================================================
    namespace pipeline_layout
    {
        constexpr u32 count = 3;


        // ==================================================
        // draw ui
        // ==================================================
        namespace draw_ui
        {
            constexpr u32 idx = 0;

            constexpr u32 descriptor_count = 3;

            constexpr u32 frame_data_descriptor_idx = 0;
            constexpr u32 ui_data_descriptor_idx = 1;
            constexpr u32 image_data_descriptor_idx = 2;
        }


        // ==================================================
        // cull mesh
        // ==================================================
        namespace cull_mesh
        {
            constexpr u32 idx = 1;

            constexpr u32 descriptor_count = 2;

            constexpr u32 frame_data_descriptor_idx = 0;
            constexpr u32 mesh_cull_data_descriptor_idx = 1;

            struct push_constant_t
            {
                u32 instance_count;
                u32 indirect_draw_cmd_count;
            };
        }


        // ==================================================
        // draw mesh
        // ==================================================
        namespace draw_mesh
        {
            constexpr u32 idx = 2;

            constexpr u32 descriptor_count = 3;

            constexpr u32 frame_data_descriptor_idx = 0;
            constexpr u32 mesh_data_descriptor_idx = 1;
            constexpr u32 image_data_descriptor_idx = 2;
        }
    }


    union pipeline_layout_t
    {
        struct
        {
            VkPipelineLayout draw_ui;
            VkPipelineLayout cull_mesh;
            VkPipelineLayout draw_mesh;
        };

        VkPipelineLayout array[pipeline_layout::count];
    };
}


// ==================================================
// descriptor set layout functions
// ==================================================
namespace resource::descriptor_set_layout
{
    void initialize();
}


// ==================================================
// pipeline layout functions
// ==================================================
namespace resource::pipeline_layout
{
    void initialize();
}
