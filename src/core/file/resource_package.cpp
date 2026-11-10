#include <core/types.h>


static b8 send_mesh_to_gpu(resource::mesh_t *mesh, const resource::mesh::setup_t *setup, const file::base_t *file);
static b8 send_materials_to_gpu(resource::material_t **materials, u32 material_count);


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

        b8 result = true;


        const auto setup_array = memory::allocate_from_temporary_block_1<resource::buffer::setup_t>(f->header.buffer_count);

        base::move_pointer(&f->base, f->header.buffer_setup_offset);
        base::read_binary(&f->base, setup_array, sizeof(resource::buffer::setup_t) * f->header.buffer_count);

        for (u32 i = 0; i < f->header.buffer_count; ++i)
        {
            const auto setup = &setup_array[i];

            if (!resource::buffer::create(setup))
                result = false;
        }

        memory::reset_temporary_block_1();

        return result;
    }


    b8 load_all_meshes(const resource_package_t *f)
    {
        if (!f->header.mesh_count)
            return true;

        b8 result = true;


        const auto setup_array = memory::allocate_from_temporary_block_1<resource::mesh::setup_t>(f->header.mesh_count);

        base::move_pointer(&f->base, f->header.mesh_setup_offset);
        base::read_binary(&f->base, setup_array, sizeof(resource::mesh::setup_t) * f->header.mesh_count);

        for (u32 i = 0; i < f->header.mesh_count; ++i)
        {
            const auto setup = &setup_array[i];
            const auto mesh = resource::mesh::create(setup);

            if (!mesh)
            {
                result = false;
                continue;
            }

            if (!send_mesh_to_gpu(mesh, setup, &f->base))
            {
                result = false;
                resource::mesh::destroy(mesh);
            }
        }

        memory::reset_temporary_block_1();

        return result;
    }


    b8 load_all_images(const resource_package_t *f)
    {
        if (!f->header.image_count)
            return true;

        b8 result = true;


        const auto setup_array = memory::allocate_from_temporary_block_1<resource::image::setup_t>(f->header.image_count);

        base::move_pointer(&f->base, f->header.image_setup_offset);
        base::read_binary(&f->base, setup_array, sizeof(resource::image::setup_t) * f->header.image_count);

        for (u32 i = 0; i < f->header.image_count; ++i)
        {
            const auto setup = &setup_array[i];

            if (!resource::image::create(setup))
                result = false;
        }

        memory::reset_temporary_block_1();

        return result;
    }


    b8 load_all_image_samplers(const resource_package_t *f)
    {
        if (!f->header.image_sampler_count)
            return true;

        b8 result = true;


        const auto setup_array = memory::allocate_from_temporary_block_1<resource::image_sampler::setup_t>(f->header.image_sampler_count);

        base::move_pointer(&f->base, f->header.image_sampler_setup_offset);
        base::read_binary(&f->base, setup_array, sizeof(resource::image_sampler::setup_t) * f->header.image_sampler_count);

        for (u32 i = 0; i < f->header.image_sampler_count; ++i)
        {
            const auto setup = &setup_array[i];

            if (!resource::image_sampler::create(setup))
                result = false;
        }

        memory::reset_temporary_block_1();

        return result;
    }


    b8 load_all_materials(const resource_package_t *f)
    {
        if (!f->header.material_count)
            return true;

        b8 result = true;


        const auto setup_array = memory::allocate_from_temporary_block_1<resource::material::setup_t>(f->header.material_count);
        const auto created_materials = memory::allocate_from_temporary_block_1<resource::material_t *>(f->header.material_count);
        u32 created_material_count = 0;

        base::move_pointer(&f->base, f->header.material_setup_offset);
        base::read_binary(&f->base, setup_array, sizeof(resource::material::setup_t) * f->header.material_count);

        for (u32 i = 0; i < f->header.material_count; ++i)
        {
            const auto setup = &setup_array[i];
            const auto material = resource::material::create(setup);

            if (!material)
            {
                result = false;
                continue;
            }

            created_materials[created_material_count] = material;
            ++created_material_count;
        }

        if (!send_materials_to_gpu(created_materials, created_material_count))
        {
            result = false;
            for (u32 i = 0; i < created_material_count; ++i)
            {
                resource::material::destroy(created_materials[i]);
            }
        }

        memory::reset_temporary_block_1();

        return result;
    }
}


static b8 send_mesh_to_gpu(resource::mesh_t *mesh, const resource::mesh::setup_t *setup, const file::base_t *file)
{
    b8 result = false;
    resource::gpu_transfer_work_t::cmd_t cmd = {};


    auto gpu_transfer_work = resource::gpu_transfer_work::book();
    if (!gpu_transfer_work)
    {
        return false;
    }
    if (!resource::gpu_transfer_work::begin(gpu_transfer_work))
    {
        goto end;
    }


    file::base::move_pointer(file, setup->package_data_offset);

    cmd.buffer.dst = mesh->position_vertex_buffer;
    cmd.buffer.dst_offset = mesh->position_vertex_buffer_offset;
    cmd.buffer.src_offset = 0;
    cmd.buffer.src_size = setup->vertex_count * setup->position_vertex_size;
    cmd.memory_barrier.access_mask = VK_ACCESS_2_SHADER_STORAGE_READ_BIT;
    cmd.memory_barrier.stage_mask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT;
    resource::gpu_transfer_work::send_buffer_cmd(gpu_transfer_work, &cmd);
    file::base::read_binary(file, gpu_transfer_work->src_memory + cmd.buffer.src_offset, cmd.buffer.src_size);

    cmd.buffer.dst = mesh->attribute_vertex_buffer;
    cmd.buffer.dst_offset = mesh->attribute_vertex_buffer_offset;
    cmd.buffer.src_offset += cmd.buffer.src_size;
    cmd.buffer.src_size = setup->vertex_count * setup->attribute_vertex_size;
    cmd.memory_barrier.access_mask = VK_ACCESS_2_SHADER_STORAGE_READ_BIT;
    cmd.memory_barrier.stage_mask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT;
    resource::gpu_transfer_work::send_buffer_cmd(gpu_transfer_work, &cmd);
    file::base::read_binary(file, gpu_transfer_work->src_memory + cmd.buffer.src_offset, cmd.buffer.src_size);

    cmd.buffer.dst = mesh->index_buffer;
    cmd.buffer.dst_offset = mesh->index_buffer_offset;
    cmd.buffer.src_offset += cmd.buffer.src_size;
    cmd.buffer.src_size = setup->index_count * setup->index_size;
    cmd.memory_barrier.access_mask = VK_ACCESS_2_INDEX_READ_BIT;
    cmd.memory_barrier.stage_mask = VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT;
    resource::gpu_transfer_work::send_buffer_cmd(gpu_transfer_work, &cmd);
    file::base::read_binary(file, gpu_transfer_work->src_memory + cmd.buffer.src_offset, cmd.buffer.src_size);


    if (!resource::gpu_transfer_work::submit(gpu_transfer_work, nullptr))
    {
        goto end;
    }

    result = true;

end:
    resource::gpu_transfer_work::unbook(gpu_transfer_work);
    return result;
}


static b8 send_materials_to_gpu(resource::material_t **materials, const u32 material_count)
{
    b8 result = false;
    resource::gpu_transfer_work_t::cmd_t cmd = {};
    resource::shader::material_t *src;


    auto gpu_transfer_work = resource::gpu_transfer_work::book();
    if (!gpu_transfer_work)
    {
        return false;
    }
    if (!resource::gpu_transfer_work::begin(gpu_transfer_work))
    {
        goto end;
    }


    cmd.memory_barrier.access_mask = VK_ACCESS_2_SHADER_STORAGE_READ_BIT;
    cmd.memory_barrier.stage_mask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
    cmd.buffer.src_size = sizeof(resource::shader::material_t);

    for (u32 f = 0; f < renderer::frames_in_flight; ++f)
    {
        cmd.buffer.dst = renderer::frames[f].data_buffer;

        for (u32 m = 0; m < material_count; ++m)
        {
            src = resource::gpu_transfer_work::memory_pointer<resource::shader::material_t>(gpu_transfer_work);
            src->color = materials[m]->color;

            cmd.buffer.dst_offset = renderer::frames[f].data_offset.materials + materials[m]->descriptor_index * sizeof(resource::shader::material_t);
            cmd.buffer.src_offset = m * sizeof(resource::shader::material_t);

            resource::gpu_transfer_work::send_buffer_cmd(gpu_transfer_work, &cmd);
        }
    }


    if (!resource::gpu_transfer_work::submit(gpu_transfer_work, nullptr))
    {
        goto end;
    }

    result = true;
end:
    resource::gpu_transfer_work::unbook(gpu_transfer_work);
    return result;
}
