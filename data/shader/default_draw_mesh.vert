#version 450

#define frame_data_descriptor
#define mesh_data_descriptor
#include "default_types.glsl"

frame_uniform(0, 0, readonly)
mesh_vertex_ssbo_array(1, 0, readonly)
mesh_instance_index_ssbo(1, 1, readonly)
mesh_instance_ssbo(1, 2, readonly)

layout(location = 0) out uint out_material_idx;
layout(location = 1) out uint out_interaction_id;
layout(location = 2) out vec3 out_world_position;
layout(location = 3) out vec3 out_world_normal;

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

    gl_Position = view_projection_matrix * vec4(world_position, 1.0);

    out_world_position = world_position;
    out_world_normal = world_normal;
    out_material_idx = material_idx(instance_idx);
    out_interaction_id = interaction_id(instance_idx);
}