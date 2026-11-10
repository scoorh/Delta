#include <core/types.h>


static void create_semaphores(u8 frame_idx);
static void create_fences(u8 frame_idx);
static void create_command_pools(u8 frame_idx);
static void create_command_buffers(u8 frame_idx);
static void create_descriptor_pool(u8 frame_idx);
static void create_descriptor_sets(u8 frame_idx);
static void create_data_buffer(u8 frame_idx);
static void update_descriptor_sets(u8 frame_idx);


namespace renderer
{
    void initialize_frames()
    {
        for (u8 f = 0; f < frames_in_flight; ++f)
        {
            create_semaphores(f);
            create_fences(f);
            create_command_pools(f);
            create_command_buffers(f);
            create_descriptor_pool(f);
            create_descriptor_sets(f);
            create_data_buffer(f);
            update_descriptor_sets(f);

            frames[f].gpu_transfer_work = &resource::gpu_transfer_works[f];
            frames[f].gpu_transfer_work->booked = true;
            frames[f].gpu_transfer_completed = frames[f].gpu_transfer_work->transfer_completed;
        }
    }
}


static void create_fences(const u8 frame_idx)
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


static void create_semaphores(const u8 frame_idx)
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


static void create_command_pools(const u8 frame_idx)
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


static void create_command_buffers(const u8 frame_idx)
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


static void create_descriptor_pool(const u8 frame_idx)
{
    constexpr u32 max_sets = 5;
    constexpr u32 sizes_count = 4;
    VkDescriptorPoolSize sizes[sizes_count];
    sizes[0].type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    sizes[0].descriptorCount = resource::descriptor_set_layout::frame_data::sampled_image_count +
                               resource::descriptor_set_layout::image_data::sampled_image_count +
                               resource::descriptor_set_layout::mesh_cull_data::sampled_image_count +
                               resource::descriptor_set_layout::mesh_data::sampled_image_count +
                               resource::descriptor_set_layout::ui_data::sampled_image_count;
    sizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    sizes[1].descriptorCount = resource::descriptor_set_layout::frame_data::storage_buffer_count +
                               resource::descriptor_set_layout::image_data::storage_buffer_count +
                               resource::descriptor_set_layout::mesh_cull_data::storage_buffer_count +
                               resource::descriptor_set_layout::mesh_data::storage_buffer_count +
                               resource::descriptor_set_layout::ui_data::storage_buffer_count;
    sizes[2].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    sizes[2].descriptorCount = resource::descriptor_set_layout::frame_data::uniform_buffer_count +
                               resource::descriptor_set_layout::image_data::uniform_buffer_count +
                               resource::descriptor_set_layout::mesh_cull_data::uniform_buffer_count +
                               resource::descriptor_set_layout::mesh_data::uniform_buffer_count +
                               resource::descriptor_set_layout::ui_data::uniform_buffer_count;
    sizes[3].type = VK_DESCRIPTOR_TYPE_SAMPLER;
    sizes[3].descriptorCount = resource::descriptor_set_layout::frame_data::sampler_count +
                               resource::descriptor_set_layout::image_data::sampler_count +
                               resource::descriptor_set_layout::mesh_cull_data::sampler_count +
                               resource::descriptor_set_layout::mesh_data::sampler_count +
                               resource::descriptor_set_layout::ui_data::sampler_count;

    VkDescriptorPoolCreateInfo descriptor_pool_create_info = {};
    descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptor_pool_create_info.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
    descriptor_pool_create_info.maxSets = max_sets;
    descriptor_pool_create_info.poolSizeCount = sizes_count;
    descriptor_pool_create_info.pPoolSizes = sizes;

    const auto result = vulkan::vkCreateDescriptorPool(renderer::device, &descriptor_pool_create_info, nullptr, &renderer::frames[frame_idx].descriptor_pool);
    if (result)
    {
        logger::critical_error(logger::code::vkCreateDescriptorPool_frame, result);
    }
}


static void create_descriptor_sets(const u8 frame_idx)
{
    VkDescriptorSetAllocateInfo allocate_info = {};
    allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocate_info.descriptorPool = renderer::frames[frame_idx].descriptor_pool;
    allocate_info.descriptorSetCount = 1;

    constexpr u32 set_count = 5;
    struct
    {
        VkDescriptorSetLayout layout;
        VkDescriptorSet *dst;
    } data[set_count];

    data[0].layout = resource::descriptor_set_layouts.frame_data;
    data[0].dst = &renderer::frames[frame_idx].descriptor_set.frame_data;
    data[1].layout = resource::descriptor_set_layouts.mesh_data;
    data[1].dst = &renderer::frames[frame_idx].descriptor_set.mesh_data;
    data[3].layout = resource::descriptor_set_layouts.image_data;
    data[3].dst = &renderer::frames[frame_idx].descriptor_set.image_data;

    data[2].layout = resource::descriptor_set_layouts.mesh_cull_data;
    data[2].dst = &renderer::frames[frame_idx].descriptor_set.mesh_cull_data;

    data[4].layout = resource::descriptor_set_layouts.ui_data;
    data[4].dst = &renderer::frames[frame_idx].descriptor_set.ui_data;

    for (u32 i = 0; i < set_count; ++i)
    {
        allocate_info.pSetLayouts = &data[i].layout;

        const auto result = vulkan::vkAllocateDescriptorSets(renderer::device, &allocate_info, data[i].dst);
        if (result)
        {
            logger::critical_error(logger::code::vkAllocateDescriptorSets_frame, result);
        }
    }

    renderer::frames[frame_idx].descriptor_set.draw_mesh_array[resource::pipeline_layout::draw_mesh::frame_data_descriptor_idx] = renderer::frames[frame_idx].descriptor_set.frame_data;
    renderer::frames[frame_idx].descriptor_set.draw_mesh_array[resource::pipeline_layout::draw_mesh::mesh_data_descriptor_idx] = renderer::frames[frame_idx].descriptor_set.mesh_data;
    renderer::frames[frame_idx].descriptor_set.draw_mesh_array[resource::pipeline_layout::draw_mesh::image_data_descriptor_idx] = renderer::frames[frame_idx].descriptor_set.image_data;

    renderer::frames[frame_idx].descriptor_set.draw_ui_array[resource::pipeline_layout::draw_ui::frame_data_descriptor_idx] = renderer::frames[frame_idx].descriptor_set.frame_data;
    renderer::frames[frame_idx].descriptor_set.draw_ui_array[resource::pipeline_layout::draw_ui::ui_data_descriptor_idx] = renderer::frames[frame_idx].descriptor_set.ui_data;
    renderer::frames[frame_idx].descriptor_set.draw_ui_array[resource::pipeline_layout::draw_ui::image_data_descriptor_idx] = renderer::frames[frame_idx].descriptor_set.image_data;

    renderer::frames[frame_idx].descriptor_set.cull_mesh_array[resource::pipeline_layout::cull_mesh::frame_data_descriptor_idx] = renderer::frames[frame_idx].descriptor_set.frame_data;
    renderer::frames[frame_idx].descriptor_set.cull_mesh_array[resource::pipeline_layout::cull_mesh::mesh_cull_data_descriptor_idx] = renderer::frames[frame_idx].descriptor_set.mesh_cull_data;
}


static void create_data_buffer(const u8 frame_idx)
{
    constexpr u32 total_offset_count = 7;
    struct _offsets_t
    {
        u32 *dst_offset;
        u32 memory_size;
    } offsets[total_offset_count];


    u32 total_buffer_size = 0;


    constexpr u32 uniform_alignment_count = 1;
    offsets[0].dst_offset = &renderer::frames[frame_idx].data_offset.frame_uniform;
    offsets[0].memory_size = sizeof(resource::shader::frame_uniform_t);

    for (u32 i = 0; i < uniform_alignment_count; ++i)
    {
        total_buffer_size = memory::align_to_alignment(total_buffer_size, gpu_info::limit->min_uniform_buffer_offset_alignment);
        *offsets[i].dst_offset = total_buffer_size;
        total_buffer_size += offsets[i].memory_size;
    }


    offsets[1].dst_offset = &renderer::frames[frame_idx].data_offset.mesh_indirect_draw_cmds_data;
    offsets[1].memory_size = settings::memory::frame_mesh_indirect_draw_cmd_data_buffer_size;
    offsets[2].dst_offset = &renderer::frames[frame_idx].data_offset.mesh_indirect_draw_cmds;
    offsets[2].memory_size = settings::memory::frame_mesh_indirect_draw_cmd_buffer_size;
    offsets[3].dst_offset = &renderer::frames[frame_idx].data_offset.mesh_instance_indices;
    offsets[3].memory_size = settings::memory::frame_mesh_instance_index_buffer_size;
    offsets[4].dst_offset = &renderer::frames[frame_idx].data_offset.mesh_instances;
    offsets[4].memory_size = settings::memory::frame_mesh_instance_buffer_size;
    offsets[5].dst_offset = &renderer::frames[frame_idx].data_offset.materials;
    offsets[5].memory_size = settings::memory::frame_material_buffer_size;
    offsets[6].dst_offset = &renderer::frames[frame_idx].data_offset.ui_instances;
    offsets[6].memory_size = settings::memory::frame_ui_instance_buffer_size;

    for (u32 i = uniform_alignment_count; i < total_offset_count; ++i)
    {
        total_buffer_size = memory::align_to_alignment(total_buffer_size, gpu_info::limit->min_storage_buffer_offset_alignment);
        *offsets[i].dst_offset = total_buffer_size;
        total_buffer_size += offsets[i].memory_size;
    }


    resource::buffer::setup_t buffer_setup = {};
    buffer_setup.id = resource::frame_data_buffer_id + frame_idx;
    buffer_setup.size = total_buffer_size;
    buffer_setup.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    buffer_setup.gpu_local_memory = true;

    const auto buffer = resource::buffer::create(&buffer_setup);
    if (buffer == nullptr)
    {
        logger::critical_error(logger::code::create_frame_data_buffer, 0);
        return;
    }

    renderer::frames[frame_idx].data_buffer = buffer->handle;
    buffer->used = total_buffer_size;
}


static void update_descriptor_sets(const u8 frame_idx)
{
    constexpr uint32_t write_count = 9;
    VkDescriptorBufferInfo descriptor_buffer_info[write_count] = {};
    VkWriteDescriptorSet descriptor_write[write_count] = {};

    for (u32 i = 0; i < write_count; ++i)
    {
        descriptor_buffer_info[i].buffer = renderer::frames[frame_idx].data_buffer;
        descriptor_write[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptor_write[i].pBufferInfo = &descriptor_buffer_info[i];;
    }


    descriptor_buffer_info[0].offset = renderer::frames[frame_idx].data_offset.frame_uniform;
    descriptor_buffer_info[0].range = sizeof(resource::shader::frame_uniform_t);
    descriptor_write[0].dstSet = renderer::frames[frame_idx].descriptor_set.frame_data;
    descriptor_write[0].dstBinding = resource::descriptor_set_layout::frame_data::uniform_binding_idx;
    descriptor_write[0].descriptorCount = 1;
    descriptor_write[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;


    descriptor_buffer_info[1].offset = renderer::frames[frame_idx].data_offset.mesh_indirect_draw_cmds_data;
    descriptor_buffer_info[1].range = settings::memory::frame_mesh_indirect_draw_cmd_data_buffer_size;
    descriptor_write[1].dstSet = renderer::frames[frame_idx].descriptor_set.mesh_cull_data;
    descriptor_write[1].dstBinding = resource::descriptor_set_layout::mesh_cull_data::indirect_draw_cmd_data_binding_idx;
    descriptor_write[1].descriptorCount = 1;
    descriptor_write[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

    descriptor_buffer_info[2].offset = renderer::frames[frame_idx].data_offset.mesh_indirect_draw_cmds;
    descriptor_buffer_info[2].range = settings::memory::frame_mesh_indirect_draw_cmd_buffer_size;
    descriptor_write[2].dstSet = renderer::frames[frame_idx].descriptor_set.mesh_cull_data;
    descriptor_write[2].dstBinding = resource::descriptor_set_layout::mesh_cull_data::indirect_draw_cmd_binding_idx;
    descriptor_write[2].descriptorCount = 1;
    descriptor_write[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

    descriptor_buffer_info[3].offset = renderer::frames[frame_idx].data_offset.mesh_instance_indices;
    descriptor_buffer_info[3].range = settings::memory::frame_mesh_instance_index_buffer_size;
    descriptor_write[3].dstSet = renderer::frames[frame_idx].descriptor_set.mesh_cull_data;
    descriptor_write[3].dstBinding = resource::descriptor_set_layout::mesh_cull_data::instance_indices_binding_idx;
    descriptor_write[3].descriptorCount = 1;
    descriptor_write[3].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

    descriptor_buffer_info[4].offset = renderer::frames[frame_idx].data_offset.mesh_instances;
    descriptor_buffer_info[4].range = settings::memory::frame_mesh_instance_buffer_size;
    descriptor_write[4].dstSet = renderer::frames[frame_idx].descriptor_set.mesh_cull_data;
    descriptor_write[4].dstBinding = resource::descriptor_set_layout::mesh_cull_data::instances_binding_idx;
    descriptor_write[4].descriptorCount = 1;
    descriptor_write[4].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;


    descriptor_buffer_info[5].offset = renderer::frames[frame_idx].data_offset.mesh_instance_indices;
    descriptor_buffer_info[5].range = settings::memory::frame_mesh_instance_index_buffer_size;
    descriptor_write[5].dstSet = renderer::frames[frame_idx].descriptor_set.mesh_data;
    descriptor_write[5].dstBinding = resource::descriptor_set_layout::mesh_data::instance_indices_binding_idx;
    descriptor_write[5].descriptorCount = 1;
    descriptor_write[5].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

    descriptor_buffer_info[6].offset = renderer::frames[frame_idx].data_offset.mesh_instances;
    descriptor_buffer_info[6].range = settings::memory::frame_mesh_instance_buffer_size;
    descriptor_write[6].dstSet = renderer::frames[frame_idx].descriptor_set.mesh_data;
    descriptor_write[6].dstBinding = resource::descriptor_set_layout::mesh_data::instances_binding_idx;
    descriptor_write[6].descriptorCount = 1;
    descriptor_write[6].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

    descriptor_buffer_info[7].offset = renderer::frames[frame_idx].data_offset.materials;
    descriptor_buffer_info[7].range = settings::memory::frame_material_buffer_size;
    descriptor_write[7].dstSet = renderer::frames[frame_idx].descriptor_set.mesh_data;
    descriptor_write[7].dstBinding = resource::descriptor_set_layout::mesh_data::materials_binding_idx;
    descriptor_write[7].descriptorCount = 1;
    descriptor_write[7].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;


    descriptor_buffer_info[8].offset = renderer::frames[frame_idx].data_offset.ui_instances;
    descriptor_buffer_info[8].range = settings::memory::frame_ui_instance_buffer_size;
    descriptor_write[8].dstSet = renderer::frames[frame_idx].descriptor_set.ui_data;
    descriptor_write[8].dstBinding = resource::descriptor_set_layout::ui_data::instances_binding_idx;
    descriptor_write[8].descriptorCount = 1;
    descriptor_write[8].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;


    vulkan::vkUpdateDescriptorSets(renderer::device, write_count, descriptor_write, 0, nullptr);
}
