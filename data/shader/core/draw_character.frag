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
layout(location = 1) out uint out_material_buffer;
layout(location = 2) out vec2 out_normal_buffer;
layout(location = 3) out uint out_interaction_buffer;


void main()
{
    vec3 N = normalize(in_world_normal);
    vec3 L = -(global_light_direction.xyz);
    vec3 V = normalize(camera_position.xyz - in_world_position);

    float NdotL = max(dot(N, L), 0.0);

    float width = max(fwidth(NdotL) * 0.5, shadow_softness(in_material_idx));
    float toon = smoothstep(shadow_threshold(in_material_idx) - width * 0.5, shadow_threshold(in_material_idx) + width, NdotL);
    float brightness = mix(0.55, 1.0, toon);

    vec4 color = color_vec4(in_material_idx);
    vec4 shadow_tint = shadow_tint_vec4(in_material_idx);

    vec4 final_color = mix(color * shadow_tint, color, toon);

    out_color_buffer = vec4(final_color.rgb, 1.0);
    out_material_buffer = in_material_idx;
    out_normal_buffer = encode_octahedral_normal(normalize(in_view_normal));
    out_interaction_buffer = in_interaction_id;
}