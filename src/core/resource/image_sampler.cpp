#include <core/types.h>


static b8 create_handle(resource::image_sampler_t *image_sampler, const resource::image_sampler::setup_t *setup);


namespace resource::image_sampler
{
    image_sampler_t *create(const setup_t *setup)
    {
        image_sampler_t *image_sampler = find(setup->id);
        if (image_sampler)
            return image_sampler;


        image_sampler = memory::resource_pool::allocate<image_sampler_t>(&memory::image_samplers, setup->id);
        if (!image_sampler)
        {
            logger::error(logger::code::allocate_from_image_sampler_resource_pool, setup->id);
            return nullptr;
        }


        image_sampler->handle = nullptr;
        image_sampler->descriptor_index = (u16) memory::resource_pool::get_index(&memory::image_samplers, image_sampler);


        if (!create_handle(image_sampler, setup))
        {
            destroy(image_sampler);
            return nullptr;
        }


        return image_sampler;
    }


    void destroy(image_sampler_t *image_sampler)
    {
        if (image_sampler->handle)
        {
            vulkan::vkDestroySampler(renderer::device, image_sampler->handle, nullptr);
        }
        memory::resource_pool::free(&memory::image_samplers, image_sampler);
    }


    image_sampler_t *find(const u16 id)
    {
        return memory::resource_pool::allocate<image_sampler_t>(&memory::image_samplers, id);
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
