#pragma once

#include "../common.h"




// memory
#define MIN_ALIGNMENT 64
#define POOL_HEADER_SIZE_PLUS_PAD_BYTES ((sizeof(memory_pool_st) + MIN_ALIGNMENT - 1) & ~(MIN_ALIGNMENT - 1))
#define ALLOCATION_HEADER_SIZE_PLUS_PAD_BYTES ((sizeof(memory_pool_allocation_header_st) + MIN_ALIGNMENT - 1) & ~(MIN_ALIGNMENT - 1))
#define ALLOCATION_FOOTER_SIZE_PLUS_PAD_BYTES ((sizeof(memory_pool_allocation_footer_st) + MIN_ALIGNMENT - 1) & ~(MIN_ALIGNMENT - 1))

enum memory_tag_e : u64 {
    MEMORY_TAG_FREE_AND_ZERO,
    MEMORY_TAG_FREE_AND_NOT_ZERO,
    MEMORY_TAG_UNTAGGED,
    MEMORY_TAG_PLATFORM,
    MEMORY_TAG_WINDOWING,
    MEMORY_TAG_UI,
    MEMORY_TAG_SOFTWARE_RENDERER,
    MEMORY_TAG_EVENTS,
    MEMORY_TAG_INPUT,

    _MEMORY_TAG_MAX_TAGS
};

static const char* memory_tag_strings[_MEMORY_TAG_MAX_TAGS] = {
    "FREE AND ZERO     ",
    "FREE AND NOT ZERO ",
    "UNTAGGED          ",
    "PLATFORM          ",
    "WINDOWING         ",
    "UI                ",
    "SOFTWARE RENDERER ",
    "EVENTS            ",
    "INPUT             "
};

struct memory_pool_allocation_header_st {
    u64 payload_size;
    memory_tag_e memory_tag;
    struct memory_pool_allocation_header_st* previous;
    struct memory_pool_allocation_header_st* next;
    #ifdef STANDARD_DEBUG_FILE_LINE_TRACKING
        const char* file;
        int line;
    #endif
};

struct memory_pool_allocation_footer_st {
    struct memory_pool_allocation_header_st* header;
};

struct memory_pool_st {
    const char* name;
    u64 payload_size;
    struct memory_pool_st* next;
    memory_pool_allocation_header_st* allocation_list;
    memory_pool_allocation_header_st* free_zero_list;
    memory_pool_allocation_header_st* free_not_zero_list;
};

// ui
struct ui_window_st {
    struct ui_context_st* context;
    const char* name;
    b32 visible;
    u64 pos_x;
    u64 pos_y;
    u64 size_x;
    u64 size_y;
};

struct ui_context_st {
    struct platform_window_st* platform_window;
    struct dictionary_linear_st* ui_windows_dictionary;
    u64 window_initial_width_x;
    u64 window_initial_height_y;
    u64 next_window_width_x;
    u64 next_window_height_y;
    ui_window_st* current_window_in_progress;

    u64 hot_id;
    u64 active_id;
    u64 last_active_id;

};

// windowing
struct platform_window_st {
    const char* title;
    u64 current_width;
    u64 current_height;
    u64 new_width;
    u64 new_height;
    b32 is_closing;

    struct os_window_st* os_window;

    ui_context_st ui_context;
};

// state
struct platform_state_st {
    b32 is_running;
    memory_pool_st* platform_pool;
    memory_pool_st* memory_pool_list_head;
    platform_window_st** windows_darray;
};

// platform
STANDARD_RESULT platform_init(platform_state_st** out_platform_state);

// memory
STANDARD_RESULT memory_pool_create(const char* name, u64 size, memory_pool_st** out_memory_pool);
void memory_pool_destroy(memory_pool_st* memory_pool);
void pools_list_print(void);
void* _pool_allocate(u64 size, memory_pool_st* memory_pool, memory_tag_e tag, b32 zero_on_allocation, const char *file, int line);
#define pool_allocate(size, memory_pool, tag, zero_on_allocation) _pool_allocate(size, memory_pool, tag, zero_on_allocation, __FILE__, __LINE__)
void _pool_free(void* block, const char* file, int line);
#define pool_free(block) _pool_free(block, __FILE__, __LINE__);
void pool_zero_allocation(void* block);
void pool_allocation_list_print(memory_pool_st* memory_pool);
void pool_free_zero_list_print(memory_pool_st* memory_pool);
void pool_free_not_zero_list_print(memory_pool_st* memory_pool);

// windowing
STANDARD_RESULT platform_windowing_init();
STANDARD_RESULT platform_window_create(u64 width, u64 height, platform_window_st** out_window);
void platform_window_destroy(platform_window_st* window);
void platform_windowing_pre_render();

// ui
void platform_ui_next_window_initial_size(ui_context_st* context, u64 width, u64 height);
void platform_ui_next_window_size(ui_context_st* context, u64 width, u64 height);

void platform_ui_new_frame(ui_context_st* context);
void platform_ui_window_begin(ui_context_st* context, const char* name);
void platform_ui_window_end(ui_context_st* context);
void platform_ui_render_frame(ui_context_st* context);

b32 platform_ui_widget_button(ui_context_st* context);
