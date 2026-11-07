#include <core/types.h>


static void wait_for_fence(u32 frame_idx);
static void reset_commands(u32 frame_idx);
static b8 acquire_swapchain_image(u32 frame_idx);
static void present_swapchain_image(u32 frame_idx);
static void begin_draw_to_swapchain_commands(u32 frame_idx);
static void submit_draw_to_swapchain_commands(u32 frame_idx);


namespace renderer
{
    void initialize()
    {
        initialize_vulkan();
        window::initialize();
        gpu_info::enumerate();
        initialize_device();
        initialize_swapchain();
        initialize_frames();
    }


    void draw_frame()
    {
        const u32 frame_idx = current_frame % frames_in_flight;

        wait_for_fence(frame_idx);

        if (!acquire_swapchain_image(frame_idx))
            return;

        reset_commands(frame_idx);
        begin_draw_to_swapchain_commands(frame_idx);
        submit_draw_to_swapchain_commands(frame_idx);
        present_swapchain_image(frame_idx);

        ++current_frame;
    }
}


static void begin_draw_to_swapchain_commands(const u32 frame_idx)
{
    const auto cmd = renderer::frames[frame_idx].draw_to_swapchain_cmd_buffer;

    VkCommandBufferBeginInfo command_buffer_begin_info = {};
    command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    auto result = vulkan::vkBeginCommandBuffer(cmd, &command_buffer_begin_info);
    if (result)
    {
        logger::critical_error(logger::code::vkBeginCommandBuffer_draw_to_swapchain, result);
    }


    constexpr u32 attachment_image_barrier_count = 1;
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
    attachment_image_barriers[0].image = renderer::swapchain_image_array[renderer::frames[frame_idx].acquired_swapchain_image_idx];
    attachment_image_barriers[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    attachment_image_barriers[0].subresourceRange.levelCount = 1;
    attachment_image_barriers[0].subresourceRange.layerCount = 1;

    vulkan::vkCmdPipelineBarrier2(cmd, &barrier_dependency_info);


    constexpr u32 color_attachment_count = 1;
    VkRenderingAttachmentInfo color_attachment_info[color_attachment_count] = {};
    color_attachment_info[0].sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    color_attachment_info[0].imageView = renderer::swapchain_image_view_array[renderer::frames[frame_idx].acquired_swapchain_image_idx];
    color_attachment_info[0].imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
    color_attachment_info[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment_info[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment_info[0].clearValue.color.float32[0] = 0.39f;
    color_attachment_info[0].clearValue.color.float32[1] = 0.58f;
    color_attachment_info[0].clearValue.color.float32[2] = 0.93f;
    color_attachment_info[0].clearValue.color.float32[3] = 1.0f;

    VkRenderingInfo rendering_info = {};
    rendering_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    rendering_info.renderArea.extent.width = renderer::window_width;
    rendering_info.renderArea.extent.height = renderer::window_height;
    rendering_info.layerCount = 1;
    rendering_info.colorAttachmentCount = color_attachment_count;
    rendering_info.pColorAttachments = color_attachment_info;

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
}


static void submit_draw_to_swapchain_commands(const u32 frame_idx)
{
    const auto cmd = renderer::frames[frame_idx].draw_to_swapchain_cmd_buffer;

    vulkan::vkCmdEndRendering(cmd);


    constexpr u32 attachment_image_barrier_count = 1;
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
    attachment_image_barriers[0].image = renderer::swapchain_image_array[renderer::frames[frame_idx].acquired_swapchain_image_idx];
    attachment_image_barriers[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    attachment_image_barriers[0].subresourceRange.levelCount = 1;
    attachment_image_barriers[0].subresourceRange.layerCount = 1;

    vulkan::vkCmdPipelineBarrier2(cmd, &barrier_dependency_info);


    auto result = vulkan::vkEndCommandBuffer(cmd);
    if (result)
    {
        logger::critical_error(logger::code::vkEndCommandBuffer_draw_to_swapchain, result);
    }


    constexpr u32 wait_semaphore_info_count = 1;
    VkSemaphoreSubmitInfo wait_semaphore_info[wait_semaphore_info_count] = {};
    VkCommandBufferSubmitInfo cmd_buffer_submit_info = {};
    VkSemaphoreSubmitInfo signal_semaphore_info = {};
    VkSubmitInfo2 submit_info = {};

    wait_semaphore_info[0].sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    wait_semaphore_info[0].semaphore = renderer::frames[frame_idx].acquired_swapchain_image;
    wait_semaphore_info[0].stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;

    cmd_buffer_submit_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    cmd_buffer_submit_info.commandBuffer = cmd;

    signal_semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    signal_semaphore_info.semaphore = renderer::frames[frame_idx].draw_to_swapchain_completed;
    signal_semaphore_info.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;

    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    submit_info.waitSemaphoreInfoCount = wait_semaphore_info_count;
    submit_info.pWaitSemaphoreInfos = wait_semaphore_info;
    submit_info.commandBufferInfoCount = 1;
    submit_info.pCommandBufferInfos = &cmd_buffer_submit_info;
    submit_info.signalSemaphoreInfoCount = 1;
    submit_info.pSignalSemaphoreInfos = &signal_semaphore_info;

    renderer::lock_queue_mutex();
    result = vulkan::vkQueueSubmit2(renderer::default_queue, 1, &submit_info, renderer::frames[frame_idx].work_completed);
    renderer::unlock_queue_mutex();

    if (result)
    {
        logger::critical_error(logger::code::vkQueueSubmit2_draw_to_swapchain, result);
    }
}


static b8 acquire_swapchain_image(const u32 frame_idx)
{
    const auto result = vulkan::vkAcquireNextImageKHR(renderer::device, renderer::swapchain, 0, renderer::frames[frame_idx].acquired_swapchain_image, nullptr, &renderer::frames[frame_idx].acquired_swapchain_image_idx);

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


static void present_swapchain_image(const u32 frame_idx)
{
    VkPresentInfoKHR present_info = {};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &renderer::frames[frame_idx].draw_to_swapchain_completed;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &renderer::swapchain;
    present_info.pImageIndices = &renderer::frames[frame_idx].acquired_swapchain_image_idx;

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


static void reset_commands(const u32 frame_idx)
{
    vulkan::vkResetCommandPool(renderer::device, renderer::frames[frame_idx].default_command_pool, 0);
}


static void wait_for_fence(const u32 frame_idx)
{
    VkResult result;

    result = vulkan::vkWaitForFences(renderer::device, 1, &renderer::frames[frame_idx].work_completed, VK_TRUE, UINT64_MAX);
    if (result)
    {
        logger::critical_error(logger::code::vkWaitForFences_frame, result);
    }

    result = vulkan::vkResetFences(renderer::device, 1, &renderer::frames[frame_idx].work_completed);
    if (result)
    {
        logger::critical_error(logger::code::vkResetFences_frame, result);
    }
}
