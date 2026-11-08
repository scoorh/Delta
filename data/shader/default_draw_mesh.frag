#version 450

#define mesh_data_descriptor
#include "default_types.glsl"

mesh_data_material_ssbo(1, 3, readonly)

layout(location = 0) in flat uint in_material_idx;
layout(location = 1) in flat uint in_interaction_id;

layout(location = 0) out vec4 out_color_buffer;
layout(location = 1) out uint out_interaction_buffer;

void main() {
    vec4 color = color_vec4(in_material_idx);

    out_color_buffer = color;
    out_interaction_buffer = in_interaction_id;
}