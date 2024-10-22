#pragma once
#include "../common.h"


#define LOG_FATAL_ENABLED 1
#define LOG_ERROR_ENABLED 1
#define LOG_WARN_ENABLED 1
#define LOG_INFO_ENABLED 1
#define LOG_DEBUG_ENABLED 1
#define LOG_TRACE_ENABLED 1

enum log_level {
    LOG_LEVEL_FATAL = 0,
    LOG_LEVEL_ERROR = 1,
    LOG_LEVEL_WARN = 2,
    LOG_LEVEL_INFO = 3,
    LOG_LEVEL_DEBUG = 4,
    LOG_LEVEL_TRACE = 5,

    _LOG_LEVEL_MAX
};

static const char* level_strings[_LOG_LEVEL_MAX] = {
    "[FATAL]:",
    "[ERROR]:",
    "[WARN]:",
    "[INFO]:",
    "[DEBUG]:",
    "[TRACE]:"
};

void log_output(u8 level, const char* message, ...);

#if LOG_FATAL_ENABLED == 1
#define LOG_FATAL(message, ...) log_output(LOG_LEVEL_FATAL, message, ##__VA_ARGS__);
#else
#define LOG_FATAL(message, ...)
#endif

#if LOG_ERROR_ENABLED == 1
#define LOG_ERROR(message, ...) log_output(LOG_LEVEL_ERROR, message, ##__VA_ARGS__);
#else
#define LOG_ERROR(message, ...)
#endif

#if LOG_WARN_ENABLED == 1
#define LOG_WARN(message, ...) log_output(LOG_LEVEL_WARN, message, ##__VA_ARGS__);
#else
#define LOG_WARN(message, ...)
#endif

#if LOG_INFO_ENABLED == 1
#define LOG_INFO(message, ...) log_output(LOG_LEVEL_INFO, message, ##__VA_ARGS__);
#else
#define LOG_INFO(message, ...)
#endif

#if LOG_DEBUG_ENABLED == 1
#define LOG_DEBUG(message, ...) log_output(LOG_LEVEL_DEBUG, message, ##__VA_ARGS__);
#else
#define LOG_DEBUG(message, ...)
#endif

#if LOG_TRACE_ENABLED == 1
#define LOG_TRACE(message, ...) log_output(LOG_LEVEL_TRACE, message, ##__VA_ARGS__);
#else
#define LOG_TRACE(message, ...)
#endif

#if LOG_MEMORY_ENABLED == 1
#define _LOG_MEMORY(message, ...) log_output(LOG_LEVEL_MEMORY, message, ##__VA_ARGS__);
#else
#define _LOG_MEMORY(message, ...)
#endif
