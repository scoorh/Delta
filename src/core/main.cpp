#include <core/types.h>


int main()
{
    settings::initialize();
    memory::initialize();
    dll::initialize();
    renderer::initialize();

    file::resource_package::load_everything("data/startup.package");

    window::show();

    while (true)
    {
        if (window::process_message())
            break;

        renderer::draw_frame();
    }

    return 0;
}
