#include <core/types.h>


namespace file::base
{
    b8 open(base_t* f, const c8* filename, const c8* mode)
    {
        f->handle = fopen(filename, mode);
        return f->handle;
    }


    void close(base_t* f)
    {
        fclose(f->handle);
    }


    void move_pointer(const base_t* f, const u32 offset)
    {
        fseek(f->handle, offset, SEEK_SET);
    }


    void read_binary(const base_t* f, void* ptr, const u32 size)
    {
        fread(ptr, size, 1, f->handle);
    }
}