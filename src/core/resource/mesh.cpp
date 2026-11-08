#include <core/types.h>


static b8 create(resource::mesh_t *mesh, const resource::mesh::setup_t *setup);


namespace resource::mesh
{
    mesh_t *create(const setup_t *setup)
    {
        mesh_t *mesh = find(setup->id);
        if (mesh)
            return mesh;


        mesh = memory::resource_pool::allocate<mesh_t>(&meshes, setup->id);
        if (!mesh)
        {
            logger::error(logger::code::allocate_from_mesh_resource_pool, setup->id);
            return nullptr;
        }


        if (!::create(mesh, setup))
        {
            destroy(mesh);
            return nullptr;
        }


        return mesh;
    }


    void destroy(mesh_t *mesh)
    {
        memory::resource_pool::free(&meshes, mesh);
    }


    mesh_t *find(const u16 id)
    {
        return memory::resource_pool::find<mesh_t>(&meshes, id);
    }
}


static b8 create(resource::mesh_t *mesh, const resource::mesh::setup_t *setup)
{
    resource::buffer_t *index_buffer = resource::buffer::find(setup->index_buffer_id);
    resource::buffer_t *position_vertex_buffer = resource::buffer::find(setup->position_vertex_buffer_id);
    resource::buffer_t *attribute_vertex_buffer = resource::buffer::find(setup->attribute_vertex_buffer_id);

    if (!index_buffer)
    {
        logger::error(logger::code::missing_index_buffer, setup->index_buffer_id);
        return false;
    }
    if (!position_vertex_buffer)
    {
        logger::error(logger::code::missing_position_vertex_buffer, setup->position_vertex_buffer_id);
        return false;
    }
    if (!attribute_vertex_buffer)
    {
        logger::error(logger::code::missing_attribute_vertex_buffer, setup->attribute_vertex_buffer_id);
        return false;
    }

    mesh->index_buffer = index_buffer->handle;
    mesh->position_vertex_buffer = position_vertex_buffer->handle;
    mesh->attribute_vertex_buffer = attribute_vertex_buffer->handle;
    mesh->index_buffer_offset = index_buffer->used;
    mesh->position_vertex_ssbo_idx = position_vertex_buffer->descriptor_index;
    mesh->attribute_vertex_ssbo_idx = attribute_vertex_buffer->descriptor_index;

    mesh->vertex_count = setup->vertex_count;
    mesh->index_count = setup->index_count;
    mesh->first_index = index_buffer->used / setup->index_size;
    mesh->first_vertex = position_vertex_buffer->used / setup->position_vertex_size;

    if (!resource::buffer::reserve_space(index_buffer, setup->index_size * setup->index_count, &mesh->index_buffer_offset))
    {
        logger::error(logger::code::reserve_index_buffer_space, setup->index_buffer_id);
        return false;
    }
    if (!resource::buffer::reserve_space(position_vertex_buffer, setup->position_vertex_size * setup->vertex_count, &mesh->position_vertex_buffer_offset))
    {
        logger::error(logger::code::reserve_position_vertex_buffer_space, setup->position_vertex_buffer_id);
        return false;
    }
    if (!resource::buffer::reserve_space(attribute_vertex_buffer, setup->attribute_vertex_size * setup->vertex_count, &mesh->attribute_vertex_buffer_offset))
    {
        logger::error(logger::code::reserve_attribute_vertex_buffer_space, setup->attribute_vertex_buffer_id);
        return false;
    }
    return true;
}

