#pragma once


#include <core/types.h>


// ==================================================
// resource package
// ==================================================
namespace metadata::resource_package
{
    constexpr u32 default_cull_mesh_cs_id = 1;
    constexpr u32 default_draw_mesh_vs_id = 2;
    constexpr u32 default_draw_mesh_fs_id = 3;
    constexpr u32 default_draw_ui_vs_id = 4;
    constexpr u32 default_draw_ui_fs_id = 5;

    constexpr u32 default_cull_mesh_pipeline_id = 1;
    constexpr u32 default_draw_mesh_pipeline_id = 2;
    constexpr u32 default_draw_ui_pipeline_id = 3;

    constexpr u32 character_index_buffer_id = 1;
    constexpr u32 character_position_vertex_buffer_id = 2;
    constexpr u32 character_attribute_vertex_buffer_id = 3;

    constexpr u32 default_character_body_mesh_id = 1;

    constexpr u32 default_character_body_material_id = 1;
}