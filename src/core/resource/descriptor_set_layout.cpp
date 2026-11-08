#include <core/types.h>


static void create_frame_data();
static void create_mesh_cull_data();
static void create_image_data();
static void create_ui_data();
static void create_mesh_data();


namespace resource::descriptor_set_layout
{
    void initialize()
    {
        create_frame_data();
        create_mesh_cull_data();
        create_image_data();
        create_ui_data();
        create_mesh_data();
    }
}


static void create_frame_data()
{
    constexpr u32 binding_count = 1;
    VkDescriptorSetLayoutBinding bindings[binding_count] = {};
    VkDescriptorSetLayoutBinding *binding;
    VkDescriptorSetLayoutCreateInfo layout_create_info = {};

    binding = &bindings[resource::descriptor_set_layout::frame_data::uniform_binding_idx];
    binding->binding = resource::descriptor_set_layout::frame_data::uniform_binding_idx;
    binding->descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    binding->descriptorCount = 1;
    binding->stageFlags = VK_SHADER_STAGE_ALL;

    layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_create_info.bindingCount = binding_count;
    layout_create_info.pBindings = bindings;

    const auto result = vulkan::vkCreateDescriptorSetLayout(renderer::device, &layout_create_info, nullptr, &resource::descriptor_set_layouts.frame_data);
    if (result)
    {
        logger::critical_error(logger::code::vkCreateDescriptorSetLayout_frame_data, result);
    }
}


static void create_mesh_cull_data()
{
    constexpr u32 binding_count = 4;
    VkDescriptorSetLayoutBinding bindings[binding_count] = {};
    VkDescriptorSetLayoutBinding *binding;
    VkDescriptorSetLayoutCreateInfo layout_create_info = {};

    binding = &bindings[resource::descriptor_set_layout::mesh_cull_data::instance_indices_binding_idx];
    binding->binding = resource::descriptor_set_layout::mesh_cull_data::instance_indices_binding_idx;
    binding->descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    binding->descriptorCount = 1;
    binding->stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    binding = &bindings[resource::descriptor_set_layout::mesh_cull_data::indirect_draw_cmd_data_binding_idx];
    binding->binding = resource::descriptor_set_layout::mesh_cull_data::indirect_draw_cmd_data_binding_idx;
    binding->descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    binding->descriptorCount = 1;
    binding->stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    binding = &bindings[resource::descriptor_set_layout::mesh_cull_data::indirect_draw_cmd_binding_idx];
    binding->binding = resource::descriptor_set_layout::mesh_cull_data::indirect_draw_cmd_binding_idx;
    binding->descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    binding->descriptorCount = 1;
    binding->stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    binding = &bindings[resource::descriptor_set_layout::mesh_cull_data::instances_binding_idx];
    binding->binding = resource::descriptor_set_layout::mesh_cull_data::instances_binding_idx;
    binding->descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    binding->descriptorCount = 1;
    binding->stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_create_info.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
    layout_create_info.bindingCount = binding_count;
    layout_create_info.pBindings = bindings;

    const auto result = vulkan::vkCreateDescriptorSetLayout(renderer::device, &layout_create_info, nullptr, &resource::descriptor_set_layouts.mesh_cull_data);
    if (result)
    {
        logger::critical_error(logger::code::vkCreateDescriptorSetLayout_mesh_cull_data, result);
    }
}


static void create_image_data()
{
    constexpr u32 binding_count = 2;
    VkDescriptorSetLayoutBinding bindings[binding_count] = {};
    VkDescriptorBindingFlags binding_flags[binding_count] = {};
    VkDescriptorSetLayoutBinding *binding;
    VkDescriptorBindingFlags *flags;
    VkDescriptorSetLayoutBindingFlagsCreateInfo binding_flags_info = {};
    VkDescriptorSetLayoutCreateInfo layout_create_info = {};

    binding = &bindings[resource::descriptor_set_layout::image_data::sampled_images_binding_idx];
    flags = &binding_flags[resource::descriptor_set_layout::image_data::sampled_images_binding_idx];
    binding->binding = resource::descriptor_set_layout::image_data::sampled_images_binding_idx;
    binding->descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    binding->descriptorCount = resource::descriptor_set_layout::image_data::sampled_images_binding_size;
    binding->stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    *flags = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;

    binding = &bindings[resource::descriptor_set_layout::image_data::samplers_binding_idx];
    flags = &binding_flags[resource::descriptor_set_layout::image_data::samplers_binding_idx];
    binding->binding = resource::descriptor_set_layout::image_data::samplers_binding_idx;
    binding->descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
    binding->descriptorCount = resource::descriptor_set_layout::image_data::samplers_binding_size;
    binding->stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    *flags = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;

    binding_flags_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
    binding_flags_info.bindingCount = binding_count;
    binding_flags_info.pBindingFlags = binding_flags;

    layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_create_info.pNext = &binding_flags_info;
    layout_create_info.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
    layout_create_info.bindingCount = binding_count;
    layout_create_info.pBindings = bindings;

    const auto result = vulkan::vkCreateDescriptorSetLayout(renderer::device, &layout_create_info, nullptr, &resource::descriptor_set_layouts.image_data);
    if (result)
    {
        logger::critical_error(logger::code::vkCreateDescriptorSetLayout_image_data, result);
    }
}


static void create_ui_data()
{
    constexpr u32 binding_count = 1;
    VkDescriptorSetLayoutBinding bindings[binding_count] = {};
    VkDescriptorSetLayoutBinding *binding;
    VkDescriptorSetLayoutCreateInfo layout_create_info = {};

    binding = &bindings[resource::descriptor_set_layout::ui_data::instances_binding_idx];
    binding->binding = resource::descriptor_set_layout::ui_data::instances_binding_idx;
    binding->descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    binding->descriptorCount = 1;
    binding->stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

    layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_create_info.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
    layout_create_info.bindingCount = binding_count;
    layout_create_info.pBindings = bindings;

    const auto result = vulkan::vkCreateDescriptorSetLayout(renderer::device, &layout_create_info, nullptr, &resource::descriptor_set_layouts.ui_data);
    if (result)
    {
        logger::critical_error(logger::code::vkCreateDescriptorSetLayout_ui_data, result);
    }
}


static void create_mesh_data()
{
    constexpr u32 binding_count = 4;
    VkDescriptorSetLayoutBinding bindings[binding_count] = {};
    VkDescriptorBindingFlags bindingFlags[binding_count] = {};
    VkDescriptorSetLayoutBinding *binding;
    VkDescriptorBindingFlags *flags;
    VkDescriptorSetLayoutBindingFlagsCreateInfo binding_flags_info = {};
    VkDescriptorSetLayoutCreateInfo layout_create_info = {};

    binding = &bindings[resource::descriptor_set_layout::mesh_data::vertex_ssbo_array_binding_idx];
    flags = &bindingFlags[resource::descriptor_set_layout::mesh_data::vertex_ssbo_array_binding_idx];
    binding->binding = resource::descriptor_set_layout::mesh_data::vertex_ssbo_array_binding_idx;
    binding->descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    binding->descriptorCount = resource::descriptor_set_layout::mesh_data::vertex_ssbo_array_binding_size;
    binding->stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    *flags = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;

    binding = &bindings[resource::descriptor_set_layout::mesh_data::instances_binding_idx];
    binding->binding = resource::descriptor_set_layout::mesh_data::instances_binding_idx;
    binding->descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    binding->descriptorCount = 1;
    binding->stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    binding = &bindings[resource::descriptor_set_layout::mesh_data::materials_binding_idx];
    binding->binding = resource::descriptor_set_layout::mesh_data::materials_binding_idx;
    binding->descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    binding->descriptorCount = 1;
    binding->stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

    binding = &bindings[resource::descriptor_set_layout::mesh_data::instance_indices_binding_idx];
    binding->binding = resource::descriptor_set_layout::mesh_data::instance_indices_binding_idx;
    binding->descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    binding->descriptorCount = 1;
    binding->stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    binding_flags_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
    binding_flags_info.bindingCount = binding_count;
    binding_flags_info.pBindingFlags = bindingFlags;

    layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_create_info.pNext = &binding_flags_info;
    layout_create_info.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
    layout_create_info.bindingCount = binding_count;
    layout_create_info.pBindings = bindings;

    const auto result = vulkan::vkCreateDescriptorSetLayout(renderer::device, &layout_create_info, nullptr, &resource::descriptor_set_layouts.mesh_data);
    if (result)
    {
        logger::critical_error(logger::code::vkCreateDescriptorSetLayout_mesh_data, result);
    }
}
