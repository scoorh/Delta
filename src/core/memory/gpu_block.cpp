#include <core/types.h>


static VkDeviceMemory create_handle(u32 size, u32 type);


namespace memory::gpu_block
{
    b8 initialize(gpu_block_t *p, const u32 size, const u32 type)
    {
        p->size = size;
        p->used = 0;
        p->type = type;
        p->block = create_handle(size, type);

        return p->block != nullptr;
    }


    b8 allocate(gpu_block_t* p, const u32 size, const u32 type, const u32 alignment, allocation_t* allocation)
    {
        const u32 alignment_padding = padding_for_alignment(p->used, alignment);

        if (p->type != type ||
            p->used + alignment_padding + size > p->size)
        {
            allocation->gpu_block = nullptr;
            return false;
        }

        allocation->gpu_block = p->block;
        allocation->size = size;
        allocation->offset = p->used + alignment_padding;
        allocation->alignment_padding = alignment_padding;

        p->used += alignment_padding + size;

        return true;
    }
}


static VkDeviceMemory create_handle(const u32 size, const u32 type)
{
    VkMemoryAllocateInfo allocate_info = {};
    allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocate_info.allocationSize = size;
    allocate_info.memoryTypeIndex = type;

    VkDeviceMemory new_block;
    const VkResult result = vulkan::vkAllocateMemory(renderer::device, &allocate_info, nullptr, &new_block);

    if (!new_block)
    {
        logger::error(logger::code::vkAllocateMemory, result);
        return nullptr;
    }
    return new_block;
}
