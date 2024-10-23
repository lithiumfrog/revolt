#pragma once

#include "../common.h"

#include "platform/platform.h"


void os_init(platform_state_st* platform_state);

void os_console_write(const char* message, u8 level);
f64 os_get_absolute_time(void);
void os_wait(f64 ms);
u64 os_rdtsc(void);

STANDARD_RESULT os_memory_allocate(u64 size, u64 debug_base_address_in_TiB, void** out_allocation, u64* out_aligned_size);
void os_memory_free(void* block, u64 length);

STANDARD_RESULT os_windowing_init(void);
STANDARD_RESULT os_window_create(struct platform_window_st* platform_window);
void os_window_destroy(struct platform_window_st* platform_window);

STANDARD_RESULT os_gl_init();
void os_gl_window_setup(struct platform_window_st* window);
void os_gl_context_make_current(struct platform_window_st* window);
void os_gl_clear(struct platform_window_st* window, f32 r, f32 g, f32 b);
void os_gl_swap_buffers(struct platform_window_st* window);

