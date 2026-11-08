#pragma once


// ==================================================
// settings
// ==================================================
namespace settings
{
    // ==================================================
    // application settings
    // ==================================================
    namespace memory
    {
        inline u32 temporary_block_1_size = 0;
        inline u32 temporary_block_2_size = 0;
        inline u32 core_block_size = 0;

        inline u32 buffer_resource_pool_size = 0;
        inline u32 image_resource_pool_size = 0;
        inline u32 image_sampler_resource_pool_size = 0;
        inline u32 shader_resource_pool_size = 0;
        inline u32 pipeline_resource_pool_size = 0;
        inline u32 material_resource_pool_size = 0;

        inline u32 gpu_block_array_size = 0;
        inline u32 free_allocation_array_size = 0;

        inline u32 frame_mesh_indirect_draw_cmd_data_buffer_size = 0;
        inline u32 frame_mesh_indirect_draw_cmd_buffer_size = 0;
        inline u32 frame_mesh_instance_index_buffer_size = 0;
        inline u32 frame_mesh_instance_buffer_size = 0;
        inline u32 frame_material_buffer_size = 0;
        inline u32 frame_ui_instance_buffer_size = 0;
    }


    // ==================================================
    // application settings
    // ==================================================
    namespace application
    {
        inline c16 name_c16[32] = {};
        inline c8 name[32] = {};

        inline union
        {
            u32 packed;

            struct
            {
                u8 major;
                u8 minor;
                u8 patch;
                u8 revision;
            };
        } version = {};
    }


    // ==================================================
    // renderer settings
    // ==================================================
    namespace renderer
    {
        inline u32 device_id = 0;
        inline u32 device_vendor_id = 0;
        inline b8 vsync = false;
    }


    // ==================================================
    // window settings
    // ==================================================
    namespace window
    {
        constexpr u16 default_x_position = 0xffff;
        constexpr u16 default_y_position = 0xffff;

        inline u16 width = 0;
        inline u16 height = 0;
        inline u16 x_position = 0;
        inline u16 y_position = 0;
        inline b8 fullscreen_mode = false;
    }
}


// ==================================================
// settings functions
// ==================================================
namespace settings
{
    void initialize();
}