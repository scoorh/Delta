#version 450

#define frame_data_descriptor
#define mesh_data_descriptor
#define image_data_descriptor
#include "types.glsl"

frame_uniform(0, 0, readonly)
material_ssbo(1, 3, readonly)

#include "functions.glsl"


layout(location = 0) flat in uint in_material_idx;
layout(location = 1) flat in uint in_interaction_id;
layout(location = 2) in vec3 in_world_position;
layout(location = 3) in vec3 in_world_normal;
layout(location = 4) in vec3 in_view_normal;


layout(location = 0) out vec4 out_color_attachment;
layout(location = 1) out vec4 out_mraos_attachment;
layout(location = 2) out uint out_material_attachment;
layout(location = 3) out vec2 out_normal_attachment;
layout(location = 4) out uint out_interaction_attachment;


void main()
{
    out_color_attachment = vec4(color_vec4(in_material_idx).rgb, 1.0);
    out_mraos_attachment = vec4(0.0, 0.0, 0.0, 0.5);
    out_material_attachment = in_material_idx;
    out_normal_attachment = encode_octahedral_normal(normalize(in_view_normal));
    out_interaction_attachment = in_interaction_id;
}