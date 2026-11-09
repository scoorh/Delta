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

        update_frame_descriptors(buffer);

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


    b8 reserve_space(buffer_t* buffer, const u32 size, u32* offset)
    {
        if (buffer->used + size < buffer->allocation.size) {
            *offset = buffer->used;
            buffer->used += size;
            return true;
        }
        return false;
    }


    void reset_to_offset(buffer_t* buffer, const u32 offset)
    {
        buffer->used = offset;
    }


    void update_frame_descriptors(const buffer_t* buffer)
    {
        if (buffer->usage != metadata::resource_package::usage_vertex_storage_buffer)
            return;

        constexpr u32 write_info_count = renderer::frames_in_flight;
        VkDescriptorBufferInfo buffer_info[write_info_count] = {};
        VkWriteDescriptorSet write_info[write_info_count] = {};

        for (u32 f = 0; f < write_info_count; ++f)
        {
            write_info[f].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write_info[f].dstSet = renderer::frames[f].descriptor_set.mesh_data;
            write_info[f].dstBinding = descriptor_set_layout::mesh_data::vertex_ssbo_array_binding_idx;
            write_info[f].dstArrayElement = buffer->descriptor_index;
            write_info[f].descriptorCount = 1;
            write_info[f].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            write_info[f].pBufferInfo = &buffer_info[f];
            buffer_info[f].range = buffer->allocation.size;
            buffer_info[f].buffer = buffer->handle;
        }

        vulkan::vkUpdateDescriptorSets(renderer::device, write_info_count, write_info, 0, nullptr);
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