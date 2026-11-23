#ifndef _EXTENSIONS_
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable
#define _EXTENSIONS_
#endif


const float inverse_255 = 1.0 / 255.0;


#ifdef frame_data_descriptor
#define frame_uniform(_set_, _binding_, _access_) \
layout(std140, set = _set_, binding = _binding_) uniform _access_ frame_uniform_t { \
    mat4 view; \
    mat4 projection; \
    mat4 view_projection; \
    vec4 packed0; \
    vec4 global_light_direction_; \
    vec4 camera_position_; \
    vec4 packed1; \
} _frame_uniform;

#define view_matrix _frame_uniform.view
#define projection_matrix _frame_uniform.projection
#define view_projection_matrix _frame_uniform.view_projection
#define viewport_size _frame_uniform.packed0.xy
#define viewport_width _frame_uniform.packed0.x
#define viewport_height _frame_uniform.packed0.y
#define ndc_scale _frame_uniform.packed0.zw
#define ndc_scale_width _frame_uniform.packed0.z
#define ndc_scale_height _frame_uniform.packed0.w
#define global_light_direction _frame_uniform.global_light_direction_
#define camera_position _frame_uniform.camera_position_
#define camera_near_z _frame_uniform.packed1.x
#define camera_far_z _frame_uniform.packed1.y
#endif


#ifdef image_data_descriptor
#define image_array(_set_, _binding_) \
layout(set = _set_, binding = _binding_) uniform texture2D images[];

#define uimage_array(_set_, _binding_) \
layout(set = _set_, binding = _binding_) uniform utexture2D uimages[];

#define sampler_array(_set_, _binding_) \
layout(set = _set_, binding = _binding_) uniform sampler samplers[];
#endif


#ifdef ui_data_descriptor
#define ui_instance_ssbo(_set_, _binding_, _access_) \
struct ui_instance_t { \
    vec4 packed0; \
    uint packed1; \
}; \
layout(set = _set_, binding = _binding_) buffer _access_ ui_instance_ssbo_t { \
    ui_instance_t array[]; \
} _ui_instance_ssbo;

#define position _ui_instance_ssbo.array[gl_InstanceIndex].packed0.xy
#define position_x _ui_instance_ssbo.array[gl_InstanceIndex].packed0.x
#define position_y _ui_instance_ssbo.array[gl_InstanceIndex].packed0.y
#define size _ui_instance_ssbo.array[gl_InstanceIndex].packed0.zw
#define size_x _ui_instance_ssbo.array[gl_InstanceIndex].packed0.z
#define size_y _ui_instance_ssbo.array[gl_InstanceIndex].packed0.w
#define interaction_id (_ui_instance_ssbo.array[gl_InstanceIndex].packed1 & 0xffff)
#endif


#ifdef mesh_data_descriptor
#define mesh_vertex_ssbo_array(_set_, _binding_, _access_) \
layout(set = _set_, binding = _binding_) buffer _access_ mesh_vertex_ssbo_array_t { \
    uint array[]; \
} _mesh_vertex_ssbo_array[];

#define local_position_vec3(ssbo_idx) \
    vec3(uintBitsToFloat(_mesh_vertex_ssbo_array[ssbo_idx].array[gl_VertexIndex * 3]), \
         uintBitsToFloat(_mesh_vertex_ssbo_array[ssbo_idx].array[gl_VertexIndex * 3 + 1]), \
         uintBitsToFloat(_mesh_vertex_ssbo_array[ssbo_idx].array[gl_VertexIndex * 3 + 2]))

#define local_position_vec4(ssbo_idx) \
    vec4(uintBitsToFloat(_mesh_vertex_ssbo_array[ssbo_idx].array[gl_VertexIndex * 3]), \
         uintBitsToFloat(_mesh_vertex_ssbo_array[ssbo_idx].array[gl_VertexIndex * 3 + 1]), \
         uintBitsToFloat(_mesh_vertex_ssbo_array[ssbo_idx].array[gl_VertexIndex * 3 + 2]), \
         1.0)

#define local_normal_vec3(ssbo_idx) \
    vec3(uintBitsToFloat(_mesh_vertex_ssbo_array[ssbo_idx].array[gl_VertexIndex * 5]), \
         uintBitsToFloat(_mesh_vertex_ssbo_array[ssbo_idx].array[gl_VertexIndex * 5 + 1]), \
         uintBitsToFloat(_mesh_vertex_ssbo_array[ssbo_idx].array[gl_VertexIndex * 5 + 2]))
#endif


#ifdef mesh_data_descriptor
#define material_ssbo(_set_, _binding_, _access_) \
struct material_t { \
    uvec2 color; \
    uvec2 shadow_tint; \
    float shadow_threshold; \
    float shadow_softness; \
    uint packed1; \
    uint _pad0; \
}; \
layout(set = _set_, binding = _binding_) buffer _access_ material_ssbo_t { \
    material_t array[]; \
} _material_ssbo;


#define shadow_threshold(material_idx) _material_ssbo.array[material_idx].shadow_threshold
#define shadow_softness(material_idx) _material_ssbo.array[material_idx].shadow_softness
#define outline_group(material_idx) (_material_ssbo.array[material_idx].packed1 & 0x0000ffffu)
#define outline_priority(material_idx) ((_material_ssbo.array[material_idx].packed1 & 0xffff0000u) >> 16)

#define color_vec4(material_idx) \
    vec4(unpackHalf2x16(_material_ssbo.array[material_idx].color.x), \
         unpackHalf2x16(_material_ssbo.array[material_idx].color.y))

 #define shadow_tint_vec4(material_idx) \
     vec4(unpackHalf2x16(_material_ssbo.array[material_idx].shadow_tint.x), \
          unpackHalf2x16(_material_ssbo.array[material_idx].shadow_tint.y))


#endif


#if defined(mesh_data_descriptor) || defined(cull_mesh_data_descriptor)
#define mesh_instance_ssbo(_set_, _binding_, _access_) \
struct mesh_instance_t { \
    vec4 packed0; \
    vec4 packed1; \
    vec4 packed2; \
    uint packed3; \
    uint packed4; \
    uint packed5; \
    uint _pad2; \
}; \
layout(set = _set_, binding = _binding_) buffer _access_ mesh_instance_ssbo_t { \
    mesh_instance_t array[]; \
} _mesh_instance_ssbo;

#define vertex_position_ssbo_idx(instance_idx) (_mesh_instance_ssbo.array[instance_idx].packed3 & 0x0000ffffu)
#define vertex_attribute_ssbo_idx(instance_idx) ((_mesh_instance_ssbo.array[instance_idx].packed3 & 0xffff0000u) >> 16)
#define material_idx(instance_idx) (_mesh_instance_ssbo.array[instance_idx].packed4 & 0x0000ffffu)
#define interaction_id(instance_idx) ((_mesh_instance_ssbo.array[instance_idx].packed4 & 0xffff0000u) >> 16)
#define outline_pixel_size(instance_idx) (_mesh_instance_ssbo.array[instance_idx].packed5 & 0x000000ffu)

#define world_position(instance_idx, position) \
    vec3(dot(_mesh_instance_ssbo.array[instance_idx].packed0, position), \
         dot(_mesh_instance_ssbo.array[instance_idx].packed1, position), \
         dot(_mesh_instance_ssbo.array[instance_idx].packed2, position))

#define world_normal(instance_idx, normal) \
    vec3(dot(_mesh_instance_ssbo.array[instance_idx].packed0.xyz, normal), \
         dot(_mesh_instance_ssbo.array[instance_idx].packed1.xyz, normal), \
         dot(_mesh_instance_ssbo.array[instance_idx].packed2.xyz, normal))

#endif


#if defined(mesh_data_descriptor) || defined(cull_mesh_data_descriptor)
#define mesh_instance_index_ssbo(_set_, _binding_, _access_) \
struct mesh_instance_index_t { \
    uint index; \
}; \
layout(set = _set_, binding = _binding_) buffer _access_ mesh_instance_index_ssbo_t { \
    mesh_instance_index_t array[]; \
} _mesh_instance_index_ssbo;

#define draw_instance_index(instance_idx) _mesh_instance_index_ssbo.array[instance_idx].index
#endif


#ifdef cull_mesh_data_descriptor
#define mesh_indirect_draw_cmd_data_ssbo(_set_, _binding_, _access_) \
struct mesh_indirect_draw_cmd_data_t { \
    uint initial_instance_count; \
}; \
layout(set = _set_, binding = _binding_) buffer _access_ mesh_indirect_draw_cmd_data_ssbo_t { \
    mesh_indirect_draw_cmd_data_t array[]; \
} _mesh_indirect_draw_cmd_data_ssbo;

#define draw_initial_instance_count(idx) _mesh_indirect_draw_cmd_data_ssbo.array[idx].initial_instance_count
#endif


#ifdef cull_mesh_data_descriptor
#define mesh_indirect_draw_cmd_ssbo(_set_, _binding_, _access_) \
struct mesh_indirect_draw_cmd_t { \
    uint index_count; \
    uint instance_count; \
    uint first_index; \
    uint vertex_offset; \
    uint first_instance; \
}; \
layout(set = _set_, binding = _binding_) buffer _access_ mesh_indirect_draw_cmd_ssbo_t { \
    mesh_indirect_draw_cmd_t array[]; \
} _mesh_indirect_draw_cmd_ssbo;

#define draw_index_count(idx) _mesh_indirect_draw_cmd_ssbo.array[idx].index_count
#define draw_instance_count(idx) _mesh_indirect_draw_cmd_ssbo.array[idx].instance_count
#define draw_first_index(idx) _mesh_indirect_draw_cmd_ssbo.array[idx].first_index
#define draw_vertex_offset(idx) _mesh_indirect_draw_cmd_ssbo.array[idx].vertex_offset
#define draw_first_instance(idx) _mesh_indirect_draw_cmd_ssbo.array[idx].first_instance
#endif


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


vec3 aces_tonemap(vec3 color)
{
    vec3 numerator = color * (2.51 * color + 0.03);
    vec3 denominator = color * (2.43 * color + 0.59) + 0.14;
    return clamp(numerator / denominator, 0.0, 1.0);
}


float linearize_depth(float depth, float near_z, float far_z)
{
    return (near_z * far_z) /
           (far_z - depth * (far_z - near_z));
}


ivec2 clamp_pixel(ivec2 p, ivec2 s)
{
    return clamp(p, ivec2(0), s - ivec2(1));
}