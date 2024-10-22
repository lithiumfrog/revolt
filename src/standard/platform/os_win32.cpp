#if STANDARD_PLATFORM_WINDOWS == 1

#include "os.h"

#include <windows.h>
#include <windowsx.h> // param input extraction
#include <intrin.h> // rdtsc


void os_console_write(const char* message, u8 level)
{
    HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    // levels[6] -- FATAL,ERROR,WARN,INFO,DEBUG,TRACE
    static u8 levels[6] = {64, 4, 6, 2, 1, 8};
    SetConsoleTextAttribute(console_handle, levels[level]);
    OutputDebugStringA(message);
    u64 length = strlen(message);
    LPDWORD number_written = 0;
    WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), message, (DWORD)length, number_written, 0);
    SetConsoleTextAttribute(console_handle, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
}


#endif
