#include <core/types.h>


static b8 create_module(resource::shader_t *shader, const resource::shader::setup_t *setup, const u32* spirv_code);


namespace resource::shader
{
    shader_t *create(const setup_t *setup, const u32* spirv_code)
    {
        shader_t *shader = find(setup->id);
        if (shader)
            return shader;


        shader = memory::resource_pool::allocate<shader_t>(&shaders, setup->id);
        if (!shader)
        {
            logger::error(logger::code::allocate_from_shader_resource_pool, setup->id);
            return nullptr;
        }


        shader->handle = nullptr;


        if (!create_module(shader, setup, spirv_code))
        {
            destroy(shader);
            return nullptr;
        }


        return shader;
    }


    void destroy(shader_t *shader)
    {
        if (shader->handle)
        {
            vulkan::vkDestroyShaderModule(renderer::device, shader->handle, nullptr);
        }
        memory::resource_pool::free(&shaders, shader);
    }


    shader_t *find(const u16 id)
    {
        return memory::resource_pool::find<shader_t>(&shaders, id);
    }


    VkShaderModule find_module(const u16 id)
    {
        const shader_t *shader = find(id);

        if (shader)
            return shader->handle;

        return nullptr;
    }
}


static b8 create_module(resource::shader_t *shader, const resource::shader::setup_t *setup, const u32 *spirv_code)
{
    VkShaderModuleCreateInfo module_create_info = {};
    module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    module_create_info.codeSize = setup->code_size;
    module_create_info.pCode = spirv_code;

    const auto result = vulkan::vkCreateShaderModule(renderer::device, &module_create_info, nullptr, &shader->handle);
    if (result)
    {
        logger::error(logger::code::vkCreateShaderModule, result);
        return false;
    }
    return true;
}
