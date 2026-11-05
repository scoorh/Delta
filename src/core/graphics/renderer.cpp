#include <core/types.h>


namespace renderer
{
    void initialize()
    {
        initialize_vulkan();
        initialize_window();
        gpu_info::enumerate();
    }
}
