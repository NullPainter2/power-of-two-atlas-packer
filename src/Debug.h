#pragma once

namespace Debug
{
    bool _debugPrintEnabled = false;

    void PrintF(const char* fmt, ...)
    {
        if (!_debugPrintEnabled)
        {
            return;
        }

        va_list list;
        va_start(list, fmt);
        vprintf(fmt, list);
        va_end(list);
    }

    void EnableDebugPrint(bool enabled)
    {
        _debugPrintEnabled = enabled;
    }
}
