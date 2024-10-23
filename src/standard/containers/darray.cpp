#include "darray.h"

#include <string.h>

typedef struct darray_header_st {
    u64 length;
    u64 capacity;
    u64 stride;
    memory_pool_st* pool;
    memory_tag_e tag;
} darray_header_st;

#define DARRAY_RESIZE_FACTOR 2
#define darray_header_size_bytes ((sizeof(darray_header_st) + MIN_ALIGNMENT - 1) & ~(MIN_ALIGNMENT - 1))


void* _darray_create(u64 capacity, u64 stride, memory_pool_st* pool, memory_tag_e tag, const char* file, int line)
{
    u64 normalized_capacity = (capacity < 1) ? 1u : capacity;
    u64 memory_requirement = normalized_capacity * stride;

    darray_header_st* memory = (darray_header_st*)_pool_allocate(darray_header_size_bytes + memory_requirement, pool, tag, true, file, line);
    memory->capacity = normalized_capacity;
    memory->length = 0;
    memory->stride = stride;
    memory->pool = pool;
    memory->tag = tag;

    return (void*)((u8*)memory + darray_header_size_bytes);
}

void _darray_destroy(void* array, const char* file, int line)
{
    darray_header_st* darray_header = (darray_header_st*)((u8*)array - darray_header_size_bytes);
    _pool_free(darray_header, file, line);
}

void* _darray_grow(void* array, const char* file, int line)
{
    darray_header_st* original_header = (darray_header_st*)((u8*)array - darray_header_size_bytes);

    void* new_darray = _darray_create((DARRAY_RESIZE_FACTOR * original_header->capacity), original_header->stride, original_header->pool, original_header->tag, file, line);
    darray_header_st* new_header = (darray_header_st*)((u8*)new_darray - darray_header_size_bytes);
    memcpy(new_darray, array, original_header->length * original_header->stride);
    new_header->length = original_header->length;

    _darray_destroy(array, file, line);

    return new_darray;
}

void* _darray_resize(void* array, u64 new_capacity, const char* file, int line)
{
    darray_header_st* original_header = (darray_header_st*)((u8*)array - darray_header_size_bytes);
    if (original_header->capacity >= new_capacity) { return array; }

    void* new_darray = _darray_create(new_capacity, original_header->stride, original_header->pool, original_header->tag, file, line);
    darray_header_st* new_header = (darray_header_st*)((u8*)new_darray - darray_header_size_bytes);
    memcpy(new_darray, array, original_header->length * original_header->stride);
    new_header->length = original_header->length;

    _darray_destroy(array, file, line);

    return new_darray;
}

u64 darray_capacity_get(void* array)
{
    darray_header_st* header = (darray_header_st*)((u8*)array - darray_header_size_bytes);
    return header->capacity;
}

void darray_length_set(void* array, u64 new_length)
{
    darray_header_st* header = (darray_header_st*)((u8*)array - darray_header_size_bytes);
    header->length = new_length;
}

u64 darray_length_get(void* array)
{
    darray_header_st* header = (darray_header_st*)((u8*)array - darray_header_size_bytes);
    return header->length;
}

void* _darray_push(void* array, const void* value_ptr, const char* file, int line)
{
    darray_header_st* header = (darray_header_st*)((u8*)array - darray_header_size_bytes);
    if (header->length == header->capacity) // capacity maxed out
    {
        array = _darray_grow(array, file, line);
        header = (darray_header_st*)((u8*)array - darray_header_size_bytes); // new header after growth
    }

    u64 current_payload_size = header->length * header->stride;
    void* end_of_array_address = (void*)((u8*)array + current_payload_size);
    memcpy(end_of_array_address, value_ptr, header->stride);
    header->length++;

    return array;
}

void _darray_pop(void* array, void* dest)
{
    darray_header_st* header = (darray_header_st*)((u8*)array - darray_header_size_bytes);
    u8* address = (u8*)array;
    address += ((header->length - 1u) * header->stride);
    if (dest) { memcpy(dest, (void*)address, header->stride); }
    header->length--;
}

void _darray_pop_at(void* array, u64 index, void* dest)
{
    darray_header_st* header = (darray_header_st*)((u8*)array - darray_header_size_bytes);
    STANDARD_ASSERT_MSG(index < header->length, "_darray_pop_at provided index out of bounds");

    u8* address = (u8*)array;
    if (dest) { memcpy(dest, (void*)(address + (index * header->stride)), header->stride); }

    if (index != header->length - 1)
    {
        void* mem_dst = (void*)(address + (index * header->stride));
        void* mem_src = (void*)(address + ((index + 1u) * header->stride));
        u64 size = header->stride * (header->length - 1 - index);
        memcpy(mem_dst, mem_src, size);
    }

    header->length--;
}

void _darray_erase_any_matching(void* array, const void* value)
{
    darray_header_st* header = (darray_header_st*)((u8*)array - darray_header_size_bytes);
    u64 stride = header->stride;

    for (u64 i = header->length; i-- > 0; ) // last to first order
    {
        void* element = (u8*)array + (i * stride);
        if (memcmp(element, value, stride) == 0)  // memcmp returns 0 if they are equal
        {
            _darray_pop_at(array, i, NULL);
        }
    }
}


void _darray_find_and_move_to_end(void* array, const void* value)
{
    darray_header_st* header = (darray_header_st*)((u8*)array - darray_header_size_bytes);
    u64 stride = header->stride;
    u64 length = header->length;

    for (u64 i = length; i-- > 0; ) // last to first order, no reason for this if we're breaking after first find
    {
        void* element = (u8*)array + (i * stride);
        if (memcmp(element, value, stride) == 0)  // memcmp returns 0 if they are equal
        {
            // copy everything after it to the left 1 space, copy value to end of array. array stays same length
            // value = *3*  - 0  1  2 *3* 4  5
            // shift left   - 0  1  2  4  5  5
            // value to end - 0  1  2  4  5 *3*

            // address of the element just after the matched one
            void* next_element = (u8*)element + stride;

            // number of bytes to move (elements after the match)
            u64 bytes_to_move = (length - i - 1) * stride;

            // shift everything after the found element to the left
            if (bytes_to_move > 0)
            {
                memmove(element, next_element, bytes_to_move);
            }

            // copy (value) at the end of the array
            memcpy((u8*)array + ((length - 1) * stride), value, stride);

            break; // exit search loop
        }
    }
}

void darray_zero(void* array)
{
    darray_header_st* header = (darray_header_st*)((u8*)array - darray_header_size_bytes);
    memset(array, 0, header->capacity * header->stride);
    header->length = 0;
}
