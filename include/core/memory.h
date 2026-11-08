#pragma once


// ==================================================
// memory constants
// ==================================================
namespace memory
{
    constexpr u32 invalid_gpu_memory_type = 0xffffffffu;
    constexpr u32 gpu_block_size = 32 << 20;
}


// ==================================================
// types
// ==================================================
namespace memory
{
    typedef void * (*PFN_allocate_function)(u32 size);


    struct linear_block_t
    {
        u32 used;
        u32 size;
        c8 *block;
    };


    struct gpu_block_t
    {
        u32 used;
        u32 size;
        u32 type;
        VkDeviceMemory block;
    };


    struct resource_pool_t
    {
        u32 size;
        u32 used;
        u32 max_used;
        u32 data_stride;
        c8 *next_free;
        c8 *data;
    };


    struct array_t
    {
        u32 size;
        u32 used;
        u32 data_stride;
        c8 *data;
    };


    struct resource_pool_id16_t
    {
        resource_pool_t _;
        u16 *id;
    };


    struct allocation_t
    {
        union
        {
            c8 *cpu_block;
            VkDeviceMemory gpu_block;
        };

        u32 type;
        u32 offset;
        u32 size;
        u32 alignment_padding;
    };
}


// ==================================================
// array functions
// ==================================================
namespace memory::array
{
    b8 initialize(array_t *p, u32 data_stride, u32 container_size, PFN_allocate_function allocate_function);

    void *allocate(array_t *p);
    void free(array_t* p, u32 idx);
    void *find(const array_t *p, u32 idx);


    template<typename T>
    T *allocate(array_t* p)
    {
        return (T *) allocate(p);
    }


    template<typename T>
    T *find(const array_t *p, const u32 idx)
    {
        return (T *) find(p, idx);
    }
}


// ==================================================
// gpu block functions
// ==================================================
namespace memory::gpu_block
{
    b8 initialize(gpu_block_t *p, u32 size, u32 type);

    b8 allocate(gpu_block_t *p, u32 size, u32 type, u32 alignment, allocation_t *allocation);
}


// ==================================================
// resource pool functions
// ==================================================
namespace memory::resource_pool
{
    b8 initialize(resource_pool_t *p, u32 data_stride, u32 container_size, PFN_allocate_function allocate_function);
    b8 initialize(resource_pool_id16_t *p, u32 data_stride, u32 container_size, PFN_allocate_function allocate_function);

    void *allocate(resource_pool_t *p);
    void *allocate(resource_pool_id16_t *p, u16 id);


    template<typename T>
    T *allocate(resource_pool_t *p)
    {
        return (T *) allocate(p);
    }


    template<typename T>
    T *allocate(resource_pool_id16_t *p, const u16 id)
    {
        return (T *) allocate(p, id);
    }


    void free(resource_pool_t *p, void *ptr);
    void free(resource_pool_id16_t *p, void *ptr);

    u32 get_index(const resource_pool_t *p, const void *ptr);
    u32 get_index(const resource_pool_id16_t *p, const void *ptr);

    void *find(const resource_pool_id16_t *p, u16 id);


    template<typename T>
    T *find(const resource_pool_id16_t *p, const u16 id)
    {
        return (T *) find(p, id);
    }
}


// ==================================================
// linear block functions
// ==================================================
namespace memory::linear_block
{
    b8 initialize(linear_block_t *ptr, u32 size, PFN_allocate_function allocate_function);

    void *allocate(linear_block_t *ptr, u32 size);
    void free(linear_block_t *ptr, u32 size);
    void reset(linear_block_t *ptr);
}


// ==================================================
// memory variables
// ==================================================
namespace memory
{
    inline linear_block_t temporary_block_1 = {};
    inline linear_block_t temporary_block_2 = {};
    inline linear_block_t core_block = {};
    inline array_t gpu_blocks = {};
    inline array_t free_allocations = {};
}


// ==================================================
// memory functions
// ==================================================
namespace memory
{
    void initialize();

    b8 allocate_from_gpu(const VkMemoryRequirements *requirements, allocation_t *allocation, b8 gpu_local_memory);

    void free(const allocation_t *allocation);

    void *allocate_from_heap(u32 size);
    void *allocate_from_temporary_block_1(u32 size);
    void *allocate_from_temporary_block_2(u32 size);
    void *allocate_from_core_block(u32 size);

    void reset_temporary_block_1();
    void reset_temporary_block_2();

    void zero_memory(void *ptr, u32 size);
    void copy_memory(void *dst, const void *src, u32 size);


    template<typename T>
    T *allocate_from_temporary_block_1()
    {
        return (T *) allocate_from_temporary_block_1(sizeof(T));
    }


    template<typename T>
    T *allocate_from_temporary_block_2()
    {
        return (T *) allocate_from_temporary_block_2(sizeof(T));
    }


    template<typename T>
    T *allocate_from_core_block()
    {
        return allocate_from_core_block(sizeof(T));
    }


    template<typename T>
    T *allocate_from_temporary_block_1(const u32 count)
    {
        return (T *) allocate_from_temporary_block_1(sizeof(T) * count);
    }


    template<typename T>
    T *allocate_from_temporary_block_2(const u32 count)
    {
        return (T *) allocate_from_temporary_block_2(sizeof(T) * count);
    }


    template<typename T>
    T *allocate_from_core_block(const u32 count)
    {
        return (T *) allocate_from_core_block(sizeof(T) * count);
    }


    inline u32 padding_for_alignment(const u32 current_size, const u32 alignment)
    {
        return (alignment - (current_size & (alignment - 1))) & (alignment - 1);
    }


    inline u32 align_to_alignment(const u32 current_size, const u32 alignment)
    {
        return (current_size + alignment - 1) & ~(alignment - 1);
    }
}
