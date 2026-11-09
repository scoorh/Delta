#include <core/types.h>


static void invalidate_pool_data(c8 *data, u32 container_size, u32 data_stride);
static void invalidate_pool_id(c8 *id, u32 container_size, u32 id_stride);


namespace memory::resource_pool
{
    b8 initialize(resource_pool_t *p, const u32 data_stride, const u32 container_size, const PFN_allocate_function allocate_function)
    {
        assert(container_size % 32 == 0);

        p->size = container_size;
        p->data_stride = data_stride;
        p->used = 0;
        p->max_used = 0;
        p->data = (c8 *) allocate_function(data_stride * container_size);
        p->next_free = p->data;

        invalidate_pool_data(p->data, container_size, data_stride);

        return nullptr != p->data;
    }


    b8 initialize(resource_pool_id16_t *p, const u32 data_stride, const u32 container_size, const PFN_allocate_function allocate_function)
    {
        assert(container_size % 32 == 0);

        if (!initialize(&p->_, data_stride, container_size, allocate_function))
        {
            return false;
        }

        p->id = (u16 *) allocate_function(sizeof(u16) * container_size);
        if (!p->id)
        {
            return false;
        }

        invalidate_pool_id((c8 *) p->id, container_size, sizeof(u16));

        return true;
    }


    void *allocate(resource_pool_t *p)
    {
        if (p->next_free == nullptr)
        {
            return nullptr;
        }

        ++p->used;
        if (p->used > p->max_used)
        {
            p->max_used = p->used;
        }

        c8 *r = p->next_free;

        if (p->used < p->size)
        {
            p->next_free = p->data + (p->data_stride * (*((u32 *) r)));
        }
        else
        {
            p->next_free = nullptr;
        }

        return r;
    }


    void *allocate(resource_pool_id16_t *p, const u16 id)
    {
        const auto r = allocate(&p->_);
        if (r)
        {
            const u32 idx = get_index(p, r);
            p->id[idx] = id;
        }
        return r;
    }


    void free(resource_pool_t *p, void *ptr)
    {
        if ((c8 *) ptr < p->data || (c8 *) ptr >= p->data + (p->size * p->data_stride))
        {
            return;
        }

        --p->used;

        if (p->next_free != nullptr)
        {
            *(u32 *) ptr = (u32) ((p->next_free - p->data) / p->data_stride);
            p->next_free = (c8 *) ptr;
        }
        else
        {
            *(u32 *) ptr = p->size;
            p->next_free = (c8 *) ptr;
        }
    }


    void free(resource_pool_id16_t *p, void *ptr)
    {
        if ((c8 *) ptr < p->_.data || (c8 *) ptr >= p->_.data + (p->_.size * p->_.data_stride))
        {
            return;
        }
        const u32 idx = get_index(p, ptr);
        p->id[idx] = 0xffff;
        free(&p->_, ptr);
    }


    u32 get_index(const resource_pool_t *p, const void *ptr)
    {
        return (u32) (((c8 *) ptr - p->data) / p->data_stride);
    }


    u32 get_index(const resource_pool_id16_t *p, const void *ptr)
    {
        return get_index(&p->_, ptr);
    }


    void *find(const resource_pool_id16_t *p, const u16 id)
    {
        const __m256i target = _mm256_set1_epi16(id);

        u32 i = 0;
        for (; i + 16 <= p->_.max_used; i += 16)
        {
            const __m256i v = _mm256_loadu_si256((const __m256i *) (p->id + i));
            const __m256i cmp = _mm256_cmpeq_epi16(v, target);

            const auto mask = (u32) _mm256_movemask_epi8(cmp);

            if (mask)
            {
                return &p->_.data[(i + (__builtin_ctz(mask) >> 1)) * p->_.data_stride];
            }
        }

        for (; i < p->_.max_used; ++i)
        {
            if (p->id[i] == id)
            {
                return p->_.data + (i * p->_.data_stride);
            }
        }

        return nullptr;
    }
}


static void invalidate_pool_data(c8 *data, const u32 container_size, const u32 data_stride)
{
    union
    {
        u32 c[8];
        __m256i sse;
    } values, ptr_index;

    values.sse = _mm256_setr_epi32(1, 2, 3, 4, 5, 6, 7, 8);
    ptr_index.sse = _mm256_setr_epi32(0, data_stride * 1, data_stride * 2, data_stride * 3, data_stride * 4, data_stride * 5, data_stride * 6, data_stride * 7);

    const __m256i value_increment = _mm256_set1_epi32(8);
    const __m256i ptr_increment = _mm256_set1_epi32(data_stride * 8);

    for (u32 i = 0; i + 8 <= container_size; i += 8)
    {
        *(u32 *) &data[ptr_index.c[0]] = values.c[0];
        *(u32 *) &data[ptr_index.c[1]] = values.c[1];
        *(u32 *) &data[ptr_index.c[2]] = values.c[2];
        *(u32 *) &data[ptr_index.c[3]] = values.c[3];
        *(u32 *) &data[ptr_index.c[4]] = values.c[4];
        *(u32 *) &data[ptr_index.c[5]] = values.c[5];
        *(u32 *) &data[ptr_index.c[6]] = values.c[6];
        *(u32 *) &data[ptr_index.c[7]] = values.c[7];
        values.sse = _mm256_add_epi32(values.sse, value_increment);
        ptr_index.sse = _mm256_add_epi32(ptr_index.sse, ptr_increment);
    }
}


static void invalidate_pool_id(c8 *id, const u32 container_size, const u32 id_stride)
{
    const __m256i v = _mm256_set1_epi8((c8) 0xFF);

    for (u32 i = 0; i + 32 <= container_size * id_stride; i += 32)
    {
        _mm256_storeu_si256((__m256i *) (id + i), v);
    }
}
