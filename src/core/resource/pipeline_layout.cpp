#include <core/types.h>


static void create_cull_mesh();
static void create_draw_mesh();
static void create_draw_ui();


namespace resource::pipeline_layout
{
    void initialize()
    {
        create_cull_mesh();
        create_draw_mesh();
        create_draw_ui();
    }
}


static void create_cull_mesh()
{
    constexpr u32 descriptor_set_count = resource::pipeline_layout::cull_mesh::descriptor_count;
    constexpr u32 push_constant_range_count = 1;

    VkDescriptorSetLayout descriptor_sets[descriptor_set_count];
    VkPushConstantRange push_constant_ranges[push_constant_range_count];
    VkPipelineLayoutCreateInfo layout_create_info = {};

    descriptor_sets[resource::pipeline_layout::cull_mesh::frame_data_descriptor_idx] = resource::descriptor_set_layouts.frame_data;
    descriptor_sets[resource::pipeline_layout::cull_mesh::mesh_cull_data_descriptor_idx] = resource::descriptor_set_layouts.mesh_cull_data;

    push_constant_ranges[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    push_constant_ranges[0].offset = 0;
    push_constant_ranges[0].size = sizeof(resource::pipeline_layout::cull_mesh::push_constant_t);

    layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layout_create_info.setLayoutCount = descriptor_set_count;
    layout_create_info.pSetLayouts = descriptor_sets;
    layout_create_info.pushConstantRangeCount = push_constant_range_count;
    layout_create_info.pPushConstantRanges = push_constant_ranges;

    const auto result = vulkan::vkCreatePipelineLayout(renderer::device, &layout_create_info, nullptr, &resource::pipeline_layouts.cull_mesh);
    if (result)
    {
        logger::critical_error(logger::code::vkCreatePipelineLayout_cull_mesh, result);
    }
}


static void create_draw_mesh()
{
    constexpr u32 descriptor_set_count = resource::pipeline_layout::draw_mesh::descriptor_count;

    VkDescriptorSetLayout descriptor_sets[descriptor_set_count];
    VkPipelineLayoutCreateInfo layout_create_info = {};

    descriptor_sets[resource::pipeline_layout::draw_mesh::frame_data_descriptor_idx] = resource::descriptor_set_layouts.frame_data;
    descriptor_sets[resource::pipeline_layout::draw_mesh::mesh_data_descriptor_idx] = resource::descriptor_set_layouts.mesh_data;
    descriptor_sets[resource::pipeline_layout::draw_mesh::image_data_descriptor_idx] = resource::descriptor_set_layouts.image_data;

    layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layout_create_info.setLayoutCount = descriptor_set_count;
    layout_create_info.pSetLayouts = descriptor_sets;

    const auto result = vulkan::vkCreatePipelineLayout(renderer::device, &layout_create_info, nullptr, &resource::pipeline_layouts.draw_mesh);
    if (result)
    {
        logger::critical_error(logger::code::vkCreatePipelineLayout_draw_mesh, result);
    }
}


static void create_draw_ui()
{
    constexpr u32 descriptor_set_count = resource::pipeline_layout::draw_ui::descriptor_count;

    VkDescriptorSetLayout descriptor_sets[descriptor_set_count];
    VkPipelineLayoutCreateInfo layout_create_info = {};

    descriptor_sets[resource::pipeline_layout::draw_ui::frame_data_descriptor_idx] = resource::descriptor_set_layouts.frame_data;
    descriptor_sets[resource::pipeline_layout::draw_ui::ui_data_descriptor_idx] = resource::descriptor_set_layouts.ui_data;
    descriptor_sets[resource::pipeline_layout::draw_ui::image_data_descriptor_idx] = resource::descriptor_set_layouts.image_data;

    layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layout_create_info.setLayoutCount = descriptor_set_count;
    layout_create_info.pSetLayouts = descriptor_sets;

    const auto result = vulkan::vkCreatePipelineLayout(renderer::device, &layout_create_info, nullptr, &resource::pipeline_layouts.draw_ui);
    if (result)
    {
        logger::critical_error(logger::code::vkCreatePipelineLayout_draw_ui, result);
    }
}
