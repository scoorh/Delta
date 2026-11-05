#pragma once


// ==================================================
// renderer variables
// ==================================================
namespace renderer
{
    inline HWND window_handle = nullptr;
    inline HINSTANCE window_instance = nullptr;
    inline VkInstance vulkan_instance = nullptr;
    inline VkSurfaceKHR vulkan_surface = nullptr;

    inline u16 window_width = 0;
    inline u16 window_height = 0;
}


// ==================================================
// renderer functions
// ==================================================
namespace renderer
{
    void initialize();
    void initialize_vulkan();
    void initialize_window();
}