#pragma once


// ==================================================
// log codes
// ==================================================
namespace logger
{
    enum class code : u32
    {
        RegisterClassExA,
        CreateWindowExA,
        vkCreateWin32SurfaceKHR,
        vkCreateInstance,
        vkCreateDebugUtilsMessengerEXT,
        load_vulkan_dll
    };
}


// ==================================================
// log variables
// ==================================================
namespace logger
{

}


// ==================================================
// log functions
// ==================================================
namespace logger
{
    void error(code code, u32 identifier);
    void critical_error(code code, u32 identifier);
    void critical_error(code code, s32 identifier);
}