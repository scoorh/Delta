#include <core/types.h>


static HMODULE load_library(const c8* name);


namespace dll
{
    void initialize()
    {
        vulkan = load_library(vulkan_name);
        if (!vulkan)
        {
            logger::critical_error(logger::code::load_vulkan_dll, 0);
        }
    }
}


HMODULE load_library(const c8* name)
{
    return LoadLibraryA(name);
}