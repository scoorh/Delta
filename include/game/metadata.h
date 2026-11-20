#pragma once


#include <core/types.h>


// ==================================================
// resource package
// ==================================================
namespace metadata::resource_package
{
    // ==================================================
    // shader
    // ==================================================
    constexpr u32 cull_mesh_cs_id = 1;
    constexpr u32 mesh_outline_fs_id = 2;
    constexpr u32 mesh_outline_vs_id = 3;
    constexpr u32 default_draw_mesh_vs_id = 200;
    constexpr u32 default_draw_mesh_fs_id = 201;
    constexpr u32 default_draw_ui_vs_id = 202;
    constexpr u32 default_draw_ui_fs_id = 203;

    // ==================================================
    // pipeline
    // ==================================================
    constexpr u32 cull_mesh_pipeline_id = 1;
    constexpr u32 mesh_outline_pipeline_id = 2;
    constexpr u32 default_draw_mesh_pipeline_id = 200;
    constexpr u32 default_draw_ui_pipeline_id = 201;

    // ==================================================
    // buffer
    // ==================================================
    constexpr u32 character_index_buffer_id = 1;
    constexpr u32 character_position_vertex_buffer_id = 2;
    constexpr u32 character_attribute_vertex_buffer_id = 3;

    // ==================================================
    // mesh
    // ==================================================
    constexpr u32 default_character_body_mesh_id = 1;

    // ==================================================
    // material
    // ==================================================
    constexpr u32 default_character_body_material_id = 1;
}