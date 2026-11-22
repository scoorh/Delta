#version 450

#define frame_data_descriptor
#include "types.glsl"


out gl_PerVertex
{
	vec4 gl_Position;
};


void main()
{
    vec2 position = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
    gl_Position = vec4(position * 2.0 - 1.0, 0.0, 1.0);
}