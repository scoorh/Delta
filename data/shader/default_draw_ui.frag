#version 450

#include "default_types.glsl"

layout(location = 0) in flat uint in_material_idx;
layout(location = 1) in flat uint in_interaction_id;

layout(location = 0) out vec4 out_color_buffer;
layout(location = 1) out uint out_interaction_buffer;

void main()
{
    out_color_buffer = vec4(1.0, 0.0, 0.0, 1.0);
    out_interaction_buffer = in_interaction_id;
}