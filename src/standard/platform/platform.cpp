#include "platform.h"

#include <string.h>

#include "os.h"
#include "log.h"
#include "containers/darray.h"
#include "containers/dictionary_linear.h"
#include "hash/fnv1a.h"

#if STANDARD_DEBUG_BASE_ADDRESS_FOR_ALLOCATIONS == 1
    static u64 memory_debug_address_current = 3u; // starting debug address in TiB
#endif

platform_state_st g_platform_state;


STANDARD_RESULT platform_init(platform_state_st** out_platform_state)
{
    STANDARD_RESULT res;
    res = memory_pool_create("platform pool", MiB(10), &g_platform_state.platform_pool);
    if (res) { return res; }
    *out_platform_state = &g_platform_state;
    return STANDARD_RESULT_SUCCESS;
}


STANDARD_RESULT memory_pool_create(const char* name, u64 size, memory_pool_st** out_memory_pool)
{
    LOG_WARN("TODO: Add pool virtualalloc/mmap growth\n");

    // allocate
    void* memory;
    u64 size_with_metadata = size + ALLOCATION_HEADER_SIZE_PLUS_PAD_BYTES;
    u64 out_aligned_size;

    // allocate call to OS, base address with debug flag
#if STANDARD_DEBUG_BASE_ADDRESS_FOR_ALLOCATIONS == 1
    STANDARD_RESULT res = os_memory_allocate(size_with_metadata, memory_debug_address_current++, &memory, &out_aligned_size);
#else
    STANDARD_RESULT res = os_memory_allocate(size_with_metadata, NULL, &memory, &out_aligned_size);
#endif
    if (res){ return res; }

    // memory_pool_st header
    (*out_memory_pool) = (memory_pool_st*)memory;
    (*out_memory_pool)->name = name;
    (*out_memory_pool)->payload_size = out_aligned_size - ALLOCATION_HEADER_SIZE_PLUS_PAD_BYTES;
    (*out_memory_pool)->next = NULL;
    (*out_memory_pool)->allocation_list = NULL;
    (*out_memory_pool)->free_zero_list = NULL;
    (*out_memory_pool)->free_not_zero_list = NULL;

    void* payload = (void*)((u8*)memory + ALLOCATION_HEADER_SIZE_PLUS_PAD_BYTES);

    // set up initial free chunk header
    memory_pool_allocation_header_st* initial_free_chunk_header = (memory_pool_allocation_header_st*)payload;
    initial_free_chunk_header->memory_tag = MEMORY_TAG_FREE_AND_ZERO;
    initial_free_chunk_header->payload_size = (*out_memory_pool)->payload_size - ALLOCATION_HEADER_SIZE_PLUS_PAD_BYTES - ALLOCATION_FOOTER_SIZE_PLUS_PAD_BYTES;
    initial_free_chunk_header->previous = NULL;
    initial_free_chunk_header->next = NULL;

    // set up initial free chunk footer
    memory_pool_allocation_footer_st* initial_free_chunk_footer = (memory_pool_allocation_footer_st*)((u8*)payload + (*out_memory_pool)->payload_size - ALLOCATION_FOOTER_SIZE_PLUS_PAD_BYTES);
    initial_free_chunk_footer->header = initial_free_chunk_header;

    // set up free list
    (*out_memory_pool)->free_zero_list = initial_free_chunk_header;

    // add to memory pools list
    if (g_platform_state.memory_pool_list_head == NULL) // first pool
    {
        g_platform_state.memory_pool_list_head = *out_memory_pool;
    }
    else // not first pool
    {
        memory_pool_st* current = g_platform_state.memory_pool_list_head;
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = *out_memory_pool;

    }

    return STANDARD_RESULT_SUCCESS;
}

void memory_pool_destroy(memory_pool_st* memory_pool)
{
    STANDARD_ASSERT_MSG(memory_pool, "memory_pool_destroy() attempting to destroy invalid memory pool!");
    STANDARD_ASSERT_MSG(g_platform_state.memory_pool_list_head, "memory_pool_destroy() memory pools list empty!");
    memory_pool_st* current = g_platform_state.memory_pool_list_head;
    while (current->next)
    {
        if (current->next == memory_pool)
        {
            if (memory_pool->next) { current->next = memory_pool->next; } // not end of list, remove from pools list
            else { current->next = NULL; } // end of list, remove from pools list
            u64 length = current->payload_size + ALLOCATION_HEADER_SIZE_PLUS_PAD_BYTES;
            os_memory_free(memory_pool, length);
            return;
        }
        current = current->next; // iterate
    }
    STANDARD_ASSERT_MSG(current->next, "memory_pool_destroy() attempting to destroy invalid memory pool!");
}

void pools_list_print(void)
{
    if (g_platform_state.memory_pool_list_head == NULL) { LOG_INFO("Memory pools list empty!\n"); return; }
    memory_pool_st* current = g_platform_state.memory_pool_list_head;

    LOG_INFO("=================== Pools List ===================\n");
    s32 index = 0;
    while (current != NULL) {
        LOG_INFO("  Pool %d:\n", index);
        LOG_INFO("    Name: %s\n", current->name);
        LOG_INFO("    Address: %p\n", (void*)current);
        LOG_INFO("    Payload Size: %llu bytes\n", current->payload_size);
        LOG_INFO("    Next: %p\n", (void*)current->next);

        current = current->next;
        index++;
    }

}

void* pool_allocations_list_iterate(memory_pool_st* pool, memory_pool_allocation_header_st* list, u64 size_requirement, memory_tag_e tag)
{
    if (list == NULL) { return NULL; }

    memory_pool_allocation_header_st* new_block_current = list;
    while (new_block_current)
    {
        STANDARD_ASSERT_MSG((new_block_current->memory_tag == MEMORY_TAG_FREE_AND_ZERO || new_block_current->memory_tag == MEMORY_TAG_FREE_AND_NOT_ZERO), "Trying to allocate an already allocated block!\n");

        if (new_block_current->payload_size >= size_requirement)
        {
            u64 new_block_total_size = new_block_current->payload_size + ALLOCATION_HEADER_SIZE_PLUS_PAD_BYTES + ALLOCATION_FOOTER_SIZE_PLUS_PAD_BYTES;
            u64 size_requirement_plus_metadata = size_requirement + ALLOCATION_HEADER_SIZE_PLUS_PAD_BYTES + ALLOCATION_FOOTER_SIZE_PLUS_PAD_BYTES;
            u64 remainder_total_size = new_block_total_size - size_requirement_plus_metadata;
            u64 min_block_size = MIN_ALIGNMENT + ALLOCATION_HEADER_SIZE_PLUS_PAD_BYTES + ALLOCATION_FOOTER_SIZE_PLUS_PAD_BYTES;
            // remainder
            if (remainder_total_size >= min_block_size)
            {
                memory_pool_allocation_header_st* previous_block = NULL;
                if (new_block_current->previous) { previous_block = new_block_current->previous; }
                memory_pool_allocation_header_st* next_block = NULL;
                if (new_block_current->next) { next_block = new_block_current->next; }

                // set up new block for remainder
                memory_pool_allocation_header_st* remainder_header = (memory_pool_allocation_header_st*)(((u8*)new_block_current) + ALLOCATION_HEADER_SIZE_PLUS_PAD_BYTES + size_requirement + ALLOCATION_FOOTER_SIZE_PLUS_PAD_BYTES);
                remainder_header->payload_size = remainder_total_size - ALLOCATION_HEADER_SIZE_PLUS_PAD_BYTES - ALLOCATION_FOOTER_SIZE_PLUS_PAD_BYTES;
                if (new_block_current->memory_tag == MEMORY_TAG_FREE_AND_ZERO) { remainder_header->memory_tag = MEMORY_TAG_FREE_AND_ZERO; } // if in free_zero list mark as free_zero
                else { remainder_header->memory_tag = MEMORY_TAG_FREE_AND_NOT_ZERO; } // if in fre not zero list mark as free not zero
                remainder_header->previous = previous_block; // fix free linked list (remainder to previous)
                remainder_header->next = next_block; // fix free linked list (remainder to next)
                if (previous_block) { previous_block->next = remainder_header; } // fix free linked list (prev to remainder)
                if (next_block) { next_block->previous = remainder_header; } // fix free linked list (next to remainder)
                memory_pool_allocation_footer_st* remainder_footer = (memory_pool_allocation_footer_st*)((u8*)remainder_header + remainder_header->payload_size); // new footer
                remainder_footer->header = remainder_header; // set pointer from new footer to new header

                // check if this block is a free list head, if so, name a successor or NULL
                if (pool->free_zero_list == new_block_current) // head block of free zero list
                {
                    if (new_block_current->next)
                    {
                        pool->free_zero_list = new_block_current->next;

                    }
                    else // this is the only block, no next
                    { pool->free_zero_list = remainder_header; }
                }
                if (pool->free_not_zero_list == new_block_current) // head block of free zero list
                {
                    if (new_block_current->next)
                    {
                        pool->free_not_zero_list = new_block_current->next;

                    }
                    else // this is the only block, no next
                    { pool->free_not_zero_list = remainder_header; }
                }
                // set up main block
                new_block_current->previous = NULL; // initialize
                new_block_current->next = NULL; // initialize - going at end of allocations list, next will be NULL anyways
                new_block_current->payload_size = new_block_total_size - ALLOCATION_HEADER_SIZE_PLUS_PAD_BYTES - ALLOCATION_FOOTER_SIZE_PLUS_PAD_BYTES - remainder_total_size; // payload size (original_total_size - header - footer - remainder_total_size)
                new_block_current->memory_tag = tag; // new allocated tag

                memory_pool_allocation_header_st* allocation_list_current = pool->allocation_list;
                if (pool->allocation_list == NULL) { goto rfolist; } // first in list, skip iterating
                while (1)
                {
                    if (allocation_list_current->next == NULL) { goto reolist; } // end of list
                    allocation_list_current = allocation_list_current->next; // iterate linked list
                }
                rfolist:
                pool->allocation_list = new_block_current;
                return (u8*)new_block_current + ALLOCATION_HEADER_SIZE_PLUS_PAD_BYTES;
                reolist:
                allocation_list_current->next = new_block_current; // update end of allocations list with newly allocated block
                new_block_current->previous = allocation_list_current; // update newly allocated block with previous block in allocations list
                return (u8*)new_block_current + ALLOCATION_HEADER_SIZE_PLUS_PAD_BYTES;
            }
            // no remainder, allocate whole block
            else
            {
                memory_pool_allocation_header_st* previous_block = NULL;
                if (new_block_current->previous) { previous_block = new_block_current->previous; }

                memory_pool_allocation_header_st* next_block = NULL;
                if (new_block_current->next) { next_block = new_block_current->next; }

                // update new block info (payload size stays the same)
                new_block_current->next = NULL; // going at end of allocations list, next will be NULL
                new_block_current->memory_tag = tag; // new allocated tag

                // pull new_block_current out of free list
                if (previous_block) { previous_block->next = next_block; } // update previous with new next
                if (next_block) { next_block->previous = previous_block; } // update next with new previous

                // if this block is head of a free list, and there is no remainder, then this block is the entire allocation.
                if (pool->free_zero_list == new_block_current) // head block of free zero list
                {
                    pool->free_zero_list = NULL; // if no remainder, then this block is the whole allocator
                }
                if (pool->free_not_zero_list == new_block_current) // head block of free not zero list
                {
                    pool->free_not_zero_list = NULL; // if no remainder, then this block is the whole allocator
                }

                // add new_block_current to allocations list
                memory_pool_allocation_header_st* allocation_list_current = NULL;
                if (pool->allocation_list == NULL) { goto nrfolist; }
                else { allocation_list_current = pool->allocation_list; }
                while (1)
                {
                    if (allocation_list_current->next == NULL) { goto nreolist; } // end of list
                    allocation_list_current = allocation_list_current->next; // iterate linked list
                }
                nrfolist:
                pool->allocation_list = new_block_current;
                return (u8*)new_block_current + ALLOCATION_HEADER_SIZE_PLUS_PAD_BYTES;
                nreolist:
                allocation_list_current->next = new_block_current; // update end of allocations list with newly allocated block
                new_block_current->previous = allocation_list_current; // update newly allocated block with previous block in allocations list
                return (u8*)new_block_current + ALLOCATION_HEADER_SIZE_PLUS_PAD_BYTES;
            }
        }
        if (new_block_current->next == NULL) { return NULL; }; // no more nodes, failed to allocate
        new_block_current = new_block_current->next; // iterate
    }
    return NULL;
}

void* _pool_allocate(u64 size, memory_pool_st* memory_pool, memory_tag_e tag, b32 zero_on_allocation, const char* file, int line)
{
    LOG_TRACE("UNUSED PARAM _pool_allocate\n", file, line);
    LOG_WARN("TODO: Add file/line to _pool_allocate\n");

    // validate pool
    if (g_platform_state.memory_pool_list_head == NULL) { return NULL; }; // no pools
    memory_pool_st* pool_current = g_platform_state.memory_pool_list_head; // first pool
    while (pool_current != NULL) // validate provided pool
    {
        if (pool_current == memory_pool) { break; } // found
        else if (pool_current->next == NULL) { return NULL; } // not found
        else { pool_current = pool_current->next; }; // iterate
    }

    // align size to 64 bytes
    u64 aligned_size = (size + MIN_ALIGNMENT - 1) & ~(MIN_ALIGNMENT - 1);

    // find suitable block
    if (zero_on_allocation == 1)
    {
        void* allocation = pool_allocations_list_iterate(memory_pool, memory_pool->free_zero_list, aligned_size, tag);
        if (allocation) { return allocation; }
        else
        {
            allocation = pool_allocations_list_iterate(memory_pool, memory_pool->free_not_zero_list, aligned_size, tag);
            if (allocation)
            {
                memset(allocation, 0, aligned_size);
                return allocation;
            }
            else { LOG_FATAL("OUT OF MEMORY - pool: %s\n", memory_pool->name); return NULL; }
        }
    }
    else
    {
        void* allocation = pool_allocations_list_iterate(memory_pool, memory_pool->free_not_zero_list, aligned_size, tag);
        if (allocation) { return allocation; }
        else
        {
            allocation = pool_allocations_list_iterate(memory_pool, memory_pool->free_zero_list, aligned_size, tag);
            if (allocation) { return allocation; }
            else { LOG_FATAL("OUT OF MEMORY - pool: %s\n", memory_pool->name); return NULL; }
        }
    }
}

void _pool_free(void* block, const char* file, int line)
{
    LOG_TRACE("UNUSED PARAM _pool_free\n", file, line);
    LOG_WARN("TODO: Add zero and coalesce function to _pool_free\n");

    STANDARD_ASSERT_MSG(g_platform_state.memory_pool_list_head, "_pool_free() cannot free memory when there are no pools.");

    // find pool
    memory_pool_st* pool_current = g_platform_state.memory_pool_list_head;
    while (pool_current)
    {
        if (block > (u8*)pool_current && block < ((u8*)pool_current + pool_current->payload_size)) { break; }
        if (pool_current->next == NULL) { return; }
        pool_current = pool_current->next;
    }

    // fix old allocation list
    memory_pool_allocation_header_st* block_header = (memory_pool_allocation_header_st*)((u8*)block - ALLOCATION_HEADER_SIZE_PLUS_PAD_BYTES);
    memory_pool_allocation_header_st* previous_in_list = NULL;
    memory_pool_allocation_header_st* next_in_list = NULL;
    b32 first_in_list = true;
    b32 last_in_list = true;
    if (block_header->previous) // get previous
    {
        previous_in_list = block_header->previous;
        first_in_list = false;
    }
    if (block_header->next) // get next
    {
        next_in_list = block_header->next;
        last_in_list = false;
    }
    if (first_in_list) // if head of allocation list
    {
        if (!last_in_list) { pool_current->allocation_list = pool_current->allocation_list->next; } // head has a next, promote next to new head
        else { pool_current->allocation_list = NULL; } // head has no next, this was the only node
    }
    if (!first_in_list) { previous_in_list->next = next_in_list; } // not head, fix previous->next
    if (!last_in_list) { next_in_list->previous = previous_in_list; } // not tail, fix next->previous

    // add to free not zero list
    memory_pool_allocation_header_st* current_free_not_zero_node = NULL;
    if (pool_current->free_not_zero_list) // iterate list
    {
        current_free_not_zero_node = pool_current->free_not_zero_list; // first node
        while (current_free_not_zero_node->next != NULL) { current_free_not_zero_node = current_free_not_zero_node->next; } // find last node
        current_free_not_zero_node->next = block_header; // add to last node ->next
        block_header->previous = current_free_not_zero_node; // prev is last node in list before it was added to end
    }
    else // first in list
    {
        pool_current->free_not_zero_list = block_header;
    }

    // change tag and next
    block_header->memory_tag = MEMORY_TAG_FREE_AND_NOT_ZERO; // mark as free and not zero
    block_header->next = NULL; // always end of allocations list
}

void pool_zero_allocation(void* block)
{
    STANDARD_ASSERT_MSG(g_platform_state.memory_pool_list_head, "_pool_free() cannot free memory when there are no pools.");

    memory_pool_allocation_header_st* block_header = (memory_pool_allocation_header_st*)((u8*)block - ALLOCATION_HEADER_SIZE_PLUS_PAD_BYTES);
    memset(block, 0, block_header->payload_size);
}

void pool_allocation_list_print(memory_pool_st* memory_pool)
{
    if (memory_pool == NULL) { LOG_INFO("Memory pool is NULL.\n"); return; }

    memory_pool_allocation_header_st* current = memory_pool->allocation_list;
    if (current == NULL) { LOG_INFO("Allocation list is empty for memory pool '%s'.\n", memory_pool->name); return; }

    LOG_INFO("Allocation list for memory pool '%s':\n", memory_pool->name);
    s32 index = 0;
    while (current != NULL) {
        LOG_INFO("  Allocated block %d:\n", index);
        LOG_INFO("    Address: %p\n", (void*)current);
        LOG_INFO("    Payload Size: %llu bytes\n", current->payload_size);
        u64 tag = current->memory_tag;
        LOG_INFO("    Memory tag: %s\n", memory_tag_strings[tag]);

        current = current->next;
        index++;
    }
}

void pool_free_zero_list_print(memory_pool_st* memory_pool)
{
    if (memory_pool == NULL) { LOG_INFO("Memory pool is NULL.\n"); return; }

    memory_pool_allocation_header_st* current = memory_pool->free_zero_list;
    if (current == NULL) { LOG_INFO("Free zero list is empty for memory pool '%s'.\n", memory_pool->name); return; }

    LOG_INFO("Free zero list for memory pool '%s':\n", memory_pool->name);
    s32 index = 0;
    while (current != NULL) {
        LOG_INFO("  Free block %d:\n", index);
        LOG_INFO("    Address: %p\n", (void*)current);
        LOG_INFO("    Payload Size: %llu bytes\n", current->payload_size);
        u64 tag = current->memory_tag;
        LOG_INFO("    Memory tag: %s\n", memory_tag_strings[tag]);

        current = current->next;
        index++;
    }
}

void pool_free_not_zero_list_print(memory_pool_st* memory_pool)
{
    if (memory_pool == NULL) { LOG_INFO("Memory pool is NULL.\n"); return; }

    memory_pool_allocation_header_st* current = memory_pool->free_not_zero_list;
    if (current == NULL) { LOG_INFO("Free not zero list is empty for memory pool '%s'.\n", memory_pool->name); return; }

    LOG_INFO("Free not zero list for memory pool '%s':\n", memory_pool->name);
    s32 index = 0;
    while (current != NULL) {
        LOG_INFO("  Free block %d:\n", index);
        LOG_INFO("    Address: %p\n", (void*)current);
        LOG_INFO("    Payload Size: %llu bytes\n", current->payload_size);
        u64 tag = current->memory_tag;
        LOG_INFO("    Memory tag: %s\n", memory_tag_strings[tag]);

        current = current->next;
        index++;
    }
}


// windowing
STANDARD_RESULT platform_windowing_init()
{
    g_platform_state.windows_darray = darray_create<platform_window_st*>(1, g_platform_state.platform_pool, MEMORY_TAG_WINDOWING);

    STANDARD_RESULT res = os_windowing_init();
    if (res) { return res; }

    return STANDARD_RESULT_SUCCESS;
}

STANDARD_RESULT platform_window_create(u64 width, u64 height, platform_window_st** out_window)
{
    platform_window_st* window = (platform_window_st*)pool_allocate(sizeof(platform_window_st), g_platform_state.platform_pool, MEMORY_TAG_WINDOWING, true);
    // os_window_st* os_window = (os_window_st*)pool_allocate(sizeof(os_window_st), platform_state.platform_pool, MEMORY_TAG_WINDOWING, true);
    window->title = "lostling window";
    window->current_width = width;
    window->new_width = width;
    window->current_height = height;
    window->new_height = height;
    // window->os_window = os_window;

    STANDARD_RESULT res = os_window_create(window);
    if (res) { pool_free(window); return STANDARD_RESULT_OS_WINDOWING_FAILURE; }

    window->ui_context.platform_window = window;
    window->ui_context.ui_panels_dictionary = dictionary_linear_create(g_platform_state.platform_pool, MEMORY_TAG_WINDOWING);

    darray_push(g_platform_state.windows_darray, window);

    *out_window = window;
    return STANDARD_RESULT_SUCCESS;
}

void platform_window_destroy(platform_window_st* window)
{
    LOG_DEBUG("old window darray length: %llu\n", darray_length_get(g_platform_state.windows_darray));
    darray_erase_any_matching(g_platform_state.windows_darray, window);
    os_window_destroy(window);
    pool_free(window->os_window);
    pool_free(window);
    LOG_DEBUG("new window darray length: %llu\n", darray_length_get(g_platform_state.windows_darray));
}

void platform_windowing_pre_render(void)
{
    if (darray_length_get(g_platform_state.windows_darray) == 0)
    {
        g_platform_state.is_running = false;
    }

    for (u64 i = darray_length_get(g_platform_state.windows_darray); i-- > 0; )
    {
        platform_window_st* window = g_platform_state.windows_darray[i];
        if (window->is_closing)
        {
            platform_window_destroy(window);
        }
        if (window->new_width != window->current_width)
        {
            LOG_WARN("TODO: resize swapchain/framebuffer/etc");
        }
        if (window->new_height != window->current_height)
        {
            LOG_WARN("TODO: resize swapchain/framebuffer/etc");
        }
    }
}


// ui
void platform_ui_begin(ui_context_st* context, const char* name, u64 x, u64 y, u64 width, u64 height, u64 panel_flags)
{
    LOG_WARN("UNUSED PARAMS platform_ui_begin\n", x, y, width, height, panel_flags);
    u64 window_id = fnv1a_hash_64(name);
    void* value = dictionary_linear_find_from_beginning(context->ui_panels_dictionary, window_id);
    ui_panel_st* ui_panel;
    if (value)
    {
        ui_panel = (ui_panel_st*)value;
    }
    else
    {
        ui_panel = (ui_panel_st*)pool_allocate(sizeof(ui_panel_st), g_platform_state.platform_pool, MEMORY_TAG_UI, true);
        ui_panel->context = context;
        ui_panel->name = name;
        dictionary_linear_push(context->ui_panels_dictionary, window_id, ui_panel);
    }

    context->panel_current = ui_panel;
    ui_panel->pos_x = x;
    ui_panel->pos_y = y;
    ui_panel->size_x = width;
    ui_panel->size_y = height;
    ui_panel->panel_flags = panel_flags;

    if (panel_flags & UI_FLAG_WINDOW_MINIMIZED) { context->panel_current = 0; return; }

    // check flags for moving/resizing enabled/disabled
    // update size/pos
    // border hovered/held drag window size

    // prepare command buffer
}

void platform_ui_end(ui_context_st* context)
{
    (void)context;




    // clear panel being worked on for next
    context->panel_current = 0;
}

void platform_ui_render(ui_context_st* context)
{
    (void)context;

}


// ui widgets
b32 platform_ui_widget_button(ui_context_st* context)
{
    ui_panel_st* panel = context->panel_current;
    (void)panel;

    // draw to command buffer
        // outline
        // fill
        // text

    return false;
}



