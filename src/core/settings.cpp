#include <core/types.h>


namespace settings
{
    void initialize()
    {
        memory::temporary_block_1_size = 2 << 20;
        memory::temporary_block_2_size = 2 << 20;
        memory::core_block_size = 2 << 20;
        memory::buffer_resource_pool_size = 128;
        memory::image_resource_pool_size = 128;
        memory::image_sampler_resource_pool_size = 128;
        memory::shader_resource_pool_size = 128;
        memory::pipeline_resource_pool_size = 128;
        memory::gpu_block_array_size = 128;
        memory::free_allocation_array_size = 128;

        application::version.major = 1;
        application::version.minor = 1;
        application::version.patch = 1;
        application::version.revision = 1;
        strcpy(application::name, "deltaGame");
        wcscpy(application::name_c16, L"deltaGame");

        renderer::device_id = 0xffffffff;
        renderer::device_vendor_id = 0xffffffff;
        renderer::vsync = true;

        window::x_position = window::default_x_position;
        window::y_position = window::default_y_position;
        window::width = 1280;
        window::height = 720;
        window::fullscreen_mode = false;
    }
}