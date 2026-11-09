#include <core/types.h>


namespace file::resource_package
{
    b8 open(resource_package_t *f, const c8 *filename)
    {
        if (!base::open(&f->base, filename, "rb"))
        {
            logger::error(logger::code::open_resource_package, filename);
            return false;
        }

        base::read_binary(&f->base, &f->header, sizeof(f->header));

        return true;
    }


    void close(resource_package_t *f)
    {
        base::close(&f->base);
    }


    b8 load_everything(const c8 *filename)
    {
        resource_package_t f;

        if (!open(&f, filename))
            return false;

        const b8 result = load_all_shaders(&f) &&
                          load_all_pipelines(&f) &&
                          load_all_buffers(&f) &&
                          load_all_image_samplers(&f) &&
                          load_all_meshes(&f) &&
                          load_all_images(&f) &&
                          load_all_materials(&f);

        //TODO:  destroy shader modules ?

        close(&f);

        return result;
    }


    b8 load_all_shaders(const resource_package_t *f)
    {
        if (!f->header.shader_count)
            return true;

        b8 result = true;


        const auto setup_array = memory::allocate_from_temporary_block_1<resource::shader::setup_t>(f->header.shader_count);
        auto spirv_code = memory::allocate_from_temporary_block_1<u32>(1);

        base::move_pointer(&f->base, f->header.shader_setup_offset);
        base::read_binary(&f->base, setup_array, sizeof(resource::shader::setup_t) * f->header.shader_count);

        for (u32 i = 0; i < f->header.shader_count; ++i)
        {
            const auto setup = &setup_array[i];

            base::move_pointer(&f->base, setup->package_data_offset);
            base::read_binary(&f->base, spirv_code, setup->code_size);

            if (!resource::shader::create(setup, spirv_code))
                result = false;
        }

        memory::reset_temporary_block_1();

        return result;
    }


    b8 load_all_pipelines(const resource_package_t *f)
    {
        if (!f->header.pipeline_count)
            return true;

        b8 result = true;


        const auto setup_array = memory::allocate_from_temporary_block_1<resource::pipeline::setup_t>(f->header.pipeline_count);
        VkShaderModule shader_module_array[2];

        base::move_pointer(&f->base, f->header.pipeline_setup_offset);
        base::read_binary(&f->base, setup_array, sizeof(resource::pipeline::setup_t) * f->header.pipeline_count);

        for (u32 i = 0; i < f->header.pipeline_count; ++i)
        {
            const auto setup = &setup_array[i];

            if (setup->is_compute)
            {
                shader_module_array[0] = resource::shader::find_module(setup->compute_shader_id);
            }
            else
            {
                shader_module_array[0] = resource::shader::find_module(setup->vertex_shader_id);
                shader_module_array[1] = resource::shader::find_module(setup->fragment_shader_id);
            }

            if (!resource::pipeline::create(setup, shader_module_array))
                result = false;
        }

        memory::reset_temporary_block_1();

        return result;
    }


    b8 load_all_buffers(const resource_package_t *f)
    {
        if (!f->header.buffer_count)
            return true;


        return true;
    }


    b8 load_all_meshes(const resource_package_t *f)
    {
        if (!f->header.mesh_count)
            return true;


        return true;
    }


    b8 load_all_images(const resource_package_t *f)
    {
        if (!f->header.image_count)
            return true;


        return true;
    }


    b8 load_all_image_samplers(const resource_package_t *f)
    {
        if (!f->header.image_sampler_count)
            return true;


        return true;
    }


    b8 load_all_materials(const resource_package_t *f)
    {
        if (!f->header.material_count)
            return true;


        return true;
    }
}
