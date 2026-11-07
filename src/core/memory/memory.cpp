#include <core/types.h>


static u32 get_device_host_local_memory_type(u32 bits);
static u32 get_device_local_memory_type(u32 bits);


namespace memory
{
    void initialize()
    {
        if (!linear_block::initialize(&temporary_block_1, settings::memory::temporary_block_1_size, allocate_from_heap))
        {
            logger::critical_error(logger::code::initialize_temporary_memory_block_1, settings::memory::temporary_block_1_size);
        }
        if (!linear_block::initialize(&temporary_block_2, settings::memory::temporary_block_2_size, allocate_from_heap))
        {
            logger::critical_error(logger::code::initialize_temporary_memory_block_2, settings::memory::temporary_block_2_size);
        }
        if (!linear_block::initialize(&core_block, settings::memory::core_block_size, allocate_from_heap))
        {
            logger::critical_error(logger::code::initialize_core_memory_block, settings::memory::core_block_size);
        }
        if (!resource_pool::initialize(&buffers, sizeof(resource::buffer_t), settings::memory::buffer_resource_pool_size, allocate_from_core_block))
        {
            logger::critical_error(logger::code::initialize_buffer_resource_pool, settings::memory::buffer_resource_pool_size);
        }
        if (!resource_pool::initialize(&images, sizeof(resource::image_t), settings::memory::image_resource_pool_size, allocate_from_core_block))
        {
            logger::critical_error(logger::code::initialize_image_resource_pool, settings::memory::image_resource_pool_size);
        }
        if (!resource_pool::initialize(&image_samplers, sizeof(resource::image_sampler_t), settings::memory::image_sampler_resource_pool_size, allocate_from_core_block))
        {
            logger::critical_error(logger::code::initialize_image_sampler_resource_pool, settings::memory::image_sampler_resource_pool_size);
        }
        if (!resource_pool::initialize(&shaders, sizeof(resource::shader_t), settings::memory::shader_resource_pool_size, allocate_from_core_block))
        {
            logger::critical_error(logger::code::initialize_shader_resource_pool, settings::memory::shader_resource_pool_size);
        }
        if (!resource_pool::initialize(&pipelines, sizeof(resource::pipeline_t), settings::memory::pipeline_resource_pool_size, allocate_from_core_block))
        {
            logger::critical_error(logger::code::initialize_pipeline_resource_pool, settings::memory::pipeline_resource_pool_size);
        }
        if (!array::initialize(&gpu_blocks, sizeof(gpu_block_t), settings::memory::gpu_block_array_size, allocate_from_core_block))
        {
            logger::critical_error(logger::code::initialize_gpu_block_array, settings::memory::gpu_block_array_size);
        }
    }


    b8 allocate_from_gpu(const VkMemoryRequirements *requirements, allocation_t *allocation, const b8 gpu_local_memory)
    {
        const u32 type = gpu_local_memory ? get_device_local_memory_type(requirements->memoryTypeBits) : get_device_host_local_memory_type(requirements->memoryTypeBits);
        if (type == invalid_gpu_memory_type)
        {
            logger::error(logger::code::invalid_gpu_memory_type, requirements->memoryTypeBits);
            return false;
        }


        for (u32 i = 0; i < gpu_blocks.used; ++i)
        {
            auto gpu_block = array::find<gpu_block_t>(&gpu_blocks, i);

            if (gpu_block::allocate(gpu_block, (u32)requirements->size, type, (u32)requirements->alignment, allocation))
            {
                return true;
            }
        }


        auto gpu_block = array::allocate<gpu_block_t>(&gpu_blocks);
        if (!gpu_block)
        {
            logger::error(logger::code::allocate_from_gpu_block_array, gpu_blocks.used);
            return false;
        }


        const u32 block_size = gpu_local_memory ? gpu_block_size : (u32)requirements->size;

        if (!gpu_block::initialize(gpu_block, block_size, type))
        {
            return false;
        }

        gpu_block::allocate(gpu_block, (u32)requirements->size, type, (u32)requirements->alignment, allocation);

        return true;
    }


    void free_to_gpu(allocation_t* allocation)
    {
        //TODO:  implement  free array
    }


    void *allocate_from_heap(const u32 size)
    {
        return malloc(size);
    }


    void *allocate_from_temporary_block_1(const u32 size)
    {
        return linear_block::allocate(&temporary_block_1, size);
    }


    void *allocate_from_temporary_block_2(const u32 size)
    {
        return linear_block::allocate(&temporary_block_2, size);
    }


    void *allocate_from_core_block(const u32 size)
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


    void zero_memory(void *ptr, const u32 size)
    {
        const auto p = (u8 *) ptr;
        const __m256i zero = _mm256_setzero_si256();
        u32 i = 0;

        for (; i + 32 < size; i += 32)
        {
            _mm256_storeu_si256((__m256i *) (p + i), zero);
        }
        for (; i < size; i++)
        {
            p[i] = 0;
        }
    }


    void copy_memory(void* dst, const void* src, u32 size)
    {
        if (size == 0 || dst == src)
            return;

        auto* d = (u8*)dst;
        auto* s = (const u8*)src;

        while (size >= 128)
        {
            __m256i a = _mm256_loadu_si256((const __m256i*)(s +  0));
            __m256i b = _mm256_loadu_si256((const __m256i*)(s + 32));
            __m256i c = _mm256_loadu_si256((const __m256i*)(s + 64));
            __m256i e = _mm256_loadu_si256((const __m256i*)(s + 96));

            _mm256_storeu_si256((__m256i*)(d +  0), a);
            _mm256_storeu_si256((__m256i*)(d + 32), b);
            _mm256_storeu_si256((__m256i*)(d + 64), c);
            _mm256_storeu_si256((__m256i*)(d + 96), e);

            s += 128;
            d += 128;
            size -= 128;
        }

        if (size >= 32)
        {
            __m256 data = _mm256_loadu_ps((const float*)s);
            _mm256_storeu_ps((float*)d, data);
            s += 32;
            d += 32;
            size -= 32;
        }

        if (size >= 16)
        {
            __m128 data = _mm_loadu_ps((const float*)s);
            _mm_storeu_ps((float*)d, data);
            s += 16;
            d += 16;
            size -= 16;
        }

        if (size >= 8)
        {
            *(u64*)d = *(const u64*)s;
            s += 8;
            d += 8;
            size -= 8;
        }

        if (size >= 2)
        {
            *(u16*)d = *(const u16*)s;
            s += 2;
            d += 2;
            size -= 2;
        }

        if (size)
            *d = *s;
    }
}


static u32 get_device_host_local_memory_type(u32 bits)
{
    for (u32 i = 0; i < 32; ++i)
    {
        const u32 cb = (1 << i);
        if (0 == (bits & cb))
        {
            continue;
        }
        if (0 == (gpu_info::memory->host_visible_type_bits & cb) ||
            0 == (gpu_info::memory->host_coherent_type_bits & cb))
        {
            continue;
        }
        return i;
    }
    return memory::invalid_gpu_memory_type;
}


static u32 get_device_local_memory_type(u32 bits)
{
    for (u32 i = 0; i < 32; ++i)
    {
        const u32 cb = (1 << i);
        if (0 == (bits & cb))
        {
            continue;
        }
        if (0 == (gpu_info::memory->device_local_type_bits & cb))
        {
            continue;
        }
        return i;
    }
    return memory::invalid_gpu_memory_type;
}
