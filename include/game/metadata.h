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
    constexpr u32 default_draw_ui_vs_id = 202;
    constexpr u32 default_draw_ui_fs_id = 203;

    // ==================================================
    // pipeline
    // ==================================================
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
    constexpr u32 clothes_mesh_id = 2;
    constexpr u32 hair_mesh_id = 3;

    // ==================================================
    // material
    // ==================================================
    constexpr u32 default_character_body_material_id = 1;
    constexpr u32 default_character_clothes_material_id = 2;
    constexpr u32 default_character_hair_material_id = 3;
}