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
    float hdr_exposure;
};


layout(location = 0) out vec4 out_color_buffer;


vec2 outline_line_filter(ivec2 center_pixel, ivec2 image_size, uint center_material_idx);


void main()
{
    ivec2 pixel = ivec2(gl_FragCoord.xy);
    ivec2 image_size = ivec2(viewport_width, viewport_height);

    uint material_idx = texel_fetch_u(material_buffer_image_idx, pixel).r;


    vec3 hdr_color = texel_fetch(color_buffer_image_idx, pixel).rgb;
    hdr_color *= exp2(hdr_exposure);
    vec3 ldr_color = aces_tonemap(hdr_color);

    vec3 out_color = ldr_color;

    if (material_idx != invalid_material_idx)
    {
        vec2 outline_line = outline_line_filter(pixel, image_size, material_idx);
        out_color = mix(ldr_color, outline_color_vec4(material_idx).rgb, max(outline_line.x, outline_line.y));
    }

    out_color_buffer = vec4(out_color, 1.0);
}


bool owns_material_edge(uint center_group, uint center_priority, uint neighbor_material_idx)
{
    if (neighbor_material_idx == invalid_material_idx)
        return true;

    uint neighbor_group = outline_group(neighbor_material_idx);
    uint neighbor_priority = outline_priority(neighbor_material_idx);

    return
        center_group != neighbor_group &&
        (
            center_priority > neighbor_priority ||
            (
                center_priority == neighbor_priority &&
                center_group > neighbor_group
            )
        );
}


vec2 outline_line_filter(ivec2 center_pixel, ivec2 image_size, uint center_material_idx)
{
    ivec2 lo = max(center_pixel - 1, ivec2(0));
    ivec2 hi = min(center_pixel + 1, image_size);


    float depth_line = 0.0;
    float depth_discontinuity = 0.0;
    float material_line = 0.0;


    if (outline_depth_threshold_sq(center_material_idx).x > 0.0)
    {
        float c = texel_fetch(depth_buffer_image_idx, center_pixel).r;

        float l = texel_fetch(depth_buffer_image_idx, ivec2(lo.x, center_pixel.y)).r;
        float r = texel_fetch(depth_buffer_image_idx, ivec2(hi.x, center_pixel.y)).r;
        float t = texel_fetch(depth_buffer_image_idx, ivec2(center_pixel.x, lo.y)).r;
        float b = texel_fetch(depth_buffer_image_idx, ivec2(center_pixel.x, hi.y)).r;

        float tl = texel_fetch(depth_buffer_image_idx, ivec2(lo.x, lo.y)).r;
        float tr = texel_fetch(depth_buffer_image_idx, ivec2(hi.x, lo.y)).r;
        float bl = texel_fetch(depth_buffer_image_idx, ivec2(lo.x, hi.y)).r;
        float br = texel_fetch(depth_buffer_image_idx, ivec2(hi.x, hi.y)).r;

        float axial = (l - c) + (r - c) + (t - c) + (b - c);
        float diagonal = (tl - c) + (tr - c) + (bl - c) + (br - c);
        float lap = fma(axial, 4.0, diagonal);

        float depth_edge = -lap / max(c, camera_near_z * 1e-6);
        float foreground_edge = max(depth_edge, 0.0);

        foreground_edge *= foreground_edge;
        depth_edge *= depth_edge;

        vec2 threshold_sq = outline_depth_threshold_sq(center_material_idx);

        depth_line = smoothstep(threshold_sq.x, threshold_sq.y, foreground_edge);
        depth_discontinuity = smoothstep(threshold_sq.x, threshold_sq.y, depth_edge);
    }


    if (outline_group(center_material_idx) != 0)
    {
        uint ml = texel_fetch_u(material_buffer_image_idx, ivec2(lo.x, center_pixel.y)).r;
        uint mr = texel_fetch_u(material_buffer_image_idx, ivec2(hi.x, center_pixel.y)).r;
        uint mt = texel_fetch_u(material_buffer_image_idx, ivec2(center_pixel.x, lo.y)).r;
        uint mb = texel_fetch_u(material_buffer_image_idx, ivec2(center_pixel.x, hi.y)).r;
        uint mdiff = (ml ^ center_material_idx) | (mr ^ center_material_idx) |  (mt ^ center_material_idx) | (mb ^ center_material_idx);
        bool material_edge = mdiff != 0u;

        if (material_edge)
        {
            uint center_group = outline_group(center_material_idx);
            uint center_priority = outline_priority(center_material_idx);

            material_edge =
                    owns_material_edge(center_group, center_priority, ml) ||
                    owns_material_edge(center_group, center_priority, mr) ||
                    owns_material_edge(center_group, center_priority, mt) ||
                    owns_material_edge(center_group, center_priority, mb);
        }

        material_line = float(material_edge) * (1.0 - depth_discontinuity);
    }


    return vec2(depth_line, material_line);
}