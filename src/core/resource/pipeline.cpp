#include <core/types.h>


static b8 create_graphics_handle(resource::pipeline_t *pipeline, const resource::pipeline::setup_t *setup, const VkShaderModule *shader_module_array);
static b8 create_compute_handle(resource::pipeline_t *pipeline, const resource::pipeline::setup_t *setup, const VkShaderModule *shader_module_array);


namespace resource::pipeline
{
    pipeline_t *create(const setup_t *setup, const VkShaderModule *shader_module_array)
    {
        auto pipeline = find(setup->id);
        if (pipeline)
            return pipeline;


        pipeline = memory::resource_pool::allocate<pipeline_t>(&pipelines, setup->id);
        if (!pipeline)
        {
            logger::error(logger::code::allocate_from_pipeline_resource_pool, setup->id);
            return nullptr;
        }


        pipeline->handle = nullptr;
        pipeline->layout = pipeline_layouts.array[setup->layout_idx];

        bool r = !setup->is_compute ? create_graphics_handle(pipeline, setup, shader_module_array) : create_compute_handle(pipeline, setup, shader_module_array);

        if (!r)
        {
            destroy(pipeline);
            return nullptr;
        }


        return pipeline;
    }


    void destroy(pipeline_t *pipeline)
    {
        if (pipeline->handle)
        {
            vulkan::vkDestroyPipeline(renderer::device, pipeline->handle, nullptr);
        }
        memory::resource_pool::free(&pipelines, pipeline);
    }


    pipeline_t *find(const u16 id)
    {
        return memory::resource_pool::find<pipeline_t>(&pipelines, id);
    }
}


static b8 create_graphics_handle(resource::pipeline_t *pipeline, const resource::pipeline::setup_t *setup, const VkShaderModule *shader_module_array)
{
    if (!shader_module_array[resource::pipeline::vertex_shader_module_idx])
    {
        logger::error(logger::code::missing_vertex_shader, setup->vertex_shader_id);
        return false;
    }
    if (!shader_module_array[resource::pipeline::fragment_shader_module_idx])
    {
        logger::error(logger::code::missing_fragment_shader, setup->fragment_shader_id);
        return false;
    }

    struct
    {
        VkFormat attachment_formats[4];
        VkPipelineShaderStageCreateInfo stages[2];
        VkPipelineVertexInputStateCreateInfo vertex_input_state;
        VkPipelineInputAssemblyStateCreateInfo input_assembly_state;
        VkPipelineViewportStateCreateInfo viewport_state;
        VkPipelineRasterizationStateCreateInfo rasterization_state;
        VkPipelineMultisampleStateCreateInfo multisample_state;
        VkPipelineDepthStencilStateCreateInfo depth_stencil_state;
        VkPipelineColorBlendStateCreateInfo color_blend_state;
        VkPipelineColorBlendAttachmentState blend_attachments[4];
        VkDynamicState dynamic_states[2];
        VkPipelineDynamicStateCreateInfo dynamic_state;
        VkPipelineRenderingCreateInfo rendering_state;
        VkGraphicsPipelineCreateInfo pipeline_create_info;
    } _ = {};

    _.stages[resource::pipeline::vertex_shader_module_idx].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    _.stages[resource::pipeline::vertex_shader_module_idx].stage = VK_SHADER_STAGE_VERTEX_BIT;
    _.stages[resource::pipeline::vertex_shader_module_idx].module = shader_module_array[resource::pipeline::vertex_shader_module_idx];
    _.stages[resource::pipeline::vertex_shader_module_idx].pName = "main";
    _.stages[resource::pipeline::fragment_shader_module_idx].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    _.stages[resource::pipeline::fragment_shader_module_idx].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    _.stages[resource::pipeline::fragment_shader_module_idx].module = shader_module_array[resource::pipeline::fragment_shader_module_idx];
    _.stages[resource::pipeline::fragment_shader_module_idx].pName = "main";
    _.vertex_input_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    _.input_assembly_state.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    _.input_assembly_state.topology = setup->primitive_topology;
    _.input_assembly_state.primitiveRestartEnable = setup->primitive_restart_enabled;
    _.viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    _.viewport_state.viewportCount = 1;
    _.viewport_state.scissorCount = 1;
    _.rasterization_state.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    _.rasterization_state.polygonMode = setup->polygon_mode;
    _.rasterization_state.cullMode = setup->cull_mode;
    _.rasterization_state.frontFace = setup->front_face;
    _.rasterization_state.lineWidth = 1.0f;
    _.multisample_state.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    _.multisample_state.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    _.depth_stencil_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    _.depth_stencil_state.depthTestEnable = setup->depth_test_enabled;
    _.depth_stencil_state.depthWriteEnable = setup->depth_write_enabled;
    _.depth_stencil_state.depthCompareOp = VK_COMPARE_OP_LESS;
    _.color_blend_state.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    _.color_blend_state.pAttachments = _.blend_attachments;
    _.color_blend_state.blendConstants[0] = 1.0f;
    _.color_blend_state.blendConstants[1] = 1.0f;
    _.dynamic_states[0] = VK_DYNAMIC_STATE_VIEWPORT;
    _.dynamic_states[1] = VK_DYNAMIC_STATE_SCISSOR;
    _.dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    _.dynamic_state.dynamicStateCount = 2;
    _.dynamic_state.pDynamicStates = _.dynamic_states;
    _.rendering_state.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    _.rendering_state.pColorAttachmentFormats = _.attachment_formats;
    _.rendering_state.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;
    _.pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    _.pipeline_create_info.pNext = &_.rendering_state;
    _.pipeline_create_info.stageCount = 2;
    _.pipeline_create_info.pStages = _.stages;
    _.pipeline_create_info.pVertexInputState = &_.vertex_input_state;
    _.pipeline_create_info.pInputAssemblyState = &_.input_assembly_state;
    _.pipeline_create_info.pViewportState = &_.viewport_state;
    _.pipeline_create_info.pRasterizationState = &_.rasterization_state;
    _.pipeline_create_info.pMultisampleState = &_.multisample_state;
    _.pipeline_create_info.pDepthStencilState = &_.depth_stencil_state;
    _.pipeline_create_info.pColorBlendState = &_.color_blend_state;
    _.pipeline_create_info.pDynamicState = &_.dynamic_state;
    _.pipeline_create_info.layout = resource::pipeline_layouts.array[setup->layout_idx];
    _.pipeline_create_info.basePipelineIndex = -1;

    switch (setup->layout_idx)
    {
        case resource::pipeline_layout::draw_mesh::idx:
        case resource::pipeline_layout::draw_ui::idx:
            _.color_blend_state.attachmentCount = 2;
            _.blend_attachments[0].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            _.blend_attachments[1].colorWriteMask = VK_COLOR_COMPONENT_R_BIT;
            _.rendering_state.colorAttachmentCount = 2;
            _.rendering_state.depthAttachmentFormat = gpu_info::format->depth_buffer;
            _.attachment_formats[0] = gpu_info::swapchain->format;
            _.attachment_formats[1] = gpu_info::format->interaction_buffer;
            break;
        default:
            logger::error(logger::code::bad_graphics_pipeline_layout, setup->layout_idx);
            return false;
    }

    auto const result = vulkan::vkCreateGraphicsPipelines(renderer::device, nullptr, 1, &_.pipeline_create_info, nullptr, &pipeline->handle);
    if (result)
    {
        logger::error(logger::code::vkCreateGraphicsPipelines, result);
        return false;
    }
    return true;
}


static b8 create_compute_handle(resource::pipeline_t *pipeline, const resource::pipeline::setup_t *setup, const VkShaderModule *shader_module_array)
{
    if (!shader_module_array[resource::pipeline::compute_shader_module_idx])
    {
        logger::error(logger::code::missing_compute_shader, setup->compute_shader_id);
        return false;
    }

    VkComputePipelineCreateInfo pipeline_create_info = {};

    pipeline_create_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipeline_create_info.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    pipeline_create_info.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    pipeline_create_info.stage.module = shader_module_array[resource::pipeline::compute_shader_module_idx];
    pipeline_create_info.stage.pName = "main";
    pipeline_create_info.layout = resource::pipeline_layouts.array[setup->layout_idx];
    pipeline_create_info.basePipelineIndex = -1;

    auto const result = vulkan::vkCreateComputePipelines(renderer::device, nullptr, 1, &pipeline_create_info, nullptr, &pipeline->handle);
    if (result)
    {
        logger::error(logger::code::vkCreateComputePipelines, result);
        return false;
    }
    return true;
}
