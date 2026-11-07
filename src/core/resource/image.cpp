#include <core/types.h>


static b8 create_handle(resource::image_t *image, const resource::image::setup_t *setup);
static b8 allocate_memory(resource::image_t *image, const resource::image::setup_t *setup);
static b8 create_view(resource::image_t *image, const resource::image::setup_t *setup);


namespace resource::image
{
    image_t *create(const setup_t *setup)
    {
        image_t *image = find(setup->id);
        if (image)
            return image;


        image = memory::resource_pool::allocate<image_t>(&memory::images, setup->id);
        if (!image)
        {
            logger::error(logger::code::allocate_from_image_resource_pool, setup->id);
            return nullptr;
        }


        image->handle = nullptr;
        image->allocation.gpu_block = nullptr;
        image->view = nullptr;
        image->usage = setup->usage;
        image->descriptor_index = (u16) memory::resource_pool::get_index(&memory::images, image);


        if (!create_handle(image, setup) ||
            !allocate_memory(image, setup) ||
            !create_view(image, setup))
        {
            destroy(image);
            return nullptr;
        }


        return image;
    }


    void destroy(image_t *image)
    {
        if (image->view)
        {
            vulkan::vkDestroyImageView(renderer::device, image->view, nullptr);
        }
        if (image->handle)
        {
            vulkan::vkDestroyImage(renderer::device, image->handle, nullptr);
        }
        if (image->allocation.gpu_block)
        {
            memory::free_to_gpu(&image->allocation);
        }
        memory::resource_pool::free(&memory::images, image);
    }


    image_t *find(const u16 id)
    {
        return memory::resource_pool::find<image_t>(&memory::images, id);
    }
}


static b8 create_handle(resource::image_t *image, const resource::image::setup_t *setup)
{
    VkImageCreateInfo image_create_info = {};
    image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_create_info.imageType = VK_IMAGE_TYPE_2D;
    image_create_info.format = setup->format;
    image_create_info.extent.width = setup->width;
    image_create_info.extent.height = setup->height;
    image_create_info.extent.depth = 1;
    image_create_info.mipLevels = 1;
    image_create_info.arrayLayers = 1;
    image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_create_info.usage = setup->usage;

    VkResult result = vulkan::vkCreateImage(renderer::device, &image_create_info, nullptr, &image->handle);
    if (result)
    {
        logger::error(logger::code::vkCreateImage, result);
        return false;
    }

    return true;
}


static b8 allocate_memory(resource::image_t *image, const resource::image::setup_t *setup)
{
    VkMemoryRequirements requirements;
    vulkan::vkGetImageMemoryRequirements(renderer::device, image->handle, &requirements);

    if (!memory::allocate_from_gpu(&requirements, &image->allocation, setup->gpu_local_memory))
    {
        return false;
    }
    vulkan::vkBindImageMemory(renderer::device, image->handle, image->allocation.gpu_block, image->allocation.offset);

    return true;
}


static b8 create_view(resource::image_t *image, const resource::image::setup_t *setup)
{
    VkImageViewCreateInfo view_create_info = {};
    view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_create_info.image = image->handle;
    view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_create_info.format = setup->format;
    view_create_info.subresourceRange.aspectMask = setup->aspect;
    view_create_info.subresourceRange.levelCount = 1;
    view_create_info.subresourceRange.layerCount = 1;

    VkResult result = vulkan::vkCreateImageView(renderer::device, &view_create_info, nullptr, &image->view);
    if (result)
    {
        logger::error(logger::code::vkCreateImageView, result);
        return false;
    }
    return true;
}
