#version 450

#define frame_data_descriptor
#define mesh_data_descriptor
#define image_data_descriptor
#include "types.glsl"


frame_uniform(0, 0, readonly)
material_ssbo(1, 3, readonly)


layout(location = 0) flat in uint in_material_idx;
layout(location = 1) flat in uint in_interaction_id;
layout(location = 2) in vec3 in_world_position;
layout(location = 3) in vec3 in_world_normal;
layout(location = 4) in vec3 in_view_normal;


layout(location = 0) out vec4 out_color_buffer;
layout(location = 1) out vec4 out_mrao_buffer;
layout(location = 2) out uint out_material_buffer;
layout(location = 3) out vec2 out_normal_buffer;
layout(location = 4) out uint out_interaction_buffer;


void main()
{
    out_color_buffer = vec4(color_vec4(in_material_idx).rgb, 1.0);
    out_mrao_buffer = vec4(0.0, 0.0, 0.0, 0.0);
    out_material_buffer = in_material_idx;
    out_normal_buffer = encode_octahedral_normal(normalize(in_view_normal));
    out_interaction_buffer = in_interaction_id;
}