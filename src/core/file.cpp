#include <core/types.h>


namespace file
{
    b8 open(file_t* f, const c8* filename, const c8* mode)
    {
        f->handle = fopen(filename, mode);
        return f->handle;
    }


    void close(file_t* f)
    {
        fclose(f->handle);
    }


    void move_pointer(const file_t* f, const u32 offset)
    {
        fseek(f->handle, offset, SEEK_SET);
    }


    void read_binary(const file_t* f, void* ptr, const u32 size)
    {
        fread(ptr, size, 1, f->handle);
    }
}