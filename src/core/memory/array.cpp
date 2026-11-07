#include <core/types.h>


namespace memory::array
{
    b8 initialize(array_t* p, const u32 data_stride, const u32 container_size, const PFN_allocate_function allocate_function)
    {
        p->used = 0;
        p->data_stride = data_stride;
        p->size = container_size;
        p->data = (c8*)allocate_function(container_size * data_stride);

        return p->data != nullptr;
    }


    void *allocate(array_t* p)
    {
        if (p->used == p->size)
            return nullptr;

        void* r = p->data + p->used * p->data_stride;
        ++p->used;
        return r;
    }


    void *find(const array_t* p, const u32 idx)
    {
        if (idx > p->used)
            return nullptr;

        return p->data + idx * p->data_stride;
    }
}