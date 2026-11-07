#include <core/types.h>


static void create_semaphores(u32 frame_idx);
static void create_fences(u32 frame_idx);
static void create_command_pools(u32 frame_idx);
static void create_command_buffers(u32 frame_idx);


namespace renderer
{
    void initialize_frames()
    {
        for (u32 f = 0; f < frames_in_flight; ++f)
        {
            create_semaphores(f);
            create_fences(f);
            create_command_pools(f);
            create_command_buffers(f);
        }
    }
}


static void create_fences(const u32 frame_idx)
{
    constexpr u32 fence_count = 1;
    VkFence *dst[fence_count];

    dst[0] = &renderer::frames[frame_idx].work_completed;

    VkFenceCreateInfo fence_create_info = {};
    fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (u32 i = 0; i < fence_count; ++i)
    {
        const auto result = vulkan::vkCreateFence(renderer::device, &fence_create_info, nullptr, dst[i]);
        if (result)
        {
            logger::critical_error(logger::code::vkCreateFence_frame, result);
        }
    }
}


static void create_semaphores(const u32 frame_idx)
{
    constexpr u32 semaphore_count = 2;
    VkSemaphore *dst[semaphore_count];

    dst[0] = &renderer::frames[frame_idx].acquired_swapchain_image;
    dst[1] = &renderer::frames[frame_idx].draw_to_swapchain_completed;

    VkSemaphoreCreateInfo semaphore_create_info = {};
    semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    for (u32 i = 0; i < semaphore_count; ++i)
    {
        const auto result = vulkan::vkCreateSemaphore(renderer::device, &semaphore_create_info, nullptr, dst[i]);
        if (result)
        {
            logger::critical_error(logger::code::vkCreateSemaphore_frame, result);
        }
    }
}


static void create_command_pools(const u32 frame_idx)
{
    constexpr u32 command_pool_count = 1;
    struct
    {
        u32 queue_family_index;
        VkCommandPool *dst;
    } dst[command_pool_count];

    dst[0].queue_family_index = gpu_info::queue->default_family_idx;
    dst[0].dst = &renderer::frames[frame_idx].default_command_pool;

    VkCommandPoolCreateInfo pool_create_info = {};
    pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;

    for (u32 i = 0; i < command_pool_count; ++i)
    {
        pool_create_info.queueFamilyIndex = dst[i].queue_family_index;

        const auto result = vulkan::vkCreateCommandPool(renderer::device, &pool_create_info, nullptr, dst[i].dst);
        if (result)
        {
            logger::critical_error(logger::code::vkCreateCommandPool_frame, result);
        }
    }
}


static void create_command_buffers(const u32 frame_idx)
{
    constexpr u32 command_buffer_count = 1;
    struct
    {
        VkCommandPool commandPool;
        VkCommandBuffer *dst;
    } dst[command_buffer_count];

    dst[0].commandPool = renderer::frames[frame_idx].default_command_pool;
    dst[0].dst = &renderer::frames[frame_idx].draw_to_swapchain_cmd_buffer;

    VkCommandBufferAllocateInfo buffer_alloc_info;
    buffer_alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    buffer_alloc_info.pNext = nullptr;
    buffer_alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    buffer_alloc_info.commandBufferCount = command_buffer_count;

    for (u32 i = 0; i < command_buffer_count; ++i)
    {
        buffer_alloc_info.commandPool = dst[i].commandPool;

        const auto result = vulkan::vkAllocateCommandBuffers(renderer::device, &buffer_alloc_info, dst[i].dst);
        if (result)
        {
            logger::critical_error(logger::code::vkAllocateCommandBuffers_frame, result);
        }
    }
}
