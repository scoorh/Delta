#include <core/types.h>


static b8 create_handle(resource::image_sampler_t *image_sampler, const resource::image_sampler::setup_t *setup);
static void update_frame_descriptors(const resource::image_sampler_t* image_sampler);


namespace resource::image_sampler
{
    image_sampler_t *create(const setup_t *setup)
    {
        image_sampler_t *image_sampler = find(setup->id);
        if (image_sampler)
            return image_sampler;


        image_sampler = memory::resource_pool::allocate<image_sampler_t>(&image_samplers, setup->id);
        if (!image_sampler)
        {
            logger::error(logger::code::allocate_from_image_sampler_resource_pool, setup->id);
            return nullptr;
        }


        image_sampler->handle = nullptr;
        image_sampler->descriptor_index = (u16) memory::resource_pool::get_index(&image_samplers, image_sampler);


        if (!create_handle(image_sampler, setup))
        {
            destroy(image_sampler);
            return nullptr;
        }

        update_frame_descriptors(image_sampler);

        return image_sampler;
    }


    void destroy(image_sampler_t *image_sampler)
    {
        if (image_sampler->handle)
        {
            vulkan::vkDestroySampler(renderer::device, image_sampler->handle, nullptr);
        }
        memory::resource_pool::free(&image_samplers, image_sampler);
    }


    image_sampler_t *find(const u16 id)
    {
        return memory::resource_pool::find<image_sampler_t>(&image_samplers, id);
    }
}


static b8 create_handle(resource::image_sampler_t *image_sampler, const resource::image_sampler::setup_t *setup)
{
    VkSamplerCreateInfo sampler_create_info = {};
    sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_create_info.magFilter = setup->min_filter;
    sampler_create_info.minFilter = setup->mag_filter;
    sampler_create_info.mipmapMode = setup->mipmap_mode;
    sampler_create_info.addressModeU = setup->address_mode;
    sampler_create_info.addressModeV = setup->address_mode;
    sampler_create_info.addressModeW = setup->address_mode;
    sampler_create_info.maxLod = VK_LOD_CLAMP_NONE;
    sampler_create_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

    VkResult result = vulkan::vkCreateSampler(renderer::device, &sampler_create_info, nullptr, &image_sampler->handle);
    if (result)
    {
        logger::error(logger::code::vkCreateSampler, result);
        return false;
    }
    return true;
}


static void update_frame_descriptors(const resource::image_sampler_t* image_sampler)
{
    constexpr u32 write_info_count = renderer::frames_in_flight;
    VkDescriptorImageInfo image_info[write_info_count] = {};
    VkWriteDescriptorSet write_info[write_info_count] = {};

    for (u32 f = 0; f < write_info_count; ++f)
    {
        write_info[f].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write_info[f].dstSet = renderer::frames[f].descriptor_set.image_data;
        write_info[f].dstBinding = resource::descriptor_set_layout::image_data::samplers_binding_idx;
        write_info[f].dstArrayElement = image_sampler->descriptor_index;
        write_info[f].descriptorCount = 1;
        write_info[f].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
        write_info[f].pImageInfo = &image_info[f];
        image_info[f].sampler = image_sampler->handle;
    }

    vulkan::vkUpdateDescriptorSets(renderer::device, write_info_count, write_info, 0, nullptr);
}