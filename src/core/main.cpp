#include <core/types.h>


int main()
{
    settings::initialize();
    memory::initialize();
    dll::initialize();
    renderer::initialize();

    window::show();

    while (true)
    {
        if (window::process_message())
            break;

        renderer::draw_frame();
    }

    return 0;
}
