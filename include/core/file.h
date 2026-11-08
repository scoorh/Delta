#pragma once


// ==================================================
// file types
// ==================================================
namespace file
{
    // ==================================================
    // base
    // ==================================================
    struct base_t
    {
        FILE *handle;
    };


    // ==================================================
    // resource package
    // ==================================================
    struct resource_package_t
    {
        base_t base;

        struct
        {
            u16 buffer_count;
            u16 mesh_count;
            u16 image_count;
            u16 image_sampler_count;
            u16 material_count;
            u16 shader_count;
            u16 pipeline_count;
            u16 _pad0;

            u32 buffer_setup_offset;
            u32 mesh_setup_offset;
            u32 image_setup_offset;
            u32 image_sampler_setup_offset;
            u32 material_setup_offset;
            u32 shader_setup_offset;
            u32 pipeline_setup_offset;
        } header;
    };
    static_assert(sizeof(resource_package_t::header) % 4 == 0);
}


// ==================================================
// base functions
// ==================================================
namespace file::base
{
    b8 open(base_t *f, const c8 *filename, const c8 *mode);
    void close(base_t *f);
    void move_pointer(const base_t *f, u32 offset);
    void read_binary(const base_t *f, void *ptr, u32 size);
}


// ==================================================
// resource package functions
// ==================================================
namespace file::resource_package
{

}