#version 450

#define frame_data_descriptor
#define mesh_data_descriptor
#include "types.glsl"


frame_uniform(0, 0, readonly)
material_ssbo(1, 3, readonly)


layout(location = 0) in flat uint in_material_idx;


layout(location = 0) out vec4 out_color_attachment;
layout(location = 1) out vec4 out_mraos_attachment;
layout(location = 2) out uint out_material_attachment;
layout(location = 3) out vec2 out_normal_attachment;
layout(location = 4) out uint out_interaction_attachment;


void main()
{
    vec4 color = vec4(0.0, 0.0, 0.0, 1.0);

    out_color_attachment = color;
    out_material_attachment = invalid_material_idx;
}