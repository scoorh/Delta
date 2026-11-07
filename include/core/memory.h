#pragma once


// ==================================================
// types
// ==================================================
namespace memory
{
    struct linear_block_t
    {
        u32 used;
        u32 size;
        c8* block;
    };
}


// ==================================================
// linear block functions
// ==================================================
namespace memory::linear_block
{
    b8 initialize(linear_block_t* ptr, u32 size);
    void* allocate(linear_block_t* ptr, u32 size);
    void free(linear_block_t* ptr, u32 size);
    void reset(linear_block_t* ptr);
}


// ==================================================
// memory variables
// ==================================================
namespace memory
{
    inline linear_block_t temporary_block_1 = {};
    inline linear_block_t temporary_block_2 = {};
    inline linear_block_t core_block = {};
}


// ==================================================
// memory functions
// ==================================================
namespace memory
{
    void initialize();

    void* allocate_from_temporary_block_1(u32 size);
    void* allocate_from_temporary_block_2(u32 size);
    void* allocate_from_core_block(u32 size);
    void reset_temporary_block_1();
    void reset_temporary_block_2();

    template<typename T> T* allocate_from_temporary_block_1() { return (T*)allocate_from_temporary_block_1(sizeof(T)); }
    template<typename T> T* allocate_from_temporary_block_2() { return (T*)allocate_from_temporary_block_2(sizeof(T)); }
    template<typename T> T* allocate_from_core_block() { return allocate_from_core_block(sizeof(T)); }
    template<typename T> T* allocate_from_temporary_block_1(const u32 count) { return (T*)allocate_from_temporary_block_1(sizeof(T) * count); }
    template<typename T> T* allocate_from_temporary_block_2(const u32 count) { return (T*)allocate_from_temporary_block_2(sizeof(T) * count); }
    template<typename T> T* allocate_from_core_block(const u32 count) { return (T*)allocate_from_core_block(sizeof(T) * count); }
}