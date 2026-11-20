#version 450

#define frame_data_descriptor
#define mesh_data_descriptor
#include "types.glsl"

frame_uniform(0, 0, readonly)
mesh_vertex_ssbo_array(1, 0, readonly)
mesh_instance_index_ssbo(1, 1, readonly)
mesh_instance_ssbo(1, 2, readonly)

layout(location = 0) out uint out_material_idx;
layout(location = 1) out uint out_interaction_id;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main()
{
    uint instance_idx = draw_instance_index(gl_InstanceIndex);
    uint position_ssbo_idx = vertex_position_ssbo_idx(instance_idx);
    uint attribute_ssbo_idx = vertex_attribute_ssbo_idx(instance_idx);

    vec4 position = local_position_vec4(position_ssbo_idx);
    vec3 normal = local_normal_vec3(attribute_ssbo_idx);

    vec3 world_position = world_position(instance_idx, position);
    vec3 world_normal = world_normal(instance_idx, normal);

    vec4 view_position = view_matrix * vec4(world_position, 1.0);
    vec3 view_normal = mat3(view_matrix) * world_normal;

    vec4 clip_position = projection_matrix * view_position;

    vec2 ndc_direction = vec2(
        projection_matrix[0][0] * (view_normal.x * view_position.z - view_position.x * view_normal.z),
        projection_matrix[1][1] * (view_normal.y * view_position.z - view_position.y * view_normal.z)
    );

    vec2 pixel_direction = ndc_direction * viewport_size;

    float len_sq = dot(pixel_direction, pixel_direction);
    pixel_direction *= inversesqrt(max(len_sq, 1e-10));

    vec2 ndc_offset = pixel_direction * outline_pixel_size(instance_idx) * ndc_scale;

    clip_position.xy += ndc_offset * clip_position.w;

    gl_Position = clip_position;

    out_material_idx = material_idx(instance_idx);
    out_interaction_id = interaction_id(instance_idx);
}