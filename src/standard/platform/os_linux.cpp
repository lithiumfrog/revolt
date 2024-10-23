#if STANDARD_PLATFORM_LINUX == 1

#include "os.h"

#include <stdio.h>


void os_console_write(const char* message, u8 level) {
    //                              FATAL,  ERROR,  WARN,   INFO,   DEBUG,  TRACE
    const char* level_strings[] = {"0;41", "1;31", "1;33", "1;32", "1;34", "1;30"};
    printf("\033[%sm%s\033[0m", level_strings[level], message);
}

#endif // STANDARD_PLATFORM_LINUX == 1
