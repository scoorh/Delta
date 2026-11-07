#include <core/types.h>


static void create_handle();
static void get_images();
static void create_views();


namespace renderer
{
    void initialize_swapchain()
    {
        create_handle();
        get_images();
        create_views();
    }
}


static void create_handle()
{
    VkSwapchainCreateInfoKHR swapchain_create_info = {};
    swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_create_info.surface = renderer::window_surface;
    swapchain_create_info.minImageCount = gpu_info::swapchain->min_image_count;
    swapchain_create_info.imageFormat = gpu_info::swapchain->format;
    swapchain_create_info.imageColorSpace = gpu_info::swapchain->color_space;
    swapchain_create_info.imageExtent.width = renderer::window_width;
    swapchain_create_info.imageExtent.height = renderer::window_height;
    swapchain_create_info.imageArrayLayers = 1;
    swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchain_create_info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_create_info.presentMode = VK_PRESENT_MODE_FIFO_KHR;

    if (settings::renderer::vsync) {
        if (gpu_info::swapchain->mailbox_present_mode) {
            swapchain_create_info.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
        }
    }
    else {
        if (gpu_info::swapchain->immediate_present_mode) {
            swapchain_create_info.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        }
    }

    const VkResult result = vulkan::vkCreateSwapchainKHR(renderer::device, &swapchain_create_info, nullptr, &renderer::swapchain);
    if (result) {
        logger::critical_error(logger::code::vkCreateSwapchainKHR, result);
    }
}


static void get_images()
{
    vulkan::vkGetSwapchainImagesKHR(renderer::device, renderer::swapchain, &renderer::swapchain_image_count, nullptr);

    if (renderer::swapchain_image_count > renderer::max_swapchain_image_count) {
        logger::critical_error(logger::code::vkGetSwapchainImagesKHR, 0);
    }

    vulkan::vkGetSwapchainImagesKHR(renderer::device, renderer::swapchain, &renderer::swapchain_image_count, renderer::swapchain_image_array);
}


static void create_views()
{
    VkImageViewCreateInfo image_view_create_info = {};
    image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    image_view_create_info.format = gpu_info::swapchain->format;
    image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    image_view_create_info.subresourceRange.levelCount = 1;
    image_view_create_info.subresourceRange.layerCount = 1;

    for (u32 i = 0; i < renderer::swapchain_image_count; ++i) {
        image_view_create_info.image = renderer::swapchain_image_array[i];

        const auto result = vulkan::vkCreateImageView(renderer::device, &image_view_create_info, nullptr, &renderer::swapchain_image_view_array[i]);
        if (result) {
            logger::critical_error(logger::code::vkCreateImageView_swapchain, result);
        }
    }
}