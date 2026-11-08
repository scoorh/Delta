#version 450

#define frame_data_descriptor
#define mesh_data_descriptor
#include "default_types.glsl"

frame_data_uniform(0, 0, readonly)
mesh_data_vertex_ssbo_array(1, 0, readonly)
mesh_indirect_draw_instance_data_ssbo(1, 1, readonly)
mesh_data_instance_ssbo(1, 2, readonly)

layout(location = 0) out uint out_material_idx;
layout(location = 1) out uint out_interaction_id;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main() {
    uint instance_idx = draw_instance_index(gl_InstanceIndex);
    uint position_ssbo_idx = vertex_position_ssbo_idx(instance_idx);

    vec4 position = local_position_vec4(position_ssbo_idx);
    vec3 world_position = world_position(instance_idx, position);

    gl_Position = view_projection_matrix * vec4(world_position, 1.0);

    out_material_idx = material_idx(instance_idx);
    out_interaction_id = interaction_id(instance_idx);
}