#pragma once


// ==================================================
// resource types
// ==================================================
namespace resource
{
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
        }


        // ==================================================
        // ui data
        // ==================================================
        namespace ui_data
        {
            constexpr u32 instances_binding_idx = 0;
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
