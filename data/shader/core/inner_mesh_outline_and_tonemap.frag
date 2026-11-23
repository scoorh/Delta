#version 450

#define frame_data_descriptor
#define image_data_descriptor
#define mesh_data_descriptor
#include "types.glsl"


frame_uniform(0, 0, readonly)
material_ssbo(1, 3, readonly)
image_array(2, 0)
uimage_array(2, 0)
sampler_array(2, 1)


layout(push_constant) uniform PushConstant
{
    uint color_buffer_image_idx;
    uint color_buffer_sampler_idx;
    uint normal_buffer_image_idx;
    uint normal_buffer_sampler_idx;
    uint depth_buffer_image_idx;
    uint depth_buffer_sampler_idx;
    uint material_buffer_image_idx;
    uint material_buffer_sampler_idx;
    float exposure;
    float normal_threshold;
    float depth_threshold;
    float outline_strength;
};


layout(location = 0) out vec4 out_color_buffer;


const uint invalid_material_idx = 0xffffu;

vec3 fetch_hdr_color(ivec2 pixel);
vec3 fetch_normal(ivec2 pixel);
uint fetch_material_idx(ivec2 pixel);
float fetch_depth(ivec2 pixel);

bool owns_material_edge(uint center_idx, uint neighbor_idx);

float detect_material_edge(ivec2 pixel);


void main()
{
    ivec2 pixel = ivec2(gl_FragCoord.xy);

    float material_edge = detect_material_edge(pixel);

    vec3 hdr_color = fetch_hdr_color(pixel);
    vec3 color = mix(hdr_color, vec3(0.0, 0.0, 0.0), material_edge);

    out_color_buffer = vec4(color, 1.0);
}


uint fetch_material_idx(ivec2 pixel)
{
    return texelFetch(
        usampler2D(
            uimages[material_buffer_image_idx],
            samplers[material_buffer_sampler_idx]
        ),
        pixel,
        0
    ).r;
}


vec3 fetch_hdr_color(ivec2 pixel)
{
    return texelFetch(
        sampler2D(
            images[color_buffer_image_idx],
            samplers[color_buffer_sampler_idx]
        ),
        pixel,
        0
    ).rgb;
}


float fetch_depth(ivec2 pixel)
{
    return texelFetch(
        sampler2D(
            images[depth_buffer_image_idx],
            samplers[depth_buffer_sampler_idx]
        ),
        pixel,
        0
    ).r;
}


vec3 fetch_normal(ivec2 pixel)
{
    vec2 encoded = texelFetch(
        sampler2D(
            images[normal_buffer_image_idx],
            samplers[normal_buffer_sampler_idx]
        ),
        pixel,
        0
    ).rg;

    return decode_octahedral_normal(encoded);
}


bool owns_material_edge(uint center_idx, uint neighbor_idx)
{
    if (center_idx == invalid_material_idx || neighbor_idx == invalid_material_idx)
        return false;

    if (center_idx == neighbor_idx)
        return false;

    if (outline_group(center_idx) == outline_group(neighbor_idx))
        return false;

    if (outline_priority(center_idx) != outline_priority(neighbor_idx))
        return outline_priority(center_idx) > outline_priority(neighbor_idx);

    return center_idx > neighbor_idx;
}


float detect_material_edge(ivec2 pixel)
{
    uint center_material_idx = fetch_material_idx(pixel);
    if (center_material_idx == invalid_material_idx)
        return 0.0;

    const ivec2 offsets[4] = ivec2[4](
        ivec2(-1,  0),
        ivec2( 1,  0),
        ivec2( 0, -1),
        ivec2( 0,  1)
    );

    ivec2 texture_size = ivec2(viewport_width, viewport_height);

    float edge = 0.0;

    for (int i = 0; i < 4; ++i)
    {
        ivec2 neighbor_pixel = clamp_pixel(pixel + offsets[i], texture_size);
        uint neighbor_material_idx = fetch_material_idx(neighbor_pixel);

        if (owns_material_edge(center_material_idx, neighbor_material_idx))
        {
            edge = 1.0;
            break;
        }
    }
    return edge;
}