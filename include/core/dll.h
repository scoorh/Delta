#pragma once


// ==================================================
// dll constants
// ==================================================
namespace dll
{
    constexpr auto vulkan_name = "vulkan-1.dll";
}


// ==================================================
// dll variables
// ==================================================
namespace dll
{
    inline HMODULE vulkan = nullptr;
}


// ==================================================
// dll functions
// ==================================================
namespace dll
{
    void initialize();
}


// ==================================================
// get function macros
// ==================================================
#define get_vulkan_dll_function(f) (PFN_##f)((void*)GetProcAddress(dll::vulkan, #f))