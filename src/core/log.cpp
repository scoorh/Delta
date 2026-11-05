#include <core/types.h>


namespace logger
{
    void error(const code code, const u32 identifier)
    {
        //
        // TODO: ..
        //
        critical_error(code, identifier);
    }

#include <stdio.h>
    void critical_error(const code code, const s32 identifier)
    {
        //
        // TODO: ..
        //
        critical_error(code, (u32)identifier);
    }


    void critical_error(const code code, const u32 identifier)
    {
        //
        // TODO: ..
        //
        c8 message[256];
        snprintf(
            message,
            sizeof(message),
            "Application encountered critical error and cannot continue - error code = %u / %u",
            (u32)code,
            identifier
        );

        MessageBoxA(nullptr, message, "Critical Error", MB_OK | MB_ICONERROR);
        ExitProcess((u32)code);
    }
}