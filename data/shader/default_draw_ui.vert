#version 450

#define frame_data_descriptor
#define ui_data_descriptor
#include "default_types.glsl"

frame_uniform(0, 0, readonly)
ui_instance_ssbo(1, 0, readonly)

layout(location = 0) out uint out_material_idx;
layout(location = 1) out uint out_interaction_id;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main()
{
    vec2 ndc =
            (position + size *
            (vec2(gl_VertexIndex & 1, (gl_VertexIndex >> 1) & 1) - 0.5))
            * ndc_scale;

    out_interaction_id = interaction_id;

    gl_Position = vec4(ndc.x - 1.0, ndc.y - 1.0, 0.5, 1.0);
}