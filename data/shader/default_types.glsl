#ifndef _EXTENSIONS_
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable
#define _EXTENSIONS_
#endif


const float inverse_255 = 1.0 / 255.0;


#ifdef frame_data_descriptor
#define frame_data_uniform(_set_, _binding_, _access_) \
layout(std140, set = _set_, binding = _binding_) uniform _access_ frame_data_uniform_t { \
    mat4 view; \
    mat4 projection; \
    mat4 view_projection; \
    vec4 packed0; \
} fdu;

#define view_matrix fdu.view
#define projection_matrix fdu.projection
#define view_projection_matrix fdu.view_projection
#define viewport_size fdu.packed0.xy
#define viewport_width fdu.packed0.x
#define viewport_height fdu.packed0.y
#define ndc_scale fdu.packed0.zw
#define ndc_scale_width fdu.packed0.z
#define ndc_scale_height fdu.packed0.w
#endif


#ifdef ui_data_descriptor
#define ui_data_instance_ssbo(_set_, _binding_, _access_) \
struct ui_data_instance_t { \
    vec4 packed0; \
    uint packed1; \
}; \
layout(set = _set_, binding = _binding_) buffer _access_ ui_data_instance_ssbo_t { \
    ui_data_instance_t array[]; \
} uidis;

#define position uidis.array[gl_InstanceIndex].packed0.xy
#define position_x uidis.array[gl_InstanceIndex].packed0.x
#define position_y uidis.array[gl_InstanceIndex].packed0.y
#define size uidis.array[gl_InstanceIndex].packed0.zw
#define size_x uidis.array[gl_InstanceIndex].packed0.z
#define size_y uidis.array[gl_InstanceIndex].packed0.w
#define interaction_id (uidis.array[gl_InstanceIndex].packed1 & 0xffff)
#endif


#ifdef mesh_data_descriptor
#define mesh_data_vertex_ssbo_array(_set_, _binding_, _access_) \
layout(set = _set_, binding = _binding_) buffer _access_ mesh_data_vertex_ssbo_array_t { \
    uint array[]; \
} mdvsa[];

#define local_position_vec3(ssbo) \
    vec3(uintBitsToFloat(mdvsa[ssbo].array[gl_VertexIndex * 3]), \
         uintBitsToFloat(mdvsa[ssbo].array[gl_VertexIndex * 3 + 1]), \
         uintBitsToFloat(mdvsa[ssbo].array[gl_VertexIndex * 3 + 2]))

#define local_position_vec4(ssbo) \
    vec4(uintBitsToFloat(mdvsa[ssbo].array[gl_VertexIndex * 3]), \
         uintBitsToFloat(mdvsa[ssbo].array[gl_VertexIndex * 3 + 1]), \
         uintBitsToFloat(mdvsa[ssbo].array[gl_VertexIndex * 3 + 2]), \
         1.0)
#endif


#ifdef mesh_data_descriptor
#define mesh_data_material_ssbo(_set_, _binding_, _access_) \
struct mesh_data_material_t { \
    uint color; \
}; \
layout(set = _set_, binding = _binding_) buffer _access_ mesh_data_material_ssbo_t { \
    mesh_data_material_t array[]; \
} mdms;


#define color_vec4(material_idx) \
    vec4(float((mdms.array[material_idx].color >> 24u) & 0xFFu), \
         float((mdms.array[material_idx].color >> 16u) & 0xFFu), \
         float((mdms.array[material_idx].color >>  8u) & 0xFFu), \
         float( mdms.array[material_idx].color        & 0xFFu)) * inverse_255

#endif


#if defined(mesh_data_descriptor) || defined(cull_mesh_data_descriptor)
#define mesh_data_instance_ssbo(_set_, _binding_, _access_) \
struct mesh_data_instance_t { \
    vec4 packed0; \
    vec4 packed1; \
    vec4 packed2; \
    uint packed3; \
    uint packed4; \
    uint _pad1; \
    uint _pad2; \
}; \
layout(set = _set_, binding = _binding_) buffer _access_ mesh_data_instance_ssbo_t { \
    mesh_data_instance_t array[]; \
} mdis;

#define vertex_position_ssbo_idx(instance_idx) (mdis.array[instance_idx].packed3 & 0xffffu)
#define vertex_attribute_ssbo_idx(instance_idx) ((mdis.array[instance_idx].packed3 & 0xffff0000u) >> 16)
#define material_idx(instance_idx) (mdis.array[instance_idx].packed4 & 0xffffu)
#define interaction_id(instance_idx) ((mdis.array[instance_idx].packed4 & 0xffff0000u) >> 16)

#define world_position(instance_idx, position) \
    vec3(dot(mdis.array[instance_idx].packed0, position), \
         dot(mdis.array[instance_idx].packed1, position), \
         dot(mdis.array[instance_idx].packed2, position))

#endif


#if defined(mesh_data_descriptor) || defined(cull_mesh_data_descriptor)
#define mesh_indirect_draw_instance_data_ssbo(_set_, _binding_, _access_) \
struct mesh_indirect_draw_instance_data_t { \
    uint index; \
}; \
layout(set = _set_, binding = _binding_) buffer _access_ mesh_indirect_draw_instance_data_ssbo_t { \
    mesh_indirect_draw_instance_data_t array[]; \
} midids;

#define draw_instance_index(idx) midids.array[idx].index
#endif


#ifdef cull_mesh_data_descriptor
#define mesh_indirect_draw_cmd_data_ssbo(_set_, _binding_, _access_) \
struct mesh_indirect_draw_cmd_data_t { \
    uint initial_instance_count; \
}; \
layout(set = _set_, binding = _binding_) buffer _access_ mesh_indirect_draw_cmd_data_ssbo_t { \
    mesh_indirect_draw_cmd_data_t array[]; \
} midcds;

#define draw_initial_instance_count(idx) midcds.array[idx].initial_instance_count
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
} midcs;

#define draw_index_count(idx) midcs.array[idx].index_count
#define draw_instance_count(idx) midcs.array[idx].instance_count
#define draw_first_index(idx) midcs.array[idx].first_index
#define draw_vertex_offset(idx) midcs.array[idx].vertex_offset
#define draw_first_instance(idx) midcs.array[idx].first_instance
#endif