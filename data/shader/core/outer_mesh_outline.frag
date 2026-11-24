#version 450

#define frame_data_descriptor
#define mesh_data_descriptor
#include "types.glsl"


frame_uniform(0, 0, readonly)
material_ssbo(1, 3, readonly)


layout(location = 0) in flat uint in_material_idx;


layout(location = 0) out vec4 out_color_buffer;
layout(location = 1) out uint out_material_buffer;


void main()
{
    vec4 color = vec4(0.0, 0.0, 0.0, 1.0);

    out_color_buffer = color;
    out_material_buffer = invalid_material_idx;
}