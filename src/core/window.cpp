#include <core/types.h>


static void get_instance();
static void register_class();
static void create_swapchain_handle();
static void create_vulkan_surface();


namespace window
{
    void initialize()
    {
        get_instance();
        register_class();
        create_swapchain_handle();
        create_vulkan_surface();
    }


    void show()
    {
        ShowWindow(renderer::window_handle, SW_SHOW);
    }


    b8 process_message()
    {
        MSG msg;
        if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                return true;
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        return false;
    }
}


static void get_instance()
{
    renderer::window_instance = GetModuleHandleW(nullptr);
}


static LRESULT main_window_proc(const HWND window_handle, const UINT message, const WPARAM wparam, const LPARAM lparam) {
    switch (message) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_QUIT:
            PostQuitMessage(0);
            return 0;

        default: ;
    }
    return DefWindowProc(window_handle, message, wparam, lparam);
}


static void register_class()
{
    WNDCLASSEXW window_class = {};
    window_class.cbSize = sizeof(WNDCLASSEXA);
    window_class.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
    window_class.lpfnWndProc = main_window_proc;
    window_class.hInstance = renderer::window_instance;
    window_class.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    window_class.lpszClassName = settings::application::name_c16;

    if (!RegisterClassExW(&window_class))
    {
        const u32 last_error = GetLastError();
        if (last_error != ERROR_CLASS_ALREADY_EXISTS)
        {
            logger::critical_error(logger::code::RegisterClassExA, last_error);
        }
    }
}


static void create_swapchain_handle()
{
    u32 ex_style = WS_EX_APPWINDOW;
    u32 style = WS_OVERLAPPEDWINDOW;
    s32 width = settings::window::width;
    s32 height = settings::window::height;
    s32 position_x = settings::window::x_position;
    s32 position_y = settings::window::y_position;

    //TODO:  cap width and height to single monitor

    if (settings::window::fullscreen_mode)
    {
        ex_style = WS_EX_APPWINDOW | WS_EX_TOPMOST;
        style = WS_POPUP;
        width = GetSystemMetrics(SM_CXSCREEN);
        height = GetSystemMetrics(SM_CYSCREEN);
        position_x = 0;
        position_y = 0;
    }
    else
    {
        if (settings::window::x_position == settings::window::default_x_position)
            position_x = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
        if (settings::window::y_position == settings::window::default_y_position)
            position_y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
    }

    RECT window_rect;
    window_rect.left = 0;
    window_rect.right = width;
    window_rect.top = 0;
    window_rect.bottom = height;
    AdjustWindowRectEx(&window_rect, style, FALSE, ex_style);

    const auto adjusted_width = window_rect.right - window_rect.left;
    const auto adjusted_height = window_rect.bottom - window_rect.top;

    renderer::window_handle = CreateWindowExA(
        ex_style,
        settings::application::name, settings::application::name,
        style,
        position_x, position_y,
        adjusted_width, adjusted_height,
        nullptr, nullptr,
        renderer::window_instance, nullptr);

    if (!renderer::window_handle)
    {
        logger::critical_error(logger::code::CreateWindowExA, 0);
    }

    renderer::window_width = (u16)width;
    renderer::window_height = (u16)height;
}


static void create_vulkan_surface()
{
    VkWin32SurfaceCreateInfoKHR create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    create_info.hinstance = renderer::window_instance;
    create_info.hwnd = renderer::window_handle;

    const VkResult result = vulkan::vkCreateWin32SurfaceKHR(renderer::vulkan_instance, &create_info, nullptr, &renderer::window_surface);
    if (result)
    {
        logger::critical_error(logger::code::vkCreateWin32SurfaceKHR, result);
    }
}
