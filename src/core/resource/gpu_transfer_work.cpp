#include <core/types.h>


static void create_fences(u32 work_idx);
static void create_semaphores(u32 work_idx);
static void create_cmd_pools(u32 work_idx);
static void create_cmd_buffers(u32 work_idx);
static void create_staging_buffer(u32 work_idx);


namespace resource::gpu_transfer_work
{
    void initialize()
    {
        for (u32 i = 0; i < gpu_transfer_work_count; ++i)
        {
            create_fences(i);
            create_semaphores(i);
            create_cmd_pools(i);
            create_cmd_buffers(i);
            create_staging_buffer(i);
        }
    }


    gpu_transfer_work_t *book()
    {
        static u32 next_work_idx = renderer::frames_in_flight;
        const u32 start_idx = next_work_idx;

        constexpr u64 wait_for_fence_miliseconds = 100;
        constexpr u32 timeout_loop_idx = 5;
        u32 current_loop_idx = 0;

        while (true)
        {
            auto work = &gpu_transfer_works[next_work_idx];

            if (!work->booked && VK_SUCCESS == vulkan::vkGetFenceStatus(renderer::device, work->all_completed))
            {
                work->booked = true;
                return work;
            }

            next_work_idx = (next_work_idx + 1) % gpu_transfer_work_count;
            if (next_work_idx < renderer::frames_in_flight)
                next_work_idx = renderer::frames_in_flight;

            if (start_idx == next_work_idx)
            {
                if (VK_SUCCESS == vulkan::vkWaitForFences(renderer::device, 1, &work->all_completed, VK_TRUE, math::miliseconds_to_nanoseconds(wait_for_fence_miliseconds)))
                {
                    work->booked = true;
                    return work;
                }
                ++current_loop_idx;

                if (current_loop_idx == timeout_loop_idx)
                    return nullptr;
            }
        }
    }


    void unbook(gpu_transfer_work_t *work)
    {
        work->booked = false;
    }


    b8 begin(gpu_transfer_work_t *work)
    {
        VkResult result = vulkan::vkWaitForFences(renderer::device, 1, &work->all_completed, VK_TRUE, 0);
        if (result)
        {
            logger::error(logger::code::vkWaitForFences_gpu_transfer_work, result);
            return false;
        }
        vulkan::vkResetFences(renderer::device, 1, &work->all_completed);

        work->used_memory = 0;
        vulkan::vkResetCommandPool(renderer::device, work->default_cmd_pool, 0);

        const auto cmd = work->transfer_cmd_buffer;

        VkCommandBufferBeginInfo command_buffer_begin_info = {};
        command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        result = vulkan::vkBeginCommandBuffer(cmd, &command_buffer_begin_info);
        if (result)
        {
            logger::error(logger::code::vkBeginCommandBuffer_gpu_transfer_work, result);
            return false;
        }

        return true;
    }


    b8 submit(gpu_transfer_work_t *work, VkSemaphore wait_for_semaphore)
    {
        const auto cmd = work->transfer_cmd_buffer;

        auto result = vulkan::vkEndCommandBuffer(cmd);
        if (result)
        {
            logger::error(logger::code::vkEndCommandBuffer_gpu_transfer_work, result);
            return false;
        }

        VkSemaphoreSubmitInfo wait_semaphore_info = {};
        VkCommandBufferSubmitInfo cmd_buffer_submit_info = {};
        VkSemaphoreSubmitInfo signal_semaphore_info = {};
        VkSubmitInfo2 submit_info = {};

        wait_semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
        wait_semaphore_info.semaphore = wait_for_semaphore;
        wait_semaphore_info.stageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;

        cmd_buffer_submit_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
        cmd_buffer_submit_info.commandBuffer = cmd;

        signal_semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
        signal_semaphore_info.semaphore = work->transfer_completed;
        signal_semaphore_info.stageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;

        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
        submit_info.waitSemaphoreInfoCount = wait_for_semaphore ? 1 : 0;
        submit_info.pWaitSemaphoreInfos = wait_for_semaphore ? &wait_semaphore_info : nullptr;
        submit_info.commandBufferInfoCount = 1;
        submit_info.pCommandBufferInfos = &cmd_buffer_submit_info;
        submit_info.signalSemaphoreInfoCount = 1;
        submit_info.pSignalSemaphoreInfos = &signal_semaphore_info;

        renderer::lock_queue_mutex();
        result = vulkan::vkQueueSubmit2(renderer::default_queue, 1, &submit_info, work->all_completed);
        renderer::unlock_queue_mutex();

        if (result)
        {
            logger::error(logger::code::vkQueueSubmit2_gpu_transfer_work, result);
            return false;
        }
        return true;
    }


    void send_buffer_cmd(gpu_transfer_work_t *work, const gpu_transfer_work_t::cmd_t *cmd)
    {
        const auto cmd_buffer = work->transfer_cmd_buffer;

        work->used_memory += cmd->buffer.src_size;


        VkBufferCopy2 copy_region = {};
        VkCopyBufferInfo2 buffer_copy_info = {};

        copy_region.sType = VK_STRUCTURE_TYPE_BUFFER_COPY_2;
        copy_region.srcOffset = cmd->buffer.src_offset;
        copy_region.dstOffset = cmd->buffer.dst_offset;
        copy_region.size = cmd->buffer.src_size;

        buffer_copy_info.sType = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2;
        buffer_copy_info.srcBuffer = work->src_buffer;
        buffer_copy_info.dstBuffer = cmd->buffer.dst;
        buffer_copy_info.regionCount = 1;
        buffer_copy_info.pRegions = &copy_region;

        vulkan::vkCmdCopyBuffer2(cmd_buffer, &buffer_copy_info);


        if (cmd->memory_barrier.access_mask != VK_ACCESS_NONE)
        {
            VkBufferMemoryBarrier2 buffer_memory_barrier = {};
            VkDependencyInfo barrier_dependency_info = {};

            buffer_memory_barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2;
            buffer_memory_barrier.srcStageMask = VK_PIPELINE_STAGE_2_COPY_BIT;
            buffer_memory_barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
            buffer_memory_barrier.dstStageMask = cmd->memory_barrier.stage_mask;
            buffer_memory_barrier.dstAccessMask = cmd->memory_barrier.access_mask;
            buffer_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            buffer_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            buffer_memory_barrier.buffer = cmd->buffer.dst;
            buffer_memory_barrier.offset = cmd->buffer.dst_offset;
            buffer_memory_barrier.size = cmd->buffer.src_size;

            barrier_dependency_info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
            barrier_dependency_info.bufferMemoryBarrierCount = 1;
            barrier_dependency_info.pBufferMemoryBarriers = &buffer_memory_barrier;

            vulkan::vkCmdPipelineBarrier2(cmd_buffer, &barrier_dependency_info);
        }
    }


    void send_image_cmd(gpu_transfer_work_t *work, const gpu_transfer_work_t::cmd_t *cmd)
    {
    }


    void read_attachment_cmd(gpu_transfer_work_t *work, const gpu_transfer_work_t::cmd_t *cmd)
    {
    }
}


static void create_fences(const u32 work_idx)
{
    VkFenceCreateInfo fence_create_info = {};
    fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    constexpr u32 fence_count = 1;
    VkFence *dst[fence_count];
    dst[0] = &resource::gpu_transfer_works[work_idx].all_completed;

    for (u32 i = 0; i < fence_count; ++i)
    {
        const auto result = vulkan::vkCreateFence(renderer::device, &fence_create_info, nullptr, dst[i]);
        if (result)
        {
            logger::critical_error(logger::code::vkCreateFence_gpu_transfer_work, result);
        }
    }
}


static void create_semaphores(const u32 work_idx)
{
    VkSemaphoreCreateInfo semaphore_create_info = {};
    semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    constexpr u32 semaphore_count = 1;
    VkSemaphore *dst[semaphore_count];
    dst[0] = &resource::gpu_transfer_works[work_idx].transfer_completed;

    for (u32 i = 0; i < semaphore_count; ++i)
    {
        const auto result = vulkan::vkCreateSemaphore(renderer::device, &semaphore_create_info, nullptr, dst[i]);
        if (result)
        {
            logger::critical_error(logger::code::vkCreateSemaphore_gpu_transfer_work, result);
        }
    }
}


static void create_cmd_pools(const u32 work_idx)
{
    VkCommandPoolCreateInfo pool_create_info = {};
    pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;

    constexpr u32 command_pool_count = 1;
    struct
    {
        u32 queue_family_index;
        VkCommandPool *dst;
    } dst[command_pool_count];

    dst[0].queue_family_index = gpu_info::queue->default_family_idx;
    dst[0].dst = &resource::gpu_transfer_works[work_idx].default_cmd_pool;

    for (u32 i = 0; i < command_pool_count; ++i)
    {
        pool_create_info.queueFamilyIndex = dst[i].queue_family_index;

        const auto result = vulkan::vkCreateCommandPool(renderer::device, &pool_create_info, nullptr, dst[i].dst);
        if (result)
        {
            logger::critical_error(logger::code::vkCreateCommandPool_gpu_transfer_work, result);
        }
    }
}


static void create_cmd_buffers(const u32 work_idx)
{
    VkCommandBufferAllocateInfo buffer_alloc_info = {};
    buffer_alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    buffer_alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    buffer_alloc_info.commandBufferCount = 1;

    constexpr u32 command_buffer_count = 2;
    struct
    {
        VkCommandPool commandPool;
        VkCommandBuffer *dst;
    } dst[command_buffer_count];

    dst[0].commandPool = resource::gpu_transfer_works[work_idx].default_cmd_pool;
    dst[0].dst = &resource::gpu_transfer_works[work_idx].transfer_cmd_buffer;
    dst[1].commandPool = resource::gpu_transfer_works[work_idx].default_cmd_pool;
    dst[1].dst = &resource::gpu_transfer_works[work_idx].readback_cmd_buffer;

    for (u32 i = 0; i < command_buffer_count; ++i)
    {
        buffer_alloc_info.commandPool = dst[i].commandPool;

        const auto result = vulkan::vkAllocateCommandBuffers(renderer::device, &buffer_alloc_info, dst[i].dst);
        if (result)
        {
            logger::critical_error(logger::code::vkAllocateCommandBuffers_gpu_transfer_work, result);
        }
    }
}


static void create_staging_buffer(const u32 work_idx)
{
    const u32 buffer_size = settings::memory::gpu_transfer_work_buffer_size;

    resource::buffer::setup_t setup = {};
    setup.id = resource::gpu_transfer_work_staging_buffer_id + (u16) work_idx;
    setup.size = buffer_size;
    setup.gpu_local_memory = false;
    setup.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

    const auto buffer = resource::buffer::create(&setup);
    if (!buffer)
    {
        logger::critical_error(logger::code::create_gpu_transfer_work_staging_buffer, 0);
    }

    void *ptr;
    const auto result = vulkan::vkMapMemory(renderer::device, buffer->allocation.gpu_block, buffer->allocation.offset, buffer->allocation.size, 0, &ptr);
    if (result)
    {
        logger::critical_error(logger::code::vkMapMemory_gpu_transfer_work_staging_buffer, result);
    }

    resource::gpu_transfer_works[work_idx].src_memory = (c8 *) ptr;
    resource::gpu_transfer_works[work_idx].src_buffer = buffer->handle;
    buffer->used = buffer_size;
}
