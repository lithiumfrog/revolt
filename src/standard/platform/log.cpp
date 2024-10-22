#include "log.h"

#include <stdio.h> // print
#include <stdarg.h> // va

#include "os.h"

#define MAX_MESSAGE_LENGTH 4096

void log_output(u8 level, const char* in_message, ...) {
    // Buffer to store the formatted message.
    char out_message[MAX_MESSAGE_LENGTH];

    // Start variadic argument processing.
    va_list args;
    va_start(args, in_message);

    // Format the message with the level string.
    s32 offset = snprintf(out_message, sizeof(out_message), "%s", level_strings[level]);

    // Append the formatted in_message
    if ((u64)offset < sizeof(out_message)) {
        vsnprintf(out_message + offset, sizeof(out_message) - offset, in_message, args);
    }

    // End variadic argument processing.
    va_end(args);

    // Platform-specific output.
    os_console_write(out_message, level);

}

void assert_print(const char* expression, const char* message, const char* file, int line) {
    log_output(LOG_LEVEL_FATAL, "Assertion Failure: %s, message: '%s', in file: %s, line: %d\n", expression, message, file, line);
}
