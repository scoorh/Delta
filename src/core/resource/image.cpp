#include <core/types.h>


static b8 create_handle(resource::image_t *image, const resource::image::setup_t *setup);
static b8 allocate_memory(resource::image_t *image, const resource::image::setup_t *setup);
static b8 create_view(resource::image_t *image, const resource::image::setup_t *setup);
static void update_frame_descriptors(const resource::image_t *image);


namespace resource::image
{
    image_t *create(const setup_t *setup)
    {
        image_t *image = find(setup->id);
        if (image)
            return image;


        image = memory::resource_pool::allocate<image_t>(&images, setup->id);
        if (!image)
        {
            logger::error(logger::code::allocate_from_image_resource_pool, setup->id);
            return nullptr;
        }


        image->handle = nullptr;
        image->allocation.gpu_block = nullptr;
        image->view = nullptr;
        image->usage = setup->usage;
        image->descriptor_index = (u16) memory::resource_pool::get_index(&images, image);


        if (!create_handle(image, setup) ||
            !allocate_memory(image, setup) ||
            !create_view(image, setup))
        {
            destroy(image);
            return nullptr;
        }

        update_frame_descriptors(image);

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
            memory::free(&image->allocation);
        }
        memory::resource_pool::free(&images, image);
    }


    image_t *find(const u16 id)
    {
        return memory::resource_pool::find<image_t>(&images, id);
    }


    void layout_transition(VkImageMemoryBarrier2 *o,
                           const VkImage image, const VkImageAspectFlags aspect,
                           const VkImageLayout old_layout, const VkImageLayout new_layout,
                           const VkPipelineStageFlags2 src_stage, const VkAccessFlags2 src_access,
                           const VkPipelineStageFlags2 dst_stage, const VkAccessFlags2 dst_access)
    {
        o->sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
        o->srcStageMask = src_stage;
        o->srcAccessMask = src_access;
        o->dstStageMask = dst_stage;
        o->dstAccessMask = dst_access;
        o->oldLayout = old_layout;
        o->newLayout = new_layout;
        o->srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        o->dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        o->image = image;
        o->subresourceRange.aspectMask = aspect;
        o->subresourceRange.levelCount = 1;
        o->subresourceRange.layerCount = 1;
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


static void update_frame_descriptors(const resource::image_t *image)
{
    if (0 == (image->usage & VK_IMAGE_USAGE_SAMPLED_BIT))
        return;

    constexpr u32 write_info_count = renderer::frames_in_flight;
    VkDescriptorImageInfo image_info[write_info_count] = {};
    VkWriteDescriptorSet write_info[write_info_count] = {};

    for (u32 f = 0; f < write_info_count; ++f)
    {
        write_info[f].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write_info[f].dstSet = renderer::frames[f].descriptor_set.image_data;
        write_info[f].dstBinding = resource::descriptor_set_layout::image_data::sampled_images_binding_idx;
        write_info[f].dstArrayElement = image->descriptor_index;
        write_info[f].descriptorCount = 1;
        write_info[f].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        write_info[f].pImageInfo = &image_info[f];
        image_info[f].imageLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL;
        image_info[f].imageView = image->view;
    }

    vulkan::vkUpdateDescriptorSets(renderer::device, write_info_count, write_info, 0, nullptr);
}