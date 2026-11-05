#pragma once


// ==================================================
// settings
// ==================================================
namespace settings
{
    // ==================================================
    // application settings
    // ==================================================
    namespace application
    {
        inline c16 name_c16[32] = {};
        inline c8 name[32] = {};

        inline union
        {
            u32 packed;

            struct
            {
                u8 major;
                u8 minor;
                u8 patch;
                u8 revision;
            };
        } version = {};
    }


    // ==================================================
    // window settings
    // ==================================================
    namespace window
    {
        constexpr u16 default_x_position = 0xffff;
        constexpr u16 default_y_position = 0xffff;

        inline u16 width = 0;
        inline u16 height = 0;
        inline u16 x_position = 0;
        inline u16 y_position = 0;
        inline b8 fullscreen_mode = false;
    }
}


// ==================================================
// settings functions
// ==================================================
namespace settings
{
    void initialize();
}