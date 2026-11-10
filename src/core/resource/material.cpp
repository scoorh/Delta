#include <core/types.h>


static b8 create(resource::material_t* material, const resource::material::setup_t* setup);


namespace resource::material
{
    material_t* create(const setup_t* setup)
    {
        auto material = find(setup->id);
        if (material)
            return material;


        material = memory::resource_pool::allocate<material_t>(&materials, setup->id);
        if (!material)
        {
            logger::error(logger::code::allocate_from_material_resource_pool, setup->id);
            return nullptr;
        }


        material->color = setup->color;
        material->descriptor_index = (u16) memory::resource_pool::get_index(&materials, material);


        if (!::create(material, setup))
        {
            destroy(material);
            return nullptr;
        }


        return material;
    }


    void destroy(material_t* material)
    {
        memory::resource_pool::free(&materials, material);
    }


    material_t* find(const u16 id)
    {
        return memory::resource_pool::find<material_t>(&materials, id);
    }
}


static b8 create(resource::material_t* material, const resource::material::setup_t* setup)
{
    return true;
}