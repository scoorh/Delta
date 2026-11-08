#pragma once


// ==================================================
// file types
// ==================================================
namespace file
{
    struct file_t
    {
        FILE* handle;
    };
}


// ==================================================
// file functions
// ==================================================
namespace file
{
    b8 open(file_t* f, const c8* filename, const c8* mode);
    void close(file_t* f);
    void move_pointer(const file_t* f, u32 offset);
    void read_binary(const file_t* f, void* ptr, u32 size);
}