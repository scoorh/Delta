#include <core/types.h>

//TODO:  remove
#include "game/metadata.h"


static void wait_for_fence(renderer::frame_t *frame);
static void reset_fence(renderer::frame_t *frame);
static void reset_commands(renderer::frame_t *frame);
static b8 acquire_swapchain_image(renderer::frame_t *frame);
static void present_swapchain_image(renderer::frame_t *frame);
static void begin_draw_to_swapchain_commands(renderer::frame_t *frame);
static void submit_draw_to_swapchain_commands(renderer::frame_t *frame);
static void transfer_frame_data(renderer::frame_t *frame);
static void transfer_frame_uniform(renderer::frame_t *frame);
static void submit_compute_commands(renderer::frame_t *frame);


namespace renderer
{
    void initialize()
    {
        initialize_vulkan();
        window::initialize();
        gpu_info::enumerate();
        initialize_device();
        initialize_swapchain();
        resource::descriptor_set_layout::initialize();
        resource::pipeline_layout::initialize();
        resource::gpu_transfer_work::initialize();
        file::resource_package::load_everything("data/core.package");
        initialize_frames();
    }


    void draw_frame()
    {
        const u32 frame_idx = current_frame % frames_in_flight;
        const auto frame = &frames[frame_idx];

        ++current_frame;

        wait_for_fence(frame);

        if (!acquire_swapchain_image(frame))
            return;

        reset_fence(frame);
        reset_commands(frame);

        transfer_frame_data(frame);
        submit_compute_commands(frame);
        begin_draw_to_swapchain_commands(frame);
        submit_draw_to_swapchain_commands(frame);
        present_swapchain_image(frame);
    }
}


static void transfer_frame_uniform(renderer::frame_t *frame)
{
    const auto gpu_transfer_work = frame->gpu_transfer_work;


    math::vector4_t global_light_direction = {-0.57735f, -0.57735f, -0.57735f, 0.0f};
    math::quaternion_t camera_rotation;
    math::vector4_t camera_position = {0.0f, 4.0f, 2.0f, 0.0f};
    math::quaternion::from_euler(&camera_rotation, 0, 0, 0);
    //math::vector4_t camera_position = {15.0f, 20.0f, 15.0f, 0.0f};
    //math::quaternion::from_euler(&camera_rotation, math::degree_to_radian(-39.23f), math::degree_to_radian(45.0f), 0.0f);


    const auto frame_data_uniform = resource::gpu_transfer_work::memory_pointer<resource::shader::frame_uniform_t>(gpu_transfer_work);
    const auto frame_data_uniform_src_offset = resource::gpu_transfer_work::memory_offset(gpu_transfer_work);


    frame_data_uniform->global_light_direction = global_light_direction;
    frame_data_uniform->camera_position = camera_position;
    frame_data_uniform->viewport_size.width = (f32) renderer::window_width;
    frame_data_uniform->viewport_size.height = (f32) renderer::window_height;
    frame_data_uniform->ndc_scale.width = 2.0f / (f32) renderer::window_width;
    frame_data_uniform->ndc_scale.height = 2.0f / (f32) renderer::window_height;

    math::matrix4x4::identity(&frame_data_uniform->projection);
    math::matrix4x4::identity(&frame_data_uniform->view);
    math::matrix4x4::perspective(&frame_data_uniform->projection, (f32) renderer::window_width / (f32) renderer::window_height, math::degree_to_radian(60), 0.01f, 1000.0f);
    math::matrix4x4::look_at(&frame_data_uniform->view, &camera_position, &camera_rotation);
    math::matrix4x4::multiply(&frame_data_uniform->view_projection, &frame_data_uniform->projection, &frame_data_uniform->view);


    resource::gpu_transfer_work_t::cmd_t transfer_cmd = {};
    transfer_cmd.buffer.dst = frame->data_buffer;
    transfer_cmd.buffer.dst_offset = frame->data_offset.frame_uniform;
    transfer_cmd.buffer.src_offset = frame_data_uniform_src_offset;
    transfer_cmd.buffer.src_size = sizeof(resource::shader::frame_uniform_t);
    transfer_cmd.memory_barrier.stage_mask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
    transfer_cmd.memory_barrier.access_mask = VK_ACCESS_2_UNIFORM_READ_BIT;
    resource::gpu_transfer_work::send_buffer_cmd(gpu_transfer_work, &transfer_cmd);


    const auto mesh_instance = resource::gpu_transfer_work::memory_pointer<resource::shader::mesh_instance_t>(gpu_transfer_work);
    const auto mesh_instance_src_offset = resource::gpu_transfer_work::memory_offset(gpu_transfer_work);
    const u32 mesh_instance_count = 1;

    math::matrix4x4_t m;
    math::quaternion_t q = {1.0f, 0.0f, 0.0f, 0.0f};
    math::matrix4x4::world_transform(&m, 0.0f, 0.0f, 0.0f, 0.5f, &q);

    mesh_instance[0].packed0.x = m.columns[0].x;
    mesh_instance[0].packed0.y = m.columns[1].x;
    mesh_instance[0].packed0.z = m.columns[2].x;
    mesh_instance[0].packed0.w = m.columns[3].x;
    mesh_instance[0].packed1.x = m.columns[0].y;
    mesh_instance[0].packed1.y = m.columns[1].y;
    mesh_instance[0].packed1.z = m.columns[2].y;
    mesh_instance[0].packed1.w = m.columns[3].y;
    mesh_instance[0].packed2.x = m.columns[0].z;
    mesh_instance[0].packed2.y = m.columns[1].z;
    mesh_instance[0].packed2.z = m.columns[2].z;
    mesh_instance[0].packed2.w = m.columns[3].z;

    auto mesh = resource::mesh::find(metadata::resource_package::default_character_body_mesh_id);
    mesh_instance[0].position_vertex_ssbo_idx = mesh->position_vertex_ssbo_idx;
    mesh_instance[0].attribute_vertex_ssbo_idx = mesh->attribute_vertex_ssbo_idx;
    mesh_instance[0].material_idx = 0;
    mesh_instance[0].interaction_id = 0;


    transfer_cmd.buffer.dst = frame->data_buffer;
    transfer_cmd.buffer.dst_offset = frame->data_offset.mesh_instances;
    transfer_cmd.buffer.src_offset = mesh_instance_src_offset;
    transfer_cmd.buffer.src_size = sizeof(resource::shader::mesh_instance_t) * mesh_instance_count;
    transfer_cmd.memory_barrier.stage_mask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT;
    transfer_cmd.memory_barrier.access_mask = VK_ACCESS_2_SHADER_STORAGE_READ_BIT;
    resource::gpu_transfer_work::send_buffer_cmd(gpu_transfer_work, &transfer_cmd);


    const auto mesh_indirect_draw_cmd = resource::gpu_transfer_work::memory_pointer<resource::shader::mesh_indirect_draw_cmd_t>(gpu_transfer_work);
    const auto mesh_indirect_draw_cmd_src_offset = resource::gpu_transfer_work::memory_offset(gpu_transfer_work);
    const u32 mesh_indirect_draw_cmd_count = 1;


    mesh_indirect_draw_cmd[0].index_count = mesh->index_count;
    mesh_indirect_draw_cmd[0].first_index = mesh->first_index;
    mesh_indirect_draw_cmd[0].vertex_offset = mesh->first_vertex;
    mesh_indirect_draw_cmd[0].first_instance = 0;


    transfer_cmd.buffer.dst = frame->data_buffer;
    transfer_cmd.buffer.dst_offset = frame->data_offset.mesh_indirect_draw_cmds;
    transfer_cmd.buffer.src_offset = mesh_indirect_draw_cmd_src_offset;
    transfer_cmd.buffer.src_size = sizeof(resource::shader::mesh_indirect_draw_cmd_t) * mesh_indirect_draw_cmd_count;
    transfer_cmd.memory_barrier.stage_mask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
    transfer_cmd.memory_barrier.access_mask = VK_ACCESS_2_SHADER_STORAGE_READ_BIT;
    resource::gpu_transfer_work::send_buffer_cmd(gpu_transfer_work, &transfer_cmd);


    const auto mesh_indirect_draw_cmd_data = resource::gpu_transfer_work::memory_pointer<resource::shader::mesh_indirect_draw_cmd_data_t>(gpu_transfer_work);
    const auto mesh_indirect_draw_cmd_data_src_offset = resource::gpu_transfer_work::memory_offset(gpu_transfer_work);
    const u32 mesh_indirect_draw_cmd_data_count = 1;

    mesh_indirect_draw_cmd_data[0].initial_instance_count = 1;

    transfer_cmd.buffer.dst = frame->data_buffer;
    transfer_cmd.buffer.dst_offset = frame->data_offset.mesh_indirect_draw_cmds_data;
    transfer_cmd.buffer.src_offset = mesh_indirect_draw_cmd_data_src_offset;
    transfer_cmd.buffer.src_size = sizeof(resource::shader::mesh_indirect_draw_cmd_data_t) * mesh_indirect_draw_cmd_data_count;
    transfer_cmd.memory_barrier.stage_mask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
    transfer_cmd.memory_barrier.access_mask = VK_ACCESS_2_SHADER_STORAGE_READ_BIT;
    resource::gpu_transfer_work::send_buffer_cmd(gpu_transfer_work, &transfer_cmd);
}


static void transfer_frame_data(renderer::frame_t *frame)
{
    const auto gpu_transfer_work = frame->gpu_transfer_work;

    resource::gpu_transfer_work::begin(gpu_transfer_work);
    transfer_frame_uniform(frame);
    resource::gpu_transfer_work::submit(gpu_transfer_work, nullptr);
}


static void submit_compute_commands(renderer::frame_t *frame)
{
    const auto cmd = frame->compute_cmd_buffer;

    VkCommandBufferBeginInfo command_buffer_begin_info = {};
    command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    auto result = vulkan::vkBeginCommandBuffer(cmd, &command_buffer_begin_info);
    if (result)
    {
        logger::critical_error(logger::code::vkBeginCommandBuffer_compute, result);
    }


    resource::pipeline_layout::cull_mesh::push_constant_t cull_mesh_push_constant = {};
    cull_mesh_push_constant.instance_count = 1;
    cull_mesh_push_constant.indirect_draw_cmd_count = 1;

    vulkan::vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, frame->cull_mesh_pipeline);
    vulkan::vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, frame->cull_mesh_pipeline_layout, 0, resource::pipeline_layout::cull_mesh::descriptor_count, frame->descriptor_set.cull_mesh_array, 0, nullptr);
    vulkan::vkCmdPushConstants(cmd, frame->cull_mesh_pipeline_layout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(cull_mesh_push_constant), &cull_mesh_push_constant);
    vulkan::vkCmdDispatch(cmd, (cull_mesh_push_constant.indirect_draw_cmd_count + frame->compute_shader_wave_size - 1) / frame->compute_shader_wave_size, 1, 1);


    VkBufferMemoryBarrier2 buffer_memory_barrier[2] = {};
    VkDependencyInfo barrier_dependency_info = {};

    buffer_memory_barrier[0].sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2;
    buffer_memory_barrier[0].srcStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
    buffer_memory_barrier[0].srcAccessMask = VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT;
    buffer_memory_barrier[0].dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT;
    buffer_memory_barrier[0].dstAccessMask = VK_ACCESS_2_SHADER_STORAGE_READ_BIT;
    buffer_memory_barrier[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    buffer_memory_barrier[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    buffer_memory_barrier[0].buffer = frame->data_buffer;
    buffer_memory_barrier[0].offset = frame->data_offset.mesh_instance_indices;
    buffer_memory_barrier[0].size = sizeof(resource::shader::mesh_instance_index_t) * cull_mesh_push_constant.instance_count;
    buffer_memory_barrier[1].sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2;
    buffer_memory_barrier[1].srcStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
    buffer_memory_barrier[1].srcAccessMask = VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT;
    buffer_memory_barrier[1].dstStageMask = VK_PIPELINE_STAGE_2_DRAW_INDIRECT_BIT;
    buffer_memory_barrier[1].dstAccessMask = VK_ACCESS_2_INDIRECT_COMMAND_READ_BIT;
    buffer_memory_barrier[1].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    buffer_memory_barrier[1].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    buffer_memory_barrier[1].buffer = frame->data_buffer;
    buffer_memory_barrier[1].offset = frame->data_offset.mesh_indirect_draw_cmds;
    buffer_memory_barrier[1].size = sizeof(resource::shader::mesh_indirect_draw_cmd_t) * cull_mesh_push_constant.indirect_draw_cmd_count;

    barrier_dependency_info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    barrier_dependency_info.bufferMemoryBarrierCount = 2;
    barrier_dependency_info.pBufferMemoryBarriers = buffer_memory_barrier;

    vulkan::vkCmdPipelineBarrier2(cmd, &barrier_dependency_info);


    result = vulkan::vkEndCommandBuffer(cmd);
    if (result)
    {
        logger::critical_error(logger::code::vkEndCommandBuffer_compute, result);
    }


    VkSemaphoreSubmitInfo wait_semaphore_info = {};
    VkCommandBufferSubmitInfo cmd_buffer_submit_info = {};
    VkSemaphoreSubmitInfo signal_semaphore_info = {};
    VkSubmitInfo2 submit_info = {};

    wait_semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    wait_semaphore_info.semaphore = frame->gpu_transfer_completed;
    wait_semaphore_info.stageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;

    cmd_buffer_submit_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    cmd_buffer_submit_info.commandBuffer = cmd;

    signal_semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    signal_semaphore_info.semaphore = frame->compute_completed;
    signal_semaphore_info.stageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;

    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    submit_info.waitSemaphoreInfoCount = 1;
    submit_info.pWaitSemaphoreInfos = &wait_semaphore_info;
    submit_info.commandBufferInfoCount = 1;
    submit_info.pCommandBufferInfos = &cmd_buffer_submit_info;
    submit_info.signalSemaphoreInfoCount = 1;
    submit_info.pSignalSemaphoreInfos = &signal_semaphore_info;

    renderer::lock_queue_mutex();
    result = vulkan::vkQueueSubmit2(renderer::default_queue, 1, &submit_info, nullptr);
    renderer::unlock_queue_mutex();

    if (result)
    {
        logger::critical_error(logger::code::vkQueueSubmit2_compute, result);
    }
}


static void begin_draw_to_swapchain_commands(renderer::frame_t *frame)
{
    const auto cmd = frame->draw_to_swapchain_cmd_buffer;

    VkCommandBufferBeginInfo command_buffer_begin_info = {};
    command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    auto result = vulkan::vkBeginCommandBuffer(cmd, &command_buffer_begin_info);
    if (result)
    {
        logger::critical_error(logger::code::vkBeginCommandBuffer_draw_to_swapchain, result);
    }


    constexpr u32 attachment_image_barrier_count = 3;
    VkImageMemoryBarrier2 attachment_image_barriers[attachment_image_barrier_count] = {};
    VkDependencyInfo barrier_dependency_info = {};

    barrier_dependency_info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    barrier_dependency_info.imageMemoryBarrierCount = attachment_image_barrier_count;
    barrier_dependency_info.pImageMemoryBarriers = attachment_image_barriers;

    attachment_image_barriers[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    attachment_image_barriers[0].srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
    attachment_image_barriers[0].srcAccessMask = 0;
    attachment_image_barriers[0].dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
    attachment_image_barriers[0].dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
    attachment_image_barriers[0].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachment_image_barriers[0].newLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
    attachment_image_barriers[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    attachment_image_barriers[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    attachment_image_barriers[0].image = renderer::swapchain_image_array[frame->acquired_swapchain_image_idx];
    attachment_image_barriers[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    attachment_image_barriers[0].subresourceRange.levelCount = 1;
    attachment_image_barriers[0].subresourceRange.layerCount = 1;

    attachment_image_barriers[2].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    attachment_image_barriers[2].srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
    attachment_image_barriers[2].srcAccessMask = 0;
    attachment_image_barriers[2].dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
    attachment_image_barriers[2].dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
    attachment_image_barriers[2].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachment_image_barriers[2].newLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
    attachment_image_barriers[2].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    attachment_image_barriers[2].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    attachment_image_barriers[2].image = renderer::interaction_buffer_image;
    attachment_image_barriers[2].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    attachment_image_barriers[2].subresourceRange.levelCount = 1;
    attachment_image_barriers[2].subresourceRange.layerCount = 1;

    attachment_image_barriers[1].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    attachment_image_barriers[1].srcStageMask = VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
    attachment_image_barriers[1].srcAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    attachment_image_barriers[1].dstStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT;
    attachment_image_barriers[1].dstAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    attachment_image_barriers[1].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachment_image_barriers[1].newLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
    attachment_image_barriers[1].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    attachment_image_barriers[1].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    attachment_image_barriers[1].image = renderer::depth_buffer_image;
    attachment_image_barriers[1].subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    attachment_image_barriers[1].subresourceRange.levelCount = 1;
    attachment_image_barriers[1].subresourceRange.layerCount = 1;

    vulkan::vkCmdPipelineBarrier2(cmd, &barrier_dependency_info);


    constexpr u32 color_attachment_count = 2;
    VkRenderingAttachmentInfo color_attachment_info[color_attachment_count] = {};
    VkRenderingAttachmentInfo depth_attachment_info = {};

    color_attachment_info[0].sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    color_attachment_info[0].imageView = renderer::swapchain_image_view_array[frame->acquired_swapchain_image_idx];
    color_attachment_info[0].imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
    color_attachment_info[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment_info[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment_info[0].clearValue.color.float32[0] = 0.39f;
    color_attachment_info[0].clearValue.color.float32[1] = 0.58f;
    color_attachment_info[0].clearValue.color.float32[2] = 0.93f;
    color_attachment_info[0].clearValue.color.float32[3] = 1.0f;

    color_attachment_info[1].sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    color_attachment_info[1].imageView = renderer::interaction_buffer_image_view;
    color_attachment_info[1].imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
    color_attachment_info[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment_info[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment_info[1].clearValue.color.uint32[0] = 0xffffffff;

    depth_attachment_info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    depth_attachment_info.imageView = renderer::depth_buffer_image_view;
    depth_attachment_info.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
    depth_attachment_info.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment_info.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment_info.clearValue.depthStencil.depth = 1.0f;

    VkRenderingInfo rendering_info = {};
    rendering_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    rendering_info.renderArea.extent.width = renderer::window_width;
    rendering_info.renderArea.extent.height = renderer::window_height;
    rendering_info.layerCount = 1;
    rendering_info.colorAttachmentCount = color_attachment_count;
    rendering_info.pColorAttachments = color_attachment_info;
    rendering_info.pDepthAttachment = &depth_attachment_info;

    vulkan::vkCmdBeginRendering(cmd, &rendering_info);


    VkRect2D scissor = {};
    VkViewport viewport = {};

    scissor.extent.width = renderer::window_width;
    scissor.extent.height = renderer::window_height;
    viewport.width = static_cast<f32>(renderer::window_width);
    viewport.height = static_cast<f32>(renderer::window_height);
    viewport.maxDepth = 1.0f;

    vulkan::vkCmdSetViewport(cmd, 0, 1, &viewport);
    vulkan::vkCmdSetScissor(cmd, 0, 1, &scissor);


    auto pipeline = resource::pipeline::find(metadata::resource_package::default_draw_mesh_pipeline_id);
    vulkan::vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->handle);
    vulkan::vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->layout, 0, resource::pipeline_layout::draw_mesh::descriptor_count, frame->descriptor_set.draw_mesh_array, 0, nullptr);

    auto mesh = resource::mesh::find(metadata::resource_package::default_character_body_mesh_id);
    vulkan::vkCmdBindIndexBuffer(cmd, mesh->index_buffer, 0, VK_INDEX_TYPE_UINT16);

    vulkan::vkCmdDrawIndexedIndirect(cmd, frame->data_buffer, frame->data_offset.mesh_indirect_draw_cmds, 1, sizeof(resource::shader::mesh_indirect_draw_cmd_t));
}


static void submit_draw_to_swapchain_commands(renderer::frame_t *frame)
{
    const auto cmd = frame->draw_to_swapchain_cmd_buffer;

    vulkan::vkCmdEndRendering(cmd);


    constexpr u32 attachment_image_barrier_count = 2;
    VkImageMemoryBarrier2 attachment_image_barriers[attachment_image_barrier_count] = {};
    VkDependencyInfo barrier_dependency_info = {};

    barrier_dependency_info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    barrier_dependency_info.imageMemoryBarrierCount = attachment_image_barrier_count;
    barrier_dependency_info.pImageMemoryBarriers = attachment_image_barriers;

    attachment_image_barriers[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    attachment_image_barriers[0].srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
    attachment_image_barriers[0].srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
    attachment_image_barriers[0].dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
    attachment_image_barriers[0].dstAccessMask = 0;
    attachment_image_barriers[0].oldLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
    attachment_image_barriers[0].newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    attachment_image_barriers[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    attachment_image_barriers[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    attachment_image_barriers[0].image = renderer::swapchain_image_array[frame->acquired_swapchain_image_idx];
    attachment_image_barriers[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    attachment_image_barriers[0].subresourceRange.levelCount = 1;
    attachment_image_barriers[0].subresourceRange.layerCount = 1;

    attachment_image_barriers[1].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    attachment_image_barriers[1].srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
    attachment_image_barriers[1].srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
    attachment_image_barriers[1].dstStageMask = VK_PIPELINE_STAGE_2_COPY_BIT;
    attachment_image_barriers[1].dstAccessMask = VK_ACCESS_2_TRANSFER_READ_BIT;
    attachment_image_barriers[1].oldLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
    attachment_image_barriers[1].newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    attachment_image_barriers[1].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    attachment_image_barriers[1].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    attachment_image_barriers[1].image = renderer::interaction_buffer_image;
    attachment_image_barriers[1].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    attachment_image_barriers[1].subresourceRange.levelCount = 1;
    attachment_image_barriers[1].subresourceRange.layerCount = 1;

    vulkan::vkCmdPipelineBarrier2(cmd, &barrier_dependency_info);


    auto result = vulkan::vkEndCommandBuffer(cmd);
    if (result)
    {
        logger::critical_error(logger::code::vkEndCommandBuffer_draw_to_swapchain, result);
    }


    constexpr u32 wait_semaphore_info_count = 2;
    VkSemaphoreSubmitInfo wait_semaphore_info[wait_semaphore_info_count] = {};
    VkCommandBufferSubmitInfo cmd_buffer_submit_info = {};
    VkSemaphoreSubmitInfo signal_semaphore_info = {};
    VkSubmitInfo2 submit_info = {};

    wait_semaphore_info[0].sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    wait_semaphore_info[0].semaphore = frame->acquired_swapchain_image;
    wait_semaphore_info[0].stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
    wait_semaphore_info[1].sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    wait_semaphore_info[1].semaphore = frame->compute_completed;
    wait_semaphore_info[1].stageMask = VK_PIPELINE_STAGE_2_DRAW_INDIRECT_BIT;

    cmd_buffer_submit_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    cmd_buffer_submit_info.commandBuffer = cmd;

    signal_semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    signal_semaphore_info.semaphore = frame->draw_to_swapchain_completed;
    signal_semaphore_info.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;

    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    submit_info.waitSemaphoreInfoCount = wait_semaphore_info_count;
    submit_info.pWaitSemaphoreInfos = wait_semaphore_info;
    submit_info.commandBufferInfoCount = 1;
    submit_info.pCommandBufferInfos = &cmd_buffer_submit_info;
    submit_info.signalSemaphoreInfoCount = 1;
    submit_info.pSignalSemaphoreInfos = &signal_semaphore_info;

    renderer::lock_queue_mutex();
    result = vulkan::vkQueueSubmit2(renderer::default_queue, 1, &submit_info, frame->work_completed);
    renderer::unlock_queue_mutex();

    if (result)
    {
        logger::critical_error(logger::code::vkQueueSubmit2_draw_to_swapchain, result);
    }
}


static b8 acquire_swapchain_image(renderer::frame_t *frame)
{
    const auto result = vulkan::vkAcquireNextImageKHR(renderer::device, renderer::swapchain, 0, frame->acquired_swapchain_image, nullptr, &frame->acquired_swapchain_image_idx);

    if (VK_SUCCESS != result)
    {
        if (result == VK_TIMEOUT || result == VK_NOT_READY)
        {
            return false;
        }
        if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            return false;
        }
        logger::critical_error(logger::code::vkAcquireNextImageKHR, result);
    }
    return true;
}


static void present_swapchain_image(renderer::frame_t *frame)
{
    VkPresentInfoKHR present_info = {};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &frame->draw_to_swapchain_completed;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &renderer::swapchain;
    present_info.pImageIndices = &frame->acquired_swapchain_image_idx;

    renderer::lock_queue_mutex();
    const VkResult result = vulkan::vkQueuePresentKHR(renderer::default_queue, &present_info);
    renderer::unlock_queue_mutex();

    if (result != VK_SUCCESS &&
        result != VK_SUBOPTIMAL_KHR &&
        result != VK_ERROR_OUT_OF_DATE_KHR)
    {
        logger::critical_error(logger::code::vkQueuePresentKHR, result);
    }
}


static void reset_commands(renderer::frame_t *frame)
{
    vulkan::vkResetCommandPool(renderer::device, frame->default_command_pool, 0);
}


static void wait_for_fence(renderer::frame_t *frame)
{
    VkResult result;

    result = vulkan::vkWaitForFences(renderer::device, 1, &frame->work_completed, VK_TRUE, UINT64_MAX);
    if (result)
    {
        logger::critical_error(logger::code::vkWaitForFences_frame, result);
    }
}


static void reset_fence(renderer::frame_t *frame)
{
    VkResult result;

    result = vulkan::vkResetFences(renderer::device, 1, &frame->work_completed);
    if (result)
    {
        logger::critical_error(logger::code::vkResetFences_frame, result);
    }
}




