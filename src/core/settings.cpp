#include <core/types.h>


namespace settings
{
    void initialize()
    {
        application::version.major = 1;
        application::version.minor = 1;
        application::version.patch = 1;
        application::version.revision = 1;

        strcpy(application::name, "deltaGame");
        wcscpy(application::name_c16, L"deltaGame");
    }
}