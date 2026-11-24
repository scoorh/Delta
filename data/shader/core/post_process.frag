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
};


layout(location = 0) out vec4 out_color_buffer;


bool owns_material_edge(uint center_idx, uint neighbor_idx);
float detect_material_edge(ivec2 pixel, uint center_material_idx);


void main()
{
    ivec2 pixel = ivec2(gl_FragCoord.xy);

    uint material_idx = texel_fetch_u(material_buffer_image_idx, pixel).r;
    float material_edge = detect_material_edge(pixel, material_idx);

    vec3 hdr_color = texel_fetch(color_buffer_image_idx, pixel).rgb;
    vec3 color = mix(hdr_color, outline_color_vec4(material_idx).rgb, material_edge);

    out_color_buffer = vec4(color, 1.0);
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


float detect_material_edge(ivec2 pixel, uint center_material_idx)
{
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
        uint neighbor_material_idx = texel_fetch_u(material_buffer_image_idx, neighbor_pixel).r;

        if (owns_material_edge(center_material_idx, neighbor_material_idx))
        {
            edge = 1.0;
            break;
        }
    }
    return edge;
}