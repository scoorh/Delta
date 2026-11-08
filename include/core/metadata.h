#pragma once


// ==================================================
// resource package
// ==================================================
namespace metadata::resource_package
{
    constexpr u32 buffer_setup_size = 12;
    constexpr u32 mesh_setup_size = 20;
    constexpr u32 image_setup_size = 20;
    constexpr u32 image_sampler_setup_size = 20;
    constexpr u32 material_setup_size = 8;
    constexpr u32 shader_setup_size = 12;
    constexpr u32 pipeline_setup_size = 28;
    constexpr u32 package_header_size = 44;

    constexpr u32 pipeline_layout_cull_mesh = 1;
    constexpr u32 pipeline_layout_draw_mesh = 2;
    constexpr u32 pipeline_layout_draw_ui = 0;

    constexpr u32 filter_nearest = 0;
    constexpr u32 filter_linear = 1;
    constexpr u32 address_mode_repeat = 0;
    constexpr u32 address_mode_mirrored_repeat = 1;
    constexpr u32 address_mode_clamp_to_edge = 2;
    constexpr u32 address_mode_clamp_to_border = 3;
    constexpr u32 address_mode_mirror_clamp_to_edge = 4;
    constexpr u32 mipmap_mode_nearest = 0;
    constexpr u32 mipmap_mode_linear = 1;

    constexpr u32 polygon_mode_fill = 0;
    constexpr u32 polygon_mode_line = 1;
    constexpr u32 polygon_mode_point = 2;
    constexpr u32 cull_mode_none = 0;
    constexpr u32 cull_mode_front_face = 1;
    constexpr u32 cull_mode_back_face = 2;
    constexpr u32 front_face_clockwise = 1;
    constexpr u32 front_face_counter_clockwise = 0;
    constexpr u32 primitive_topology_point_list = 0;
    constexpr u32 primitive_topology_line_list = 1;
    constexpr u32 primitive_topology_line_strip = 2;
    constexpr u32 primitive_topology_triangle_list = 3;
    constexpr u32 primitive_topology_triangle_strip = 4;
    constexpr u32 primitive_topology_triangle_fan = 5;

    constexpr u32 usage_index_buffer = 66;
    constexpr u32 usage_vertex_storage_buffer = 34;
}


static_assert(metadata::resource_package::buffer_setup_size == sizeof(resource::buffer::setup_t));
static_assert(metadata::resource_package::mesh_setup_size == sizeof(resource::mesh::setup_t));
static_assert(metadata::resource_package::image_setup_size == sizeof(resource::image::setup_t));
static_assert(metadata::resource_package::image_sampler_setup_size == sizeof(resource::image_sampler::setup_t));
static_assert(metadata::resource_package::material_setup_size == sizeof(resource::material::setup_t));
static_assert(metadata::resource_package::shader_setup_size == sizeof(resource::shader::setup_t));
static_assert(metadata::resource_package::pipeline_setup_size == sizeof(resource::pipeline::setup_t));
static_assert(metadata::resource_package::package_header_size == sizeof(file::resource_package_t::header_t));

static_assert(offsetof(resource::pipeline_layout_t, array[metadata::resource_package::pipeline_layout_draw_ui]) == offsetof(resource::pipeline_layout_t, draw_ui));
static_assert(offsetof(resource::pipeline_layout_t, array[metadata::resource_package::pipeline_layout_draw_mesh]) == offsetof(resource::pipeline_layout_t, draw_mesh));
static_assert(offsetof(resource::pipeline_layout_t, array[metadata::resource_package::pipeline_layout_cull_mesh]) == offsetof(resource::pipeline_layout_t, cull_mesh));

static_assert(metadata::resource_package::filter_nearest == VK_FILTER_NEAREST);
static_assert(metadata::resource_package::filter_linear == VK_FILTER_LINEAR);
static_assert(metadata::resource_package::address_mode_repeat == VK_SAMPLER_ADDRESS_MODE_REPEAT);
static_assert(metadata::resource_package::address_mode_mirrored_repeat == VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT);
static_assert(metadata::resource_package::address_mode_clamp_to_edge == VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
static_assert(metadata::resource_package::address_mode_clamp_to_border == VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER);
static_assert(metadata::resource_package::address_mode_mirror_clamp_to_edge == VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE);
static_assert(metadata::resource_package::mipmap_mode_nearest == VK_SAMPLER_MIPMAP_MODE_NEAREST);
static_assert(metadata::resource_package::mipmap_mode_linear == VK_SAMPLER_MIPMAP_MODE_LINEAR);

static_assert(metadata::resource_package::polygon_mode_fill == VK_POLYGON_MODE_FILL);
static_assert(metadata::resource_package::polygon_mode_line == VK_POLYGON_MODE_LINE);
static_assert(metadata::resource_package::polygon_mode_point == VK_POLYGON_MODE_POINT);
static_assert(metadata::resource_package::cull_mode_none == VK_CULL_MODE_NONE);
static_assert(metadata::resource_package::cull_mode_front_face == VK_CULL_MODE_FRONT_BIT);
static_assert(metadata::resource_package::cull_mode_back_face == VK_CULL_MODE_BACK_BIT);
static_assert(metadata::resource_package::front_face_clockwise == VK_FRONT_FACE_CLOCKWISE);
static_assert(metadata::resource_package::front_face_counter_clockwise == VK_FRONT_FACE_COUNTER_CLOCKWISE);
static_assert(metadata::resource_package::primitive_topology_point_list == VK_PRIMITIVE_TOPOLOGY_POINT_LIST);
static_assert(metadata::resource_package::primitive_topology_line_list == VK_PRIMITIVE_TOPOLOGY_LINE_LIST);
static_assert(metadata::resource_package::primitive_topology_line_strip == VK_PRIMITIVE_TOPOLOGY_LINE_STRIP);
static_assert(metadata::resource_package::primitive_topology_triangle_list == VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
static_assert(metadata::resource_package::primitive_topology_triangle_strip == VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP);
static_assert(metadata::resource_package::primitive_topology_triangle_fan == VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN);

static_assert(metadata::resource_package::usage_index_buffer == (VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT));
static_assert(metadata::resource_package::usage_vertex_storage_buffer == (VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT));
