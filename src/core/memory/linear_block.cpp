#include <core/types.h>


namespace memory::linear_block
{
    b8 initialize(linear_block_t* ptr, const u32 size, const PFN_allocate_function allocate_function)
    {
        ptr->used = 0;
        ptr->size = size;
        ptr->block = (c8*)allocate_function(size);
        return true;
    }


    void* allocate(linear_block_t* ptr, const u32 size)
    {
        if (ptr->used + size > ptr->size)
        {
            return nullptr;
        }
        void* r = ptr->block + ptr->used;
        ptr->used += size;
        return r;
    }


    void free(linear_block_t* ptr, const u32 size)
    {
        assert(ptr->used > size);
        ptr->used -= size;
    }


    void reset(linear_block_t* ptr)
    {
        ptr->used = 0;
    }
}