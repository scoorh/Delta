#include <core/types.h>


namespace memory
{
    void initialize()
    {
        if (!linear_block::initialize(&temporary_block_1, settings::memory::temporary_block_1_size))
        {
            logger::critical_error(logger::code::initialize_temporary_memory_block_1, settings::memory::temporary_block_1_size);
        }
        if (!linear_block::initialize(&temporary_block_2, settings::memory::temporary_block_2_size))
        {
            logger::critical_error(logger::code::initialize_temporary_memory_block_2, settings::memory::temporary_block_2_size);
        }
        if (!linear_block::initialize(&core_block, settings::memory::core_block_size))
        {
            logger::critical_error(logger::code::initialize_core_memory_block, settings::memory::core_block_size);
        }
    }


    void* allocate_from_temporary_block_1(const u32 size)
    {
        return linear_block::allocate(&temporary_block_1, size);
    }


    void* allocate_from_temporary_block_2(const u32 size)
    {
        return linear_block::allocate(&temporary_block_2, size);
    }


    void* allocate_from_core_block(const u32 size)
    {
        return linear_block::allocate(&core_block, size);
    }


    void reset_temporary_block_1()
    {
        linear_block::reset(&temporary_block_1);
    }


    void reset_temporary_block_2()
    {
        linear_block::reset(&temporary_block_2);
    }
}