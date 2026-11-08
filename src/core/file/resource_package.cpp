#include <core/types.h>


namespace file::resource_package
{
    b8 open(resource_package_t* f, const c8* filename)
    {
        if (!base::open(&f->base, filename, "rb")) {
            logger::error(logger::code::open_resource_package, filename);
            return false;
        }

        base::read_binary(&f->base, &f->header, sizeof(f->header));

        return true;
    }


    void close(resource_package_t* f)
    {
        base::close(&f->base);
    }
}