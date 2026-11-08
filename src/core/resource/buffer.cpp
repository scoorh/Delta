#include <core/types.h>


static b8 create_handle(resource::buffer_t* buffer, const resource::buffer::setup_t* setup);
static b8 allocate_memory(resource::buffer_t *buffer, const resource::buffer::setup_t *setup);


namespace resource::buffer
{
    buffer_t *create(const setup_t *setup)
    {
        buffer_t *buffer = find(setup->id);
        if (buffer)
            return buffer;


        buffer = memory::resource_pool::allocate<buffer_t>(&buffers, setup->id);
        if (!buffer)
        {
            logger::error(logger::code::allocate_from_buffer_resource_pool, setup->id);
            return nullptr;
        }


        buffer->used = 0;
        buffer->handle = nullptr;
        buffer->allocation.gpu_block = nullptr;
        buffer->usage = setup->usage;
        buffer->descriptor_index = (u16) memory::resource_pool::get_index(&buffers, buffer);


        if (!create_handle(buffer, setup) ||
            !allocate_memory(buffer, setup))
        {
            destroy(buffer);
            return nullptr;
        }


        return buffer;
    }


    void destroy(buffer_t *buffer)
    {
        if (buffer->handle)
        {
            vulkan::vkDestroyBuffer(renderer::device, buffer->handle, nullptr);
        }
        if (buffer->allocation.gpu_block)
        {
            memory::free(&buffer->allocation);
        }
        memory::resource_pool::free(&buffers, buffer);
    }


    buffer_t *find(const u16 id)
    {
        return memory::resource_pool::find<buffer_t>(&buffers, id);
    }
}


static b8 create_handle(resource::buffer_t* buffer, const resource::buffer::setup_t* setup)
{
    VkBufferCreateInfo buffer_create_info = {};
    buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_create_info.size = setup->size;
    buffer_create_info.usage = setup->usage;

    VkResult result = vulkan::vkCreateBuffer(renderer::device, &buffer_create_info, nullptr, &buffer->handle);
    if (result) {
        logger::error(logger::code::vkCreateBuffer, result);
        return false;
    }
    return true;
}


static b8 allocate_memory(resource::buffer_t *buffer, const resource::buffer::setup_t *setup)
{
    VkMemoryRequirements requirements;
    vulkan::vkGetBufferMemoryRequirements(renderer::device, buffer->handle, &requirements);

    if (!memory::allocate_from_gpu(&requirements, &buffer->allocation, setup->gpu_local_memory))
    {
        return false;
    }
    vulkan::vkBindBufferMemory(renderer::device, buffer->handle, buffer->allocation.gpu_block, buffer->allocation.offset);

    return true;
}