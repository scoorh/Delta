#version 450

#define frame_data_descriptor
#define image_data_descriptor
#include "types.glsl"


frame_uniform(0, 0, readonly)
image_array(1, 0)
sampler_array(1, 1)


layout(push_constant) uniform PushConstant
{
    uint color_buffer_image_idx;
    uint color_buffer_sampler_idx;
    uint normal_buffer_image_idx;
    uint normal_buffer_sampler_idx;
    uint depth_buffer_image_idx;
    uint depth_buffer_sampler_idx;
    float exposure;
    float normal_threshold;
    float normal_softness;
    float depth_threshold;
    float depth_softness;
    float outline_strength;
};


layout(location = 0) out vec4 out_color_buffer;


void main()
{
    ivec2 pixel = ivec2(gl_FragCoord.xy);

    vec3 hdr_color = texelFetch(
        sampler2D(
            images[color_buffer_image_idx],
            samplers[color_buffer_sampler_idx]
        ),
        pixel,
        0
    ).rgb;

    float edge = 1.0;


    vec3 color = hdr_color;
    //vec3 color = aces_tonemap(hdr_color * exposure);

    //color *= 1.0 - clamp(edge * outline_strength, 0.0, 1.0);

    out_color_buffer = vec4(color, 1.0);
}