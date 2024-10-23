#pragma once

#include "../common.h"

#include "platform/platform.h"

// create
void* _darray_create(u64 capacity, u64 stride, memory_pool_st* pool, memory_tag_e tag, const char* file, int line);
template<typename T>
T* darray_create(u64 capacity, memory_pool_st* pool, memory_tag_e tag) {
    void* array = _darray_create(capacity, sizeof(T), pool, tag, __FILE__, __LINE__);
    return static_cast<T*>(array);
}

// destroy
void _darray_destroy(void* array, const char* file, int line);
template<typename T>
void darray_destroy(T*& array) {
    _darray_destroy(array, __FILE__, __LINE__);
    array = nullptr;
}

// grow
void* _darray_grow(void* array, const char* file, int line);

// resize (increase capacity)
void* _darray_resize(void* array, u64 new_capacity, const char* file, int line);
template<typename T>
void darray_resize(T*& array, u64 new_capacity) {
    array = static_cast<T*>(_darray_resize(array, new_capacity, __FILE__, __LINE__));
}

// get capacity
u64 darray_capacity_get(void* array);

// set length manually (e.g., for writing data into array directly)
void darray_length_set(void* array, u64 new_length);

// get length
u64 darray_length_get(void* array);

// push
void* _darray_push(void* array, const void* value_ptr, const char* file, int line);
template<typename T>
void darray_push(T*& array, const T& value) {
    T typed_value = value;
    array = static_cast<T*>(_darray_push(array, &typed_value, __FILE__, __LINE__));
}

// pop
void _darray_pop(void* array, void* dest);
template<typename T>
T darray_pop(T*& array) {
    T value;
    _darray_pop(array, &value);
    return value;
}

// pop at
void _darray_pop_at(void* array, u64 index, void* dest);
template<typename T>
T darray_pop_at(T*& array, u64 index) {
    T value;
    _darray_pop_at(array, index, &value);
    return value;
}

// erase any matching
void _darray_erase_any_matching(void* array, const void* value);
template<typename T>
void darray_erase_any_matching(T*& array, const T& value) {
    _darray_erase_any_matching(array, &value);
}

// find and move to end
void _darray_find_and_move_to_end(void* array, const void* value);
template<typename T>
void darray_find_and_move_to_end(T*& array, const T& value) {
    _darray_find_and_move_to_end(array, &value);
}

// zero
void darray_zero(void* array);
