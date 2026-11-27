vec2 encode_octahedral_normal(vec3 normal)
{
    normal /= abs(normal.x) + abs(normal.y) + abs(normal.z);
    vec2 p = normal.xy;
    vec2 folded = (1.0 - abs(p.yx)) * mix(vec2(-1.0), vec2(1.0), greaterThanEqual(p, vec2(0.0)));
    return mix(p, folded, float(normal.z < 0.0));
}


vec3 decode_octahedral_normal(vec2 encoded)
{
    vec3 n = vec3(encoded, 1.0 - abs(encoded.x) - abs(encoded.y));
    float t = max(-n.z, 0.0);
    n.xy += mix(vec2(t), vec2(-t), greaterThanEqual(n.xy, vec2(0.0)));
    return normalize(n);
}


float inverse_lerp(float v, float a, float b)
{
    return (v - a) / (b - a);
}


vec3 aces_tonemap(vec3 color)
{
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;

    return clamp(
        (color * (a * color + b)) /
        (color * (c * color + d) + e),
        0.0,
        1.0
    );
}


ivec2 clamp_pixel(ivec2 p, ivec2 s)
{
    return clamp(p, ivec2(0), s - ivec2(1));
}


#ifdef frame_data_descriptor
bool is_background(float depth)
{
    return depth == 0.0;
}


vec2 pixel_to_ndc(vec2 pixel)
{
    return pixel * ndc_scale - 1.0;
}


float linearize_depth(float depth)
{
    return camera_near_z / depth;
}


vec3 reconstruct_view_position(vec2 pixel, float depth)
{
    vec2 ndc = pixel_to_ndc(pixel);

    vec3 ray = vec3(
        ndc.x * inverse_projection_matrix[0][0],
        ndc.y * inverse_projection_matrix[1][1],
        1.0
    );

    return ray * linearize_depth(depth);
}
#endif


float distribution_ggx(float NoH, float alpha)
{
    float alpha2 = alpha * alpha;
    float NoH2 = NoH * NoH;
    float d = (1.0 - NoH2) + NoH2 * alpha2;
    return inverse_pi * alpha2 / (d * d);
}


vec3 fresnel_schlick(float VoH, vec3 F0)
{
    float f = 1.0 - VoH;
    float f2 = f * f;
    float f5 = f2 * f2 * f;
    return F0 = (1.0 - F0) * f5;
}


float visibility_smith_ggx(float NoV, float NoL, float roughness)
{
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float GGXV = NoL * sqrt(NoV * NoV * (1.0 - alpha2) + alpha2);
    float GGXL = NoV * sqrt(NoL * NoL * (1.0 - alpha2) + alpha2);
    return 0.5 / max(GGXV + GGXL, 1e-6);
}


float visibility_smith_ggx_fast(float NoV, float NoL, float alpha)
{
    float v = 2.0 * NoV * NoL * (1.0 - alpha) + alpha * (NoV + NoL);
    return 0.5 / max(v, 1e-6);
}


struct surface_t
{
    vec3 position;
    vec3 normal;
    vec3 view_direction;
    vec3 albedo;
    float metallic;
    float roughness;
    float ao;
    float specular;

    vec3 F0;
    vec3 diffuse_color;
    float alpha;
    float alpha2;
    float NoV;
    float specular_mul;
    vec2 visibility_mad;
};


float distribution_visibility_ggx(surface_t s, float NoH, float NoL)
{
    float NoH2 = NoH * NoH;
    float d = (1.0 - NoH2) + NoH2 * s.alpha2;
    float v = max(fma(NoL, s.visibility_mad.x, s.visibility_mad.y), 1e-6);
    return s.specular_mul / (d * d * v);
}


vec3 evaluate_brdf(surface_t s, vec3 L, vec3 radiance)
{
    float NoL = clamp(dot(s.normal, L), 0.0, 1.0);
    if (NoL <= 0.0 || s.NoV <= 0.0)
        return vec3(0.0);

    float VoL = clamp(dot(s.view_direction, L), -1.0, 1.0);
    float inv_len = inversesqrt(max(2.0 + 2.0 * VoL, 1e-6));
    float NoH = clamp((s.NoV + NoL) * inv_len, 0.0, 1.0);
    float VoH = clamp((1.0 + VoL) * inv_len, 0.0, 1.0);
    float DVis = distribution_visibility_ggx(s, NoH, NoL);

    vec3 F = fresnel_schlick(VoH, s.F0);
    vec3 Fr = DVis * F;
    vec3 Fd = (1.0 - F) * s.diffuse_color;

    return (Fd + Fr) * radiance * NoL;
}